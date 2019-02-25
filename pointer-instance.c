#include <stdio.h>
#include "mpi.h"

void g(int s){
	if(s)
		MPI_Barrier(MPI_COMM_WORLD);
}


int main(int argc, char **argv) {
	int r,s=1;

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&a);
 
  MPI_Barrier(MPI_COMM_WORLD);

  g(s);
	s=r%2;

	if (s)
    MPI_Barrier(MPI_COMM_WORLD);

	MPI_Finalize();
	return 0;
}



