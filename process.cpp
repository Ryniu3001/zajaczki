#include "process.h"

Process::Process(int id, Type type, int polanyNumber, int worldSize) {
	this->id = id;
	this->type = type;
	this->lamport = new Lamport(id, polanyNumber);
	this->worldSize = worldSize;
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
	printf("Process %d received message from %d\n",MPI::COMM_WORLD.Get_rank(), status.Get_source());
	Demand *demand = new Demand(msg.processId, msg.clock, msg.type);
	this->lamport->addProcess(demand, msg.polana);
}

void Process::send(int receiver,int tag, Message msg){
	MPI::COMM_WORLD.Send(&msg, sizeof(struct Message), MPI_BYTE, receiver, tag);
	printf("Send from %d to %d\n", MPI::COMM_WORLD.Get_rank(), receiver);
}

void Process::receive(int sender, int tag){
	Message msg;
	MPI::COMM_WORLD.Recv(&msg, sizeof(struct Message), MPI_BYTE, sender, tag);
	printf("Process %d received message from %d\n",MPI::COMM_WORLD.Get_rank(), sender);
	this->lamport->clock = max(this->lamport->clock,msg.clock) + 1;
}

void Process::randPolana(int number) {
	srand (time(NULL));
	polanasId = rand()%number;
}

bool Process::canEnterCriticalSection() {
	return false;
}

void Process::enterCriticalSection() {}

void Process::leaveCriticalSection() {}

void Process::iWannaParty() {
	lamport->addMe(polanasId, type);
	Message msg;
	msg.clock = ++this->lamport->clock;
	msg.type = this->type;
	msg.polana = polanasId;
	msg.processId = this->id;
	for(int i=0;i<worldSize;i++) {
		if(i != this->id) {
			send(i,MPI_BYTE,msg);
		}
	}
}
