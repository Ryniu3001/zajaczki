#ifndef process_h
#define process_h

#include <algorithm>
# include "mpi.h"

struct Message {
	int polana;
	int clock;
};

class Process
{
private:
	int clock;      // wartosc zegara lamporta
public:
	Process();
	void broadcast(int root);
	void receiveAny();
	void send(int receiver, int tag);
	void receive(int sender, int tag);
};

#endif