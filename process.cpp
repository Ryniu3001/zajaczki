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
		printf("%d @%d Process %d received request from %d, polana: %d\n",this->lamport->clock,this->type,MPI::COMM_WORLD.Get_rank(), status.Get_source(), msg.polana);
		Demand *demand = new Demand(msg.processId, msg.clock, msg.type);
		this->lamport->addProcess(demand, msg.polana);
		respond(status.Get_source(), msg.polana);
	}else if (status.Get_tag() == AnswerPermit)			//odpowiedz na zadanie z zezwoleniem
	{
		printf("%d @%d Process %d received answer-permit from %d, polana: %d\n",this->lamport->clock,this->type,MPI::COMM_WORLD.Get_rank(), status.Get_source(), msg.polana);
	}
	else if(status.Get_tag() == AnswerRequest)			//odpowiedz na zadanie z wlasnym zadaniem
	{
		printf("%d @%d Process %d received answer-request from %d, polana: %d\n",this->lamport->clock,this->type,MPI::COMM_WORLD.Get_rank(), status.Get_source(), msg.polana);
		Demand *demand = new Demand(msg.processId, msg.clock, msg.type);
		this->lamport->addProcess(demand, msg.polana);
		if (msg.type == Niedzwiedz)
			this->bears++;
		
	}else if (status.Get_tag() == Release){
		printf("%d @%d Process %d received release message from %d, polana: %d\n",this->lamport->clock,this->type,MPI::COMM_WORLD.Get_rank(), status.Get_source(), msg.polana);
		lamport->removeProcess(msg.processId,msg.polana);
	}
	
}

void Process::send(int receiver,int tag, Message msg){
	MPI::COMM_WORLD.Send(&msg, sizeof(struct Message), MPI_BYTE, receiver, tag);
	if (tag == Request)
		printf("%d @%d Send request from %d to %d, polana: %d\n",this->lamport->clock,this->type, MPI::COMM_WORLD.Get_rank(), receiver, msg.polana);
	else if (tag == AnswerRequest)
		printf("%d Respond for request from process %d Process: %d Polana: %d\n",this->lamport->clock,receiver,this->id,msg.polana);
		
}

void Process::receive(int sender, int tag){
	Message msg;
	MPI::COMM_WORLD.Recv(&msg, sizeof(struct Message), MPI_BYTE, sender, tag);
	printf("Process %d received message from %d\n",MPI::COMM_WORLD.Get_rank(), sender);
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
	srand (this->lamport->id);
	polanasId = rand() % number;
}

bool Process::canEnterCriticalSection() {
	if (this->responds == MPI::COMM_WORLD.Get_size() - 1)
	{
		if (lamport->checkPosition(polanasId))
			printf("%d Proces %d otrzymal wszystkie odpowiedzi || Chetnych na impreze na polanie %d jest %d -> Wchodzi\n",this->lamport->clock, this->id,polanasId,(int)lamport->polany[polanasId].size());
		else
			printf("%d Proces %d otrzymal wszystkie odpowiedzi || Chetnych na impreze na polanie %d jest %d -> Nie zmieści się\n",this->lamport->clock, this->id,polanasId,(int)lamport->polany[polanasId].size());
		return true;
	}
	return false;
}

void Process::enterCriticalSection() {
	printf("%d Zajaczek %d wchodzi na polane %d\n",lamport->clock,this->id, polanasId);
	int superDrinks = this->bears;
	int misieNaPolanie = 0;
	for (int i = 0; i < lamport->polany[polanasId].size(); i++){
		if (lamport->polany[polanasId][i]->type == Niedzwiedz)
			misieNaPolanie++;
		if ((lamport->polany[polanasId][i]->id == this->id)
			&& (this->type = Zajac) && (this->bears > 0))
		{
			printf("%d Zajaczek %d przynosi dodatkowa flaszke!\n",lamport->clock,this->id);
			this->bears -= 1;
		}		
	}
	if (this->bears > 0)
		printf("%d Za malo zajaczkow zeby przyniesc dodatkowe flaszki!\n",lamport->clock);
	if (superDrinks > (4 * misieNaPolanie))
		printf("%d Za duzo dodatkowych flaszek! Misie beda zjadac zajaczki !!\n",lamport->clock);	
}

void Process::leaveCriticalSection() {
	this->bears = 0;
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
