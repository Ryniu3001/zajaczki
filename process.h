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
	int polanasId;
	Type type;
	int worldSize;
	int responds;
	bool partying;
	Process(int id, Type type, int polanyNumber, int worldSize);
	void broadcast(int root);
	void receiveAny();
	void send(int receiver, int tag, Message msg);
	void receive(int sender, int tag);
	void respond(int receiver, int polana);
	void randPolana(int number);
	void iWannaParty();
	bool canEnterCriticalSection();
	bool isZajacAtTheParty();
	void enterCriticalSection();
	void leaveCriticalSection();
};

#endif
