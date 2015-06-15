#include "helpers.h"
#include "process.h"

using namespace std;

int main ( int argc, char *argv[] ) {

  MPI::Init (argc, argv);
  int size = MPI::COMM_WORLD.Get_size();
  int id = MPI::COMM_WORLD.Get_rank();
  srand(time(NULL) + id);
  Process *p;
  if(id < ceil(size*N)) {
    p = new Process(id, Niedzwiedz, P, size);
  } else {
    p = new Process(id, Zajac, P, size);
  }

  int counter = 0;
  bool CHOOSE_PARTY = true;
  while(counter < PARTIES) {
    if(CHOOSE_PARTY) {
      CHOOSE_PARTY = false;
      p->randPolana(P);
      p->iWannaParty();
    }

    p->receiveAny();
    if(!CHOOSE_PARTY && p->canEnterCriticalSection()){
      counter++;
      p->enterCriticalSection();
      p->leaveCriticalSection();
      p->responds = 0;
      CHOOSE_PARTY = true;
    }


  }

  delete p;
  MPI::Finalize();
  return 0;
}
