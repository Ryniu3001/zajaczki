#ifndef helpers_h
#define helpers_h

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <math.h>

enum Type {Niedzwiedz = 4, Zajac = 1};
enum MsgTag {Request, AnswerRequest, AnswerPermit, Release};

struct Message {
	int polana;
	int clock;
	int processId;
	Type type;
};

const int S = 10; // pojemnosc polany

#endif
