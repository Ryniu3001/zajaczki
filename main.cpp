#include "helpers.h"
#include "process.h"

using namespace std;

int main ( int argc, char *argv[] ) {
  float N = 0.1; // procent niedzwiedzi
  float Z = 0.9; // procent zajaczkow
  int P = 20; // liczba polan
  int S = 15; // pojemnosc polany

  MPI::Init (argc, argv);
  int size = MPI::COMM_WORLD.Get_size();
  int id = MPI::COMM_WORLD.Get_rank();
  Process *p;

  if(id < ceil(size*N)) {
    p = new Process(id, Niedzwiedz, P);
  } else {
    p = new Process(id, Zajac, P);
  }
	p->broadcast(0);


  MPI::Finalize();
  return 0;
}
