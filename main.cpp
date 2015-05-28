# include <cstdlib>
# include <iostream>
# include <iomanip>
# include <ctime>

using namespace std;

# include "mpi.h"

int main ( int argc, char *argv[] );

int main ( int argc, char *argv[] )

{
  int id;
  int p;
//
//  Initialize MPI.
//
  MPI::Init ( argc, argv );
//
//  Get the number of processes.
//
  p = MPI::COMM_WORLD.Get_size ( );
//
//  Get the individual process ID.
//
  id = MPI::COMM_WORLD.Get_rank ( );
//
//  Every process prints a hello.
//
  printf("Process %d says Hello World!\n",id);
  //cout << "  Process " << id << " says 'Hello, world!'" << endl;
//
//  Terminate MPI.
//
  MPI::Finalize ( );
  return 0;
}

