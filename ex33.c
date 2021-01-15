#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

struct thread_info {
   int maxitr;
   double exec_time;
   int oper;
};

typedef struct thread_info thread_info_t;

void *func(void *arg)
{
   struct timespec time_1, time_2;
   double exec_time;
   thread_info_t *info;
   int i, maxitr, op;
   volatile double a, b, c;

   info = (thread_info_t *)arg;
   maxitr = info->maxitr;
   op= info->oper;
   b = 2.3; c = 4.5;

   exec_time = 0.0;
   switch(op){
   case 1:
   clock_gettime(CLOCK_REALTIME, &time_1);

   for (i = 0; i < maxitr ; i++) {
         a = b+b + b+c + c+c;
   }
         
   clock_gettime(CLOCK_REALTIME, &time_2);
   break;
   case 2:
   clock_gettime(CLOCK_REALTIME, &time_1);

   for (i = 0; i < maxitr ; i++) {
         a = b*b + b*c + c*c;
   }
         
   clock_gettime(CLOCK_REALTIME, &time_2);
   break;
   case 3:
   clock_gettime(CLOCK_REALTIME, &time_1);

   for (i = 0; i < maxitr ; i++) {
         a = b/b + b/c + c/c;
   }
         
   clock_gettime(CLOCK_REALTIME, &time_2);
   break;
   case 4:
   clock_gettime(CLOCK_REALTIME, &time_1);

   for (i = 0; i < maxitr ; i++) {
         a = b-b + b-c + c-c;
   }
         
   clock_gettime(CLOCK_REALTIME, &time_2);
   break;  
   }

   exec_time = (time_2.tv_sec - time_1.tv_sec);
   exec_time = exec_time + (time_2.tv_nsec - time_1.tv_nsec)/1.0e9;

   info->exec_time = exec_time;
   printf("Exec time for thread %d = %lf sec\n",op ,exec_time);
   pthread_exit(NULL);
   
}

int main(void)
{
   pthread_t thread1, thread2, thread3 , thread4;
   thread_info_t info1, info2,info3,info4;
   double maxitr;

   maxitr = 5.0e8;

   info1.maxitr = (int)maxitr;
   info1.oper = 1;
   info2.maxitr = (int)maxitr;
   info2.oper = 2;
   info3.maxitr = (int)maxitr;
   info3.oper = 3;
   info4.maxitr = (int)maxitr;
   info4.oper = 4;

   if (pthread_create(&thread1, NULL, &func, &info1) != 0) {
           printf("Error in creating thread 1\n");
           exit(1);
   }
   

   if (pthread_create(&thread2, NULL, &func, &info2) != 0) {
           printf("Error in creating thread 2\n");
           exit(1);
   }
   

   if (pthread_create(&thread3, NULL, &func, &info3) != 0) {
           printf("Error in creating thread 3\n");
           exit(1);
   }
   

   if (pthread_create(&thread4, NULL, &func, &info4) != 0) {
           printf("Error in creating thread 3\n");
           exit(1);
   }
      


   pthread_exit(NULL);
}
