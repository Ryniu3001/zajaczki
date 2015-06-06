#ifndef helpers_h
#define helpers_h

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <math.h>

enum Type {Niedzwiedz, Zajac};

struct Message {
	int polana;
	int clock;
	int processId;
	Type type;
};

#endif
