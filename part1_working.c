#include "mpi.h"
#include <stdio.h>

#define n 800
#define master 0

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
	int my_dest, my_source,right_id, left_id;
	right_id = my_id +1;    left_id = my_id -1;
			if(right_id==num_procs)   right_id=0;
			if(left_id==-1)   left_id = num_procs -1;
			
	if(my_id==master)   {
		for(i=0;i<n;i++)   array[i]=1;
		printf("**************************************\n");
		printf("AVAILABLE PROCESSORS %d \n \n", num_procs);	
		printf("ARRAY HAS TOTAL %d ELEMENTS \n", n);
		printf("RING OF PROCESSORS SUMMATION BEGINS \n");
		printf("**************************************\n");
	}
	for(i=0; i<num_procs; i++){
			if( my_id ==i )    {
				index=my_id* ( n / num_procs);   
				chunk = n- (right_id* (n / num_procs));
				for(j=index; j < index+(n/num_procs); j++)  current_sum = current_sum + array[j];      
				
				MPI_Send(&current_sum,1     ,MPI_INT,right_id,1,MPI_COMM_WORLD);
				//printf("Sent cursum %d        to  %d     from %d \n", current_sum,right_id, my_id);
				index= right_id*(n/num_procs);
				if(my_id != (num_procs-1))  MPI_Send(&array[index], chunk, MPI_INT, right_id,2,MPI_COMM_WORLD);
			}
			if(my_id==i+1){
				index=my_id*(n/num_procs);
				chunk=n-(my_id * (n/num_procs));
				MPI_Recv(&current_sum,1     ,MPI_INT,left_id,1,MPI_COMM_WORLD,&status);
				MPI_Recv(&array[index],chunk,MPI_INT,left_id,2,MPI_COMM_WORLD,&status);
				//printf("Recv cursum %d  at %d    from %d \n", current_sum,my_id, left_id);
				//printf("Recv chunk %d   at %d    from %d \n \n",chunk,my_id,left_id);	
			}	
			if(i==(num_procs-1)  &&  my_id==master) {
				printf("**************************************\n");
				MPI_Recv(&master_sum,1, MPI_INT, num_procs-1,1, MPI_COMM_WORLD, &status);
				printf("RESULT: %d  \n",master_sum);
				
			}
					
	}
	MPI_Finalize();	
}
