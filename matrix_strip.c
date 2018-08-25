

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
int n , rank, p ; // Size of matrix, Rank of Proceses, No. of processes
int *A, *B, *C; // Matrix A, B, C

int main(int argc, char *argv[])
{	
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc,&argv);

	MPI_Comm_size(MPI_COMM_WORLD, &p); // Get total no of resources. 
	// Please note: assume  n = p as discussed in class.

	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get rank of processes. 
        
	MPI_Status status;

	if (rank != 0 )
	{
		// Receive data from P0 process. Because n is given by P0.
		MPI_Recv(&n, 1, MPI_INT, 0, 0 , MPI_COMM_WORLD, &status);
		
		// Reffered: https://www.geeksforgeeks.org/dynamically-allocate-2d-array-c/ 
		
		A = (int*) malloc( p/n * n * sizeof( int* ));
		// p/n*n --> Because row of matrix A is multipled to column of matrix 
		if(A == NULL)
         	{
            	fprintf(stderr, "out of memory\n"); // handle out of memory exception
            	exit(0);
         	}

		B = (int*) malloc( n * n* sizeof( int* ));
		// n*n --> Because row of matrix A is multipled to column of matrix B. 
		// Hence we need all rows and columns
		if(B == NULL)
         	{
            	fprintf(stderr, "out of memory\n"); // handle out of memory exception
            	exit(0);
         	}

		C = (int*) malloc( p /n *n * sizeof( int* ));
		// Because we need entire memory
		if(C == NULL)
         	{
            	fprintf(stderr, "out of memory\n"); // handle out of memory exception
            	exit(0);
         	}

		
		// Receive matrices A and B from P0 after it partitions them. 
		MPI_Recv(&A[0] ,p/p*n, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
		// p/n*n because Matrix A's row are needed

		MPI_Recv(&B[0] , n * n, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
		// n*n because we need all of B

		// with received data, finding matrix C
		// Reference from https://stackoverflow.com/questions/2151084/map-a-2d-array-onto-a-1d-array
		for ( int i=0 ; i< (n/p) ; i++ ) {
			for ( int j = 0 ; j<n ; j++ )
			{			
				C[i*n+j] = 0 ;	
				for ( int k = 0 ; k<n ; k++ )
					C[i*n+j] += A[i*n+k]*B[k*n+j];
			}
		}

		// send matrix C to p0
		MPI_Send(& C[0] , p/p*n , MPI_INT, 0 , 3, MPI_COMM_WORLD);


	}

	if (rank ==0 ) {

	// Responsibilities of Process 0 
	// 1. Partition Data and send
	// 2. Perform its multiplication
	// 3. Receive data 

		FILE *fp = fopen("data.txt","r"); 
		
		fscanf ( fp , "%d", & n );
		int count = p/p*n ; // Calculate each processor has how much portion of Matrix

	// Reffered: https://www.geeksforgeeks.org/dynamically-allocate-2d-array-c/ 
	// Reffered from: https://sidkashyap.wordpress.com/2013/07/27/matrix-multiplication-in-c-dynamic-from-files/
	// Reference from https://stackoverflow.com/questions/2151084/map-a-2d-array-onto-a-1d-array

		// Similarly for matrix A
		A = (int*) malloc( n *n * sizeof( int* )); // memory allocation
		if(A == NULL)
         	{
            	fprintf(stderr, "out of memory\n"); // handle out of memory exception
            	exit(0);
         	}
         for ( int r = 0 ; r < n ; r++) {
			for ( int c = 0 ; c < n ; c++ ) {
				fscanf ( fp , "%d", & A[r*n + c] );	 // read it like 1d coz of file pointer
			}
		}

		// Similarly for matrix B
		B = (int*) malloc( n *n *sizeof( int* )); // memory allocation
		if(B == NULL)
         	{
            	fprintf(stderr, "out of memory\n"); // handle out of memory exception
            	exit(0);
         	}
         for ( int r = 0 ; r < n ; r++) {
			for ( int c = 0 ; c < n ; c++ ) {
				fscanf ( fp , "%d", & B[r*n + c] );	// read it like 1d coz of file pointer
			}
		}

		// Similarly for matrix C
		C = (int*) malloc( n *n* sizeof( int* ));	
		if(C == NULL)
         	{
            	fprintf(stderr, "out of memory\n"); // handle out of memory exception
            	exit(0);
         	}
		
        // Send data to p processes
		for ( int i = 1 ; i < p ; i ++ ) 
		// i starts from 1 and is < p --> Because i++ is POST INCREMENT. 
		// i is first incremented (previous iteration of loop) and then the comaprison is made.
		{  
			// Sending two matrices and the size.
			MPI_Send(& n , 1 , MPI_INT, i , 0, MPI_COMM_WORLD);
			MPI_Send(& A[count],count , MPI_INT, i , 1, MPI_COMM_WORLD);
			MPI_Send(& B[0] , n * n , MPI_INT, i , 2, MPI_COMM_WORLD); 
					
		}
		
		for ( int i = 1 ; i < p ; i ++ ) 
			MPI_Recv(&C[count] ,count, MPI_INT, i, 3, MPI_COMM_WORLD, &status);

		// With the received data, P0 does its multiplication.
		// Reference from https://stackoverflow.com/questions/2151084/map-a-2d-array-onto-a-1d-array

		 for ( int i=0 ; i< (n/p) ; i++ ){ 
			for ( int j = 0 ; j<n ; j++ ){			
				C[i*n+j] = 0 ;	
				for ( int k = 0 ; k<n ; k++ )
					C[i*n+j] += A[i*n+k]*B[k*n+j];
			}
		}
					
		// Display the results of matrix multiplication by parallel processing.
		for ( int i=0 ; i< n ; i++ ){
			for ( int j = 0 ; j<n ; j++ )	
				printf ( "%d  " , C[i*n+j] ) ;	
		}

	}

	MPI_Finalize();
	return 0;
}