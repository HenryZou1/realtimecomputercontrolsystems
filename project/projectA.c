#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <mqueue.h>
#include <errno.h>
#include <math.h>
#include "dlab.h"
	pthread_t controlthread;
	sem_t data_avail;
	#define MAXS 5000
	float thetadeg[MAXS];
	float refdeg [MAXS];
	float thetarad[MAXS];
	float refrad [MAXS];
	int Kp = 1.0;
	int run_time = 3.0;
	int Fs = 200.0;
	int motor_number = 5;
	int no_of_samples;
	
void *Control(void *arg)
{
	int k = 0;
	no_of_samples = (int)(run_time*Fs);
	float motor_position;
	float ek;
	float uk;
	while (k < no_of_samples) {
		sem_wait(&data_avail);
		motor_position = EtoR(ReadEncoder());
		ek = refrad[k] - motor_position;
		uk = Kp*ek;
		DtoA(VtoD(uk));
		thetarad[k] = motor_position;
		thetadeg[k] = motor_position/M_PI*180;
		k++;
	}
	pthread_exit(NULL);
}

int main(void *arg)
{

	char oper, type;
	float mag, freq, duty;
    int i;
	no_of_samples = (int)(run_time*Fs);
	for(i=0; i<MAXS; i++){
			refrad[i] = 50*M_PI/180.0;
			refdeg[i] = 50;
	}
	while(1){
		printf("Please select a option\n");
		scanf(" %c", &oper);
		
		switch(oper){
			case'r':
				printf("Running code.\n");
				sem_init(&data_avail, 0, 0);
				Initialize(DLAB_SIMULATE, Fs, motor_number);
				if (pthread_create(&controlthread, NULL, &Control, NULL) != 0) {
		       		printf("Error creating thread.\n");
		       		exit(-1);
		  		}
				pthread_join(controlthread, NULL);
				Terminate();
				sem_destroy(&data_avail);
				break;
			case 'u':
			printf("Please select a input s for step q for square\n");			
			scanf(" %c", &type);
			
			if (type == 's') {
			//prompt for magnitude of the step
			//set up the step reference input {ref[k]}
				printf("Please enter a magnitude for step input\n");	
				scanf("%f", &mag);
				for(i=0; i<no_of_samples; i++){
					refrad[i] = mag*M_PI/180.0;
					refdeg[i] = mag;
				}
			}
			if (type == 'q') {
				printf("Please select a magnitude in degree\n");
				scanf("%f", &mag);
				printf("Please select a frequency in degree\n");
				scanf("%f", &freq);
				printf("Please select a duty in degree\n");
				scanf("%f", &duty);
				Square(refrad, no_of_samples, Fs, mag*M_PI/180.0, freq, duty);
				for(i=0; i<no_of_samples; i++){
					refrad[i] = mag*M_PI/180.0;
					refdeg[i] = mag;
				}
			}
			
			break;

			
			case 'p':
			//Prompt user for new value of Kp;
				printf("Please select a kp\n");
				scanf("%f", &mag);
				Kp = mag;
			break;
			
			case 'f':
			//Prompt user for new value of Kp;
				printf("Please select a sampling frequency\n");
				scanf("%f", &mag);
				no_of_samples = (int)(run_time*Fs);
				Fs = mag;
			break;
			
			case 't':

				printf("Please select a time\n");
				scanf("%f", &mag);
				 run_time = mag;
			break;
			
			case 'g':
			plot(thetadeg, refdeg, Fs, no_of_samples, PS, "Response of Motor", "Time (seconds)", "Load Shaft position (degrees)");
			break;
			
			case 'h':
				plot(refdeg,thetadeg, Fs, MAXS, SCREEN, "GRAPH Title", "x-axis","y-axis");
			break;

			case 'q':
			//We are done!
			exit(0);

			default:
				printf("Invalid \n" );
			break;
		}
	}
	exit(0);
}
