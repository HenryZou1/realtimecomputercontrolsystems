#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
int a[18][16];
int b[16][18];
struct thread_info {
   int row,col,c;
};

typedef struct thread_info thread_info_t;

void *compute_C_ij(void *arg)
{
   
 
     thread_info_t *info;
    info = (thread_info_t *)arg;
   int c=0;
   int k = 0;
   for (k=0; k<18;k++)
      c += a[info->row][k]*b[k][info->col];
   info->c=c;
   
	
   pthread_exit(NULL);
   
}

int main(void)
{
	
	pthread_t ar_thread [18][16];
        struct thread_info arr_thread[18][16];
	int r,t;
	for(r=0;r<18;r++){
		for(t =0; t<16;t++){
		arr_thread[r][t].row = r;
		arr_thread[r][t].col= t;
		
	}
   	}
	
	for(r=0;r<18;r++){
		for(t =0; t<16;t++){
		 a[r][t] = (r+1)+(t+1);	
	}
   	}
        for(r=0;r<18;r++){
		for(t =0; t<16;t++){
		 b[t][r] = (t+1)+2*(r+1);	
	}
   	}
	
	for(r=0;r<18;r++){
		for(t =0; t<16;t++){
		if (pthread_create(&ar_thread[r][t], NULL, &compute_C_ij, &arr_thread[r][t]) != 0) {
           printf("Error in creating thread 1\n");
           exit(1);
  	 }
	}
   	}
	for(r=0;r<16;r++){
		for(t =0; t<18;t++){
		printf("%d ",arr_thread[r][t].c);
  	 }
          printf("\n");
	}
   	
   
	


}
