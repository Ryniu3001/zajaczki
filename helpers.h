#ifndef helpers_h
#define helpers_h

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <math.h>
#include <pthread.h>

enum Type {Niedzwiedz = 4, Zajac = 1};
enum MsgTag {Request, AnswerRequest, AnswerPermit, Release};

#define KNRM  "\x1B[0m" 10
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

//#define DEBUG

struct Message {
	int polana;
	int clock;
	int processId;
	Type type;
};

const int S = 30; // pojemnosc polany
const float N = 0.2; // procent niedzwiedzi
const float Z = 0.8; // procent zajaczkow
const int P = 4; // liczba polan
const int ITER = 10; // liczba iteracji

#endif
