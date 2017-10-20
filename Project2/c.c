//#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <mpi.h>

#define row 4
#define col 4
double array[row][col] = { {10,1,2,3}, {2,3,4,5}, {5,6,7,8}, {2,3,4,8} };

int worldMap[row];

int main(int argc, char* argv[]) {

	int i, j, k, y, my_id, nprocs;
	MPI_Init(&argc, &argv); 
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);	// get my_id for each procs
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs); // # of total procs.
//	int *worldMap = new int[row];
	for (i = 0; i < row; i++) {
		worldMap[i] = i%nprocs;
	}
//	double mat_ik[row/sqrt(nprocs)]; 
//	double mat_kj[row/sqrt(nprocs)]; 

	int my_row_begin, my_row_end;
	int my_col_begin, my_col_end;
	int n_per_row = row / sqrt(nprocs);
	my_row_begin = my_id * n_per_row;
	my_row_end = my_row_begin + n_per_row - 1;
	if (my_id == nprocs - 1 && (row - (my_row_end + 1)) > 0)   my_row_end = row - 1;
	//*******************************************************************************
	// Make array
	time_t t;
	srand((unsigned)time(&t));
	for (i = 0; i < row; i++) {
		for (j = 0; j < col; j++) {
			//array[i][j] =1+ (double)rand() / (double)(RAND_MAX / 1);
		//	printf(" %f \n",array[i][i]);
		}
	}
	//*******************************************************************************
	// print function,  should not use if large matrix
	if (my_id == 0) {
		for (i = 0; i < row; i++) {
			//cout << " " << array[i][i];
		}

	}

	double det = array[0][0];
	/*		TESSTER FUNCTION,  PRINTS OUT PRODUCT OF DIAGANONAL BEFORE CALCULATION.
	if(my_id==0) cout << det << endl;
	for (i = 1; i < row; i++) {
		det = det*array[i][i];
		if (my_id == 0) cout << array[i][i] << endl;
	}*/
	//cout << "before DETERMINATE OF MATRIX= " << det << endl;
	//Make array
	double Aik, Akj;
	int current_proc = 0;
	for (k = 0; k < row; k++) {
		current_proc = worldMap[k];
		
		if (current_proc == my_id) {
			for (i = k + 1; i < row; i++) {
				//cout << "before "<<" " << i<< " "<<k << " " <<array[i][k] <<"/"<<array[k][k]<< endl;
				array[i][k] = array[i][k] / array[k][k];
				Aik = array[i][k];
				// Broadcast A[i][k] everytime it is calculated
			}
		}
		//for (i = k + 1; i < row; i++) {
		//	MPI_Bcast(&array[i][k], 1, MPI_DOUBLE, current_proc, MPI_COMM_WORLD);
	//	}
		//if (current_proc == my_id) cout << " master " << array[k][k] << endl;
		MPI_Bcast(&array[k][k], row - k, MPI_DOUBLE, current_proc, MPI_COMM_WORLD);
		// Afterwawrds, broadcast the updated row to respectively row
		//cout << " slave"<<my_id<< "  " << array[k][k] << endl;
		for (i = k + 1; i < row; i++) {
			if (current_proc == my_id) {
				for (j = k + 1; j < row; j++) {
					//cout << array[i][j] << "-" << array[i][k] << "*" << array[k][j] << endl;
					array[i][j] = array[i][j] - array[i][k] * array[k][j];
					// Inner most loop, calculate all the values 
					//cout << "array " << i << " " << j << " ="<< array[i][k]<< endl;
				}
			}
		}
	}


	if (my_id == nprocs-1) {
		printf("Total Processors \n");
		printf("Dimension of Matrix: %dx%d \n",row,col );
		det = array[0][0];
		for (i = 1; i < row; i++) {
			det = det*array[i][i];
			//cout << "AFTER " << array[i][i] << endl;
		}
		printf("DETERMINATE OF MATRIX= %f\n",det);
	}


	MPI_Finalize();
 
 return 0;
}
