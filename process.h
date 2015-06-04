#ifndef process_h
#define process_h

#include <algorithm>
#include <mpi.h>
#include "lamport.h"
#include "helpers.h"
#include <stdlib.h>

class Process
{
private:
	int id;
public:
	Lamport *lamport;
	Type type;
	Process(int id, Type type, int polanyNumber);
	void broadcast(int root);
	void receiveAny();
	void send(int receiver, int tag);
	void receive(int sender, int tag);
};

#endif
