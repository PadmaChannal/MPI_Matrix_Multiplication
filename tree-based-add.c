#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

//Reference : http://mpitutorial.com/tutorials/
// Reference: https://www.youtube.com/watch?v=RoQJNx5npF4&t=2810s
	
	int  p, procid; // no. pf processes and procssor id
	int total = 0; 
	int Tempprocid; // temporary process id's to send and receive and add them.

int main(int argc, char *argv[])
{
	int mytotal=0;
	
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	
	MPI_Init(&argc,&argv);

	MPI_Comm_size(MPI_COMM_WORLD, &p); // no. of processes 

	MPI_Comm_rank(MPI_COMM_WORLD, &procid);  // get process-id

	MPI_Status status;


		if (2*procid+1 <= p-1) { /* I have left child */
			
				// MPI_RECEIVE(data-holder,no-of-varables,)
				MPI_Recv(&Tempprocid,1,MPI_INT,2*procid+1,0,MPI_COMM_WORLD, &status);
				mytotal += Tempprocid; /*accumulate the sum from left child into mytotal */
			}

		if (2*procid+2 <= p-1) { /* I have right child */
			
				//RECEIVE(&temptotal, sizeof(int), 2*pid+2, TOTAL);
				MPI_Recv(&Tempprocid,1,MPI_INT,2*procid+2,0,MPI_COMM_WORLD, &status);
			    mytotal += Tempprocid; /*accumulate the sum from right child into mytotal */
			}

		if (procid != 0)
	
				// MPI_SEND(sender-data,no-of-varaibles,data-type, to-procid, message-label, com_world) 
				MPI_Send(&procid, 1, MPI_INT, (procid-1)/2, 0, MPI_COMM_WORLD);
	
	
			if(procid == 0) 
				total+= mytotal;

	MPI_Finalize();

	// Print result
	printf("sum is %d\n", total);
	return 0;


}
