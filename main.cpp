# include <cstdlib>
# include <iostream>
# include <iomanip>
# include <ctime>

using namespace std;

# include "process.h"

int main ( int argc, char *argv[] );

int main ( int argc, char *argv[] )

{
  int id;
  int size;

  MPI::Init ( argc, argv );
  size = MPI::COMM_WORLD.Get_size ( );
  id = MPI::COMM_WORLD.Get_rank ( );
  Process p;
  
	  p.broadcast(0);

  
  //printf("Process %d says Hello World!\n",id);

  MPI::Finalize ( );
  return 0;
}

