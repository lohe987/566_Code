#include "mpi.h"
#include <stdio.h>
#include <math.h>

#define n 800		// ARRAY SIZE
#define master 0	// MASTER
#define d 3			//DIMENSION

int array[n];
int answer[n];

int main(int argc, char *argv[]){
	
	MPI_Status  status;
	
	int my_id, num_procs,chunk, index, i,j;
	int mask,rc;  	// error code.
	int master_sum=0;
	int current_sum=0;
	//******************************************
	rc = MPI_Init(&argc,&argv);
	rc|= MPI_Comm_size(MPI_COMM_WORLD,&num_procs);
	rc|= MPI_Comm_rank(MPI_COMM_WORLD,&my_id);
	if(rc!=0)   printf("ERROR \n");	
	//******************************************
	int my_dest, my_source;
	mask = pow(2,d) - 1;
	
	if(my_id==master){
		printf("AVAILABLE PROCESORS: %d" , num_procs);
		for(i=0;i<n;i++)  array[i] = 1;	
	}
	for(i=d-1;i>-1;i--){
		mask = mask ^ (int)pow(2,i);
		if((my_id&mask)==0)	{
		//	printf(" %d  ACTIVE IN ITERATION %d \n",my_id,i);
		//If the lower i bits of my_id are 0
			if((my_id& (int)pow(2,i)) ==0)	 {
				my_dest = my_id^ (int)pow(2,i);
				//SEND X
				index = (my_dest*n/num_procs) ;
				chunk =n / pow(2,(d-i));
				MPI_Send(&array[index],chunk,MPI_INT,my_dest,1,MPI_COMM_WORLD);
				//printf("Sent from %d   to  %d   index %d    chunk %d \n",my_id,my_dest,index,chunk);
			}
			else{
				my_source=my_id^ (int)pow(2,i);
				index=(my_id*n/num_procs) ;
				chunk = n / pow(2,(d-i));
				MPI_Recv(&array[index],chunk,MPI_INT,my_source,1,MPI_COMM_WORLD,&status);			
				//printf("Recv my_id %d   from  %d   index %d    chunk %d \n",my_id,my_source,index,chunk);	
				if(i==0){	// on last iteration
						for(j=index;j<index+chunk;j++)   current_sum=current_sum + array[index];
						MPI_Send(&current_sum,1,MPI_INT,master,2,MPI_COMM_WORLD);
						printf("Sent sum %d to master    my_id: %d\n",current_sum,my_id);
				}
			}		
		}
	}
	if(my_id == master){
		for(i=0;i<num_procs;i++) {
			MPI_Recv(&current_sum,1,MPI_INT,MPI_ANY_SOURCE,2,MPI_COMM_WORLD,&status);
			master_sum=master_sum+current_sum;
		}
		for(i=0;i< n/num_procs; i++)       master_sum = master_sum + array[i];
		printf("FINAL SUM %d   \n", master_sum);
	}


	
	MPI_Finalize();
	
}
