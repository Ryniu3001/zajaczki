#ifndef lamport_h
#define lamport_h

#include <vector>
#include "helpers.h"

using namespace std;

class Demand {
public:
  int id;
  int clock;
  Type type;
  Demand(int i, int c, Type t):id(i),clock(c),type(t){}
};

class Lamport {
public:
  int clock;
  int id;
  vector< vector<Demand *> > polany;
  void addProcess(Demand *p, int polanasId);
  void addMe(int polanasId, Type type);
  void removeProcess(int id, int polanasId);
  bool checkPosition(int polanasId);
  Lamport(int id, int polanyNumber);
};

#endif
