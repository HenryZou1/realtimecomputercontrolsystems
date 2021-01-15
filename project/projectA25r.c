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
	int data_avail;
	#define MAXS 5000
	float thetadeg[MAXS];
	float refdeg [MAXS];
	float thetarad[MAXS];
	float refrad [MAXS];
	
	double Kp = 25.8; 		   // Initialize Kp to 30.
double run_time = 20.0;   // Set the initial run time to 3 seconds.
double Fs = 200.0; 	   // Set the initial sampling frequency to 200 Hz.
int motor_number = 5; // See lab_layout.pdf for your motor_number.
int no_of_samples;


double Ti = 0.0245;
double Td = 0.006125;
double N = 1000;
double Tt = 0.01;
double satblk (double input_voltage)
{

	double output_voltage;

	if (input_voltage > 1.4) {

		output_voltage = 1.4;
	}

	else if (input_voltage < -1.4) {
		output_voltage = -1.4;
	}
	else  {
		output_voltage = input_voltage;
	}

	return output_voltage;
}
	
	void *Control(void *arg)
{
	double ek;
	double uk = 0;
	float motor_position;

	
	double P;
	double I;
	double D;
	double ekpre = 0;
	double dpre = 0;
	double ipre = 0;
	double T = 1/Fs;

	double a;
	double a_minus1 = 0;
	double v;



	int k = 0;

	no_of_samples = (int)(run_time*Fs);
	
	while (k < no_of_samples) {
		rtf_sem_wait(data_avail);
		motor_position = EtoR(ReadEncoder());

		// calculate tracking error
	   ek = refrad[k] - motor_position;

		// calculate control value


		P = Kp*ek;
		I = ipre + (Kp/Ti)*(ekpre)*(T) + (1/Tt)*(a_minus1)*(T);
		D = (Td/(N*T + Td))* dpre + ((Kp*Td*N)/(N*T + Td))*(ek - ekpre);
		v = P + I + D;

		uk = satblk(v);
		a = uk-v;

		DtoA(VtoD(uk));
		thetarad[k] = motor_position;
		thetadeg[k] = motor_position/M_PI*180;

		ipre = I;
		dpre = D;
		ekpre = ek;
		a_minus1 = a;

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
				if (rtf_sem_init(data_avail,0)!= 0){
				}
				Initialize(DLAB_SIMULATE, Fs, motor_number);
				if (pthread_create(&controlthread, NULL, &Control, NULL) != 0) {
		       		printf("Error creating thread.\n");
		       		exit(-1);
		  		}
				pthread_join(controlthread, NULL);
				Terminate();
				rtf_sem_destroy(data_avail);
				close(data_avail)
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
				Square(refrad, no_of_samples, Fs, mag, freq, duty);
				
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
				
				Fs = mag;
				no_of_samples = (int)(run_time*Fs);
			break;
			
			case 't':

				printf("Please select a time\n");
				scanf("%f", &mag);

				 run_time = mag;
				 no_of_samples = (int)(run_time*Fs);
			break;
			
			case 'g':
			plot(thetarad, refrad, Fs, no_of_samples, PS, "Response of Motor", "Time (seconds)", "Shaft position (degrees)");
			break;
			
			case 'h':
				plot(refrad,thetarad, Fs, MAXS, SCREEN, "Response of Motor", "Time (seconds)","haft position (degrees)");
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
