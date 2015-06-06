#include "helpers.h"
#include "process.h"

using namespace std;

int main ( int argc, char *argv[] ) {
  float N = 0.2; // procent niedzwiedzi
  float Z = 0.8; // procent zajaczkow
  int P = 3; // liczba polan
  int ITER = 10; // liczba iteracji

  MPI::Init (argc, argv);
  int size = MPI::COMM_WORLD.Get_size();
  int id = MPI::COMM_WORLD.Get_rank();
  Process *p;
  if(id < ceil(size*N)) {
    p = new Process(id, Niedzwiedz, P, size);
  } else {
    p = new Process(id, Zajac, P, size);
  }
  
  int counter = 0;
  bool CHOOSE_PARTY = true;
  while(counter < 20) {
    if(CHOOSE_PARTY) {
      counter++;
      CHOOSE_PARTY = false;
      p->randPolana(P);
      p->iWannaParty();
    }

    p->receiveAny();
    if(!CHOOSE_PARTY && p->canEnterCriticalSection()){
      p->enterCriticalSection();
      p->leaveCriticalSection();
      CHOOSE_PARTY = true;
      p->responds = 0;
    }
  }

  MPI::Finalize();
  return 0;
}
