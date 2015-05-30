#include "process.h"

Process::Process() {
	this->clock = 0;
}

void Process::broadcast(int root)
{
	this->clock += 1;
	Message msg;
	msg.polana = 5;
	msg.clock = this->clock;
	MPI::COMM_WORLD.Bcast(&msg, sizeof(struct Message), MPI_BYTE, root);
	if (MPI::COMM_WORLD.Get_rank() == root)
	{
		printf("Broadcast from %d with clock %d\n",root,this->clock);
	}
	else
	{
		this->clock = std::max(this->clock,msg.clock) + 1;
		printf("Process %d received broadcast, clock %d\n",MPI::COMM_WORLD.Get_rank(), this->clock);
	}
		
}

void Process::receiveAny(){
	Message msg;
	MPI::Status status;
	MPI::COMM_WORLD.Recv(&msg, sizeof(struct Message), MPI_BYTE, MPI::ANY_SOURCE, MPI::ANY_TAG, status);
	this->clock = std::max(this->clock,msg.clock) + 1;
	printf("Process %d received message from %d\n",MPI::COMM_WORLD.Get_rank(), status.Get_source());
}

void Process::send(int receiver,int tag){
	this->clock += 1;
	Message msg;
	msg.polana = 5;
	msg.clock = this->clock;
	MPI::COMM_WORLD.Send(&msg, sizeof(struct Message), MPI_BYTE, receiver, tag);
	printf("Send from %d to %d\n", MPI::COMM_WORLD.Get_rank(), receiver);
}

void Process::receive(int sender, int tag){
	Message msg;
	MPI::COMM_WORLD.Recv(&msg, sizeof(struct Message), MPI_BYTE, sender, tag);
	printf("Process %d received message from %d\n",MPI::COMM_WORLD.Get_rank(), sender);
	this->clock = std::max(this->clock,msg.clock) + 1;
}