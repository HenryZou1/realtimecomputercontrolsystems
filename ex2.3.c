#include <stdio.h>
#include <time.h>
/*By Yun Peng Zou 500 628 304*/
int main()
{
   double i,a, b ,c , itr;
   char oper;
   struct timespec start_time, stop_time, sleep_time, res;
   double tmp, elapsed_time;

   clock_getres(CLOCK_REALTIME, &res);
   printf("operation +,-,/,*\n");
   scanf("%c", &oper);
   printf("enter interger value for a\n");
   scanf("%le", &a);
   printf("enter interger value for b\n");
   scanf("%le", &b);


  

   itr = 50000;
   for(i = 0; i < itr; i++)
   {
	
	if (oper == '+'){
      clock_gettime(CLOCK_REALTIME, &start_time);
      c= a+b;
      clock_gettime(CLOCK_REALTIME, &stop_time);
	}
      if (oper == '-'){
      clock_gettime(CLOCK_REALTIME, &start_time);
      c= a-b;
      clock_gettime(CLOCK_REALTIME, &stop_time);
	}
      if (oper == '/'){
      clock_gettime(CLOCK_REALTIME, &start_time);
      c= a/b;
      clock_gettime(CLOCK_REALTIME, &stop_time);
	}
	if (oper == '*'){
      clock_gettime(CLOCK_REALTIME, &start_time);
      c= a*b;
      clock_gettime(CLOCK_REALTIME, &stop_time);
	}
      tmp = (stop_time.tv_sec - start_time.tv_sec)*1e9;
      tmp  = (stop_time.tv_nsec - start_time.tv_nsec);
      elapsed_time += tmp;
   }


      printf("Time per Iteration %2lf nsec\n", elapsed_time/itr);


   return 0;
}
