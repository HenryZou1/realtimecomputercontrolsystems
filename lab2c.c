#include <stdio.h>
#include <math.h>
#include <time.h>

int main()
{
   double a, b ,c,maxitr ;
	int i;
   int op;
   struct timespec time_1, time_2;
   double tmp, exec_time;

   printf("operation +,-,/,*\n");
   scanf("%d", &op);
   printf("enter interger value for a\n");
   scanf("%le", &a);
   printf("enter interger value for b\n");
   scanf("%le", &b);


  
   exec_time=0.0;
   maxitr = 5.0e8;
	  switch(op){
   case 1:
   clock_gettime(CLOCK_REALTIME, &time_1);

   for (i = 0; i < maxitr ; i++) {
         c= a+b;
   }
         
   clock_gettime(CLOCK_REALTIME, &time_2);
   break;
   case 2:
   clock_gettime(CLOCK_REALTIME, &time_1);

   for (i = 0; i < maxitr ; i++) {
         c= a-b;
   }
         
   clock_gettime(CLOCK_REALTIME, &time_2);
   break;
   case 3:
   clock_gettime(CLOCK_REALTIME, &time_1);

   for (i = 0; i < maxitr ; i++) {
         c=a/b;
   }
         
   clock_gettime(CLOCK_REALTIME, &time_2);
   break;
   case 4:
   clock_gettime(CLOCK_REALTIME, &time_1);

   for (i = 0; i < maxitr ; i++) {
         c= a*b;
   }
         
   clock_gettime(CLOCK_REALTIME, &time_2);
   break;  
   }

   exec_time = (time_2.tv_sec - time_1.tv_sec)*1.0e9;
   exec_time = exec_time + (time_2.tv_nsec - time_1.tv_nsec);
printf("Time per Iteration %2lf nsec\n", exec_time);
printf("Time per Iteration %2lf nsec\n", maxitr);
	     
	      printf("Time per Iteration %.15lf nsec\n", exec_time/maxitr);

   return 0;
}
