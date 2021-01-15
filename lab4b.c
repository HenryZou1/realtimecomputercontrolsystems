#include <stdio.h>       /* for fprintf() */
#include <stdlib.h>      /* for exit() */
#include <unistd.h>      /* for sleep() */
#include <pthread.h>     /* for pthreads functions */
#define NTHREADS 3
pthread_mutex_t lock;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;
int string_index = 0;

char string_to_print[] = "0123456789";

void *func(void *arg)
{
   int tmp_index;

   sleep(1);   /* sleep 1 sec to allow all threads to start */

   while (1) {
      pthread_mutex_lock(&lock);
      
      while (string_index > 4){
          pthread_cond_wait(&c,&lock);
      }
      
      tmp_index = string_index;
      if (!(tmp_index+1 > sizeof(string_to_print)/2)) {
         printf("A%c ", string_to_print[tmp_index]);
         usleep(1);   /* sleep 1 usec */
      }
      string_index = tmp_index + 1;

      if (tmp_index+1 > 4) {
         pthread_cond_signal(&c);
      }
      
      pthread_mutex_unlock(&lock);
   }
}

void *funcb(void *arg)
{
   int tmp_index;

   sleep(1);   /* sleep 1 sec to allow all threads to start */
  
   while (1) {
       pthread_mutex_lock(&lock);
      
      while(string_index<= 4){
          pthread_cond_wait(&c,&lock);
      }
      tmp_index = string_index;
	
      if (!(tmp_index+2 > sizeof(string_to_print))) {
         printf("B%c ", string_to_print[tmp_index]);
         usleep(1);   /* sleep 1 usec */
      }
      string_index = tmp_index + 1;

      	

      if (tmp_index +1> sizeof(string_to_print)) {
         printf("\n");
         string_index = 0;   /* wrap around */
         	
      }
      if (string_index  <= 4) {
         
         pthread_cond_signal(&c);
        
      }
      pthread_mutex_unlock(&lock);
   }

}



int main(void)
{
   pthread_t threads[NTHREADS];
   int k;

   
      printf("Starting Thread %d\n", 1);
      if (pthread_create(&threads[k], NULL, &func, NULL) != 0) {
            printf("Error creating thread %d\n", k+1);
            exit(-1);
      }
      printf("Starting Thread %d\n", 2);
      if (pthread_create(&threads[k], NULL, &funcb, NULL) != 0) {
            printf("Error creating thread %d\n", 2);
            exit(-1);
      }
 

   sleep(20);   /* sleep 20 secs to allow time for the threads to run */
                /* before terminating them with pthread_cancel()      */

   for (k = 0; k < NTHREADS; k++) {
      pthread_cancel(threads[k]);
   }

   pthread_exit(NULL);
}
