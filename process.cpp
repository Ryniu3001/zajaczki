#include "process.h"

Process::Process(int id, Type type, int polanyNumber, int worldSize) {
	this->id = id;
	this->type = type;
	this->lamport = new Lamport(id, polanyNumber);
	this->worldSize = worldSize;
	this->responds = 0;
}

void Process::broadcast(int root)
{
	this->lamport->clock++;
	Message msg;
	msg.polana = rand()%5;
	msg.clock = this->lamport->clock;
	MPI::COMM_WORLD.Bcast(&msg, sizeof(struct Message), MPI_BYTE, root);
	if (MPI::COMM_WORLD.Get_rank() == root)
	{
		printf("@%d: Broadcast from %d with clock %d\n",this->type, root,this->lamport->clock);
	}
	else
	{
		this->lamport->clock = std::max(this->lamport->clock,msg.clock) + 1;
		printf("@%d Process %d received broadcast, clock %d\n",this->type,MPI::COMM_WORLD.Get_rank(), this->lamport->clock);
	}

}

void Process::receiveAny(){
	Message msg;
	MPI::Status status;
	MPI::COMM_WORLD.Recv(&msg, sizeof(struct Message), MPI_BYTE, MPI::ANY_SOURCE, MPI::ANY_TAG, status);
	this->lamport->clock = std::max(this->lamport->clock,msg.clock) + 1;

	if ((status.Get_tag() == AnswerRequest)
		|| (status.Get_tag() == AnswerPermit) && (msg.polana == polanasId))
		responds++;

	if (status.Get_tag() == Request)
	{
		#ifdef DEBUG
		printf("%d @%d Process %d received request from %d, polana: %d\n",
		       this->lamport->clock,this->type,MPI::COMM_WORLD.Get_rank(), status.Get_source(), msg.polana);
		#endif
		Demand *demand = new Demand(msg.processId, msg.clock, msg.type);
		this->lamport->addProcess(demand, msg.polana);
		respond(status.Get_source(), msg.polana);
	}else if (status.Get_tag() == AnswerPermit)			//odpowiedz na zadanie z zezwoleniem
	{
		#ifdef DEBUG
		printf("%d @%d Process %d received answer-permit from %d, polana: %d Received Answers: %d\n",
		       this->lamport->clock,this->type,MPI::COMM_WORLD.Get_rank(), status.Get_source(), msg.polana, responds);
		#endif
	}
	else if(status.Get_tag() == AnswerRequest)			//odpowiedz na zadanie z wlasnym zadaniem
	{
		if(this->polanasId == msg.polana) {
			#ifdef DEBUG
			printf("%d @%d Process %d received answer-request from %d, polana: %d Received Answers: %d\n",
			       this->lamport->clock,this->type,MPI::COMM_WORLD.Get_rank(), status.Get_source(), msg.polana,responds);
			#endif
			Demand *demand = new Demand(msg.processId, msg.clock, msg.type);
			this->lamport->addProcess(demand, msg.polana);
		}

	}else if (status.Get_tag() == Release){
		#ifdef DEBUG
		printf("%d @%d Process %d received release message from %d, polana: %d\n",
		       this->lamport->clock,this->type,MPI::COMM_WORLD.Get_rank(), status.Get_source(), msg.polana);
		#endif
		lamport->removeProcess(msg.processId,msg.polana);
	}

}

void Process::send(int receiver,int tag, Message msg){
	MPI::COMM_WORLD.Send(&msg, sizeof(struct Message), MPI_BYTE, receiver, tag);
	if (tag == Request) {
		#ifdef DEBUG
			printf("%d @%d Send request from %d to %d, polana: %d\n",
			       this->lamport->clock,this->type, MPI::COMM_WORLD.Get_rank(), receiver, msg.polana);
		#endif
	}	else if (tag == AnswerRequest) {
		#ifdef DEBUG
			printf("%d @%d Process %d send respond for request from process %d Polana: %d\n",
			       this->lamport->clock,this->type,this->id,receiver,msg.polana);
		#endif
	}


}

void Process::receive(int sender, int tag){
	Message msg;
	MPI::COMM_WORLD.Recv(&msg, sizeof(struct Message), MPI_BYTE, sender, tag);
	#ifdef DEBUG
	printf("Process %d received message from %d\n",MPI::COMM_WORLD.Get_rank(), sender);
	#endif
	this->lamport->clock = max(this->lamport->clock,msg.clock) + 1;
}

void Process::respond(int receiver, int polana){
	Message msg;
	msg.clock = ++this->lamport->clock;	//Wysylamy odpowiedz z aktualnym clock poniewaz i tak proces odbierajacy
	msg.type = this->type;			//odpowiedz nie doda do listy tego zadania poniewaz tez proces juz bedzie na jego liscie
	msg.polana = polana;
	msg.processId = this->id;
	if (polanasId == polana){					//Jesli odpowiadamy na zadanie wejscia do tej samej polany na ktora my chcemy wejsc
		send(receiver, AnswerRequest, msg);
	}else{
		send(receiver,AnswerPermit, msg);
	}
}
void Process::randPolana(int number) {
	polanasId = rand() % number;
}

bool Process::canEnterCriticalSection() {
	if (this->responds == MPI::COMM_WORLD.Get_size() - 1)
	{
		if (lamport->checkPosition(polanasId, this->id)) {
			#ifdef DEBUG
			printf("%d @%d %d otrzymal wszystkie odpowiedzi | Chetnych na impreze na polanie %d jest %d -> Wchodzi\n",
			       this->lamport->clock, this->type, this->id,polanasId,(int)lamport->polany[polanasId].size());
			#endif
			return true;
		}
		else {
			#ifdef DEBUG
			printf("%d @%d %d otrzymal wszystkie odpowiedzi | Chetnych na impreze na polanie %d jest %d -> Nie zmieści się\n",
			       this->lamport->clock, this->type, this->id,polanasId,(int)lamport->polany[polanasId].size());
			#endif
			return false;
		}
	}
	//printf("%d Za malo odzpowiedzi. Proces %d\n",lamport->clock,this->id); //Tylko w celach debugu
	return false;
}

bool Process::isZajacAtTheParty() {

	for(int i=0;i<lamport->polany[polanasId].size();i++) {
		if(lamport->polany[polanasId][i]->type == Zajac) {
			return true;
		}
		if(lamport->polany[polanasId][i]->id == this->id) {
			return false;
		}
	}

	return false;
}

void Process::enterCriticalSection() {
	printf("%s%d @%d %d wchodzi na polane %d \033[0m\n",KCYN, lamport->clock,this->type, this->id, polanasId);
	if(this->type != Niedzwiedz) {
		int misieNaPolanie = 0;
		for (int i = 0; i < lamport->polany[polanasId].size(); i++){
			if (lamport->checkPosition(polanasId, lamport->polany[polanasId][i]->id) && lamport->polany[polanasId][i]->type == Niedzwiedz) {
				printf("%d Misiu %d na polanie %d \n",lamport->clock, lamport->polany[polanasId][i]->id, polanasId);
				misieNaPolanie++;
			}

		}
		int i = 0;
		int liczbaZajaczkowPrzedeMna = 0;
		while(lamport->polany[polanasId][i]->id != this->id) {
			if((lamport->polany[polanasId][i]->type == Zajac) && (lamport->polany[polanasId][i]->id != this->id)) {
				liczbaZajaczkowPrzedeMna++;
			}
			i++;
		}
		if(misieNaPolanie > 0 && liczbaZajaczkowPrzedeMna + i - 1 < misieNaPolanie) {
			printf("%s%d @%d %d przynosi alko na polane %d.\033[0m\n",KGRN , lamport->clock, this->type, this->id, polanasId);

			printf("%d @%d %d | %d mis(ie) na polanie %d \n",lamport->clock, this->type, this->id, misieNaPolanie, polanasId);

		}
	}
}

void Process::leaveCriticalSection() {
	lamport->removeProcess(this->id, this->polanasId);
	Message msg;
	msg.type = this->type;
	msg.polana = polanasId;
	msg.processId = this->id;
	for(int i=0;i<worldSize;i++) {
		if(i != this->id) {
			msg.clock = ++this->lamport->clock;
			send(i,Release,msg);
		}
	}
	printf("%s%d @%d %d wychodzi z polany %d \033[0m\n",KYEL, lamport->clock,this->type, this->id, polanasId);
}

void Process::iWannaParty() {
	lamport->addMe(polanasId, type);
	Message msg;
	msg.type = this->type;
	msg.polana = polanasId;
	msg.processId = this->id;
	for(int i=0;i<worldSize;i++) {
		if(i != this->id) {
			msg.clock = ++this->lamport->clock;
			send(i,Request,msg);
		}
	}
}
