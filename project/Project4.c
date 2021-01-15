


#include "dlab.h"






// Declare global variables (common data), for example:
#define MAXS 5000 // Maximum no of samples
						// Increase value of MAXS for more samples
#define pi 3.1415926535

// The float type can represent values ranging from approximately 
// 1.5 x 10-45 to 3.4 x 1038, with a precision — the limit of digits — of seven.
float theta[MAXS]; // Array for storing motor position in rads
float theta_deg[MAXS]; // Array for storing motor position in degrees
float ref[MAXS]; // Array for storing reference input in radians (not degrees)
float ref_deg[MAXS]; // Array for storing reference input in degrees

double Kp = 30.0; 		   // Initialize Kp to 30.
double run_time = 5.0;   // Set the initial run time to 5 seconds.
double Fs = 200.0; 	   // Set the initial sampling frequency to 200 Hz.
int motor_number = 2; // See lab_layout.pdf for your motor_number.
int no_of_samples;



	int num_samples_3sec = 600; 	 // Fs*3 sec
	float three_second_data_ref [MAXS];
	float three_second_data_output [MAXS];



// Additional variables for Task 2.3
double Ti = 0.02;
double Td = 0.005;
double N = 20;
double Tt = 0.01;

sem_t data_avail; // Do not change the name of this semaphore
// sem_t wait_next_sample;


int stop = 0;  // stop continuous running of motor

int proceed = 0;


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




// This thread function is for control
void *Control(void *arg)
{

	int counter = 0;

	double ek;
	double uk = 0;
	float motor_position;

	// Additional variables for Task 2.3
	double P;
	double I;
	double D;
	double ek_minus1 = 0;
	double D_minus1 = 0;
	double I_minus1 = 0;
	double T = 1/Fs;

	double a;
	double a_minus1 = 0;
	double v;

	int z;

	while (stop == 0) {
		int k = 0;

		no_of_samples = (int)(run_time*Fs);
	
		while ((k < no_of_samples) && (stop == 0)) {
			sem_wait(&data_avail);
			// sem_wait(&wait_next_sample);
			proceed = 0;
			motor_position = EtoR(ReadEncoder());

			// calculate tracking error
			ek = ref[k] - motor_position;

			// calculate control value


			P = Kp*ek;
			I = I_minus1 + (Kp/Ti)*(ek_minus1)*(T) + (1/Tt)*(a_minus1)*(T);
			D = (Td/(N*T + Td))* D_minus1 + ((Kp*Td*N)/(N*T + Td))*(ek - ek_minus1);
			v = P + I + D;

			uk = satblk(v);
			a = uk-v;

			// sem_post(&wait_next_sample);  // release semaphore 
			proceed = 1;
			DtoA(VtoD(uk));
			theta[k] = motor_position;
			theta_deg[k] = motor_position/pi*180;

			I_minus1 = I;
			D_minus1 = D;
			ek_minus1 = ek;
			a_minus1 = a;


			// Remembering the last 3 seconds

			if (counter <= (num_samples_3sec-1))	{
				three_second_data_ref [counter] = ref_deg[k];
				three_second_data_output [counter] = theta_deg[k];
			
				counter++;

			}


			if (counter > (num_samples_3sec-1)) {

					for( z = 0; z < (num_samples_3sec-1); z++ )	{
							three_second_data_ref [z] = three_second_data_ref [z+1];
							three_second_data_output [z] = three_second_data_output [z+1];
					
						}


					three_second_data_ref [num_samples_3sec-1 ] = ref_deg[k];
					three_second_data_output [num_samples_3sec-1] = theta_deg[k];		

			}
			

					k++;
		}
	}

	pthread_exit(NULL);

}



// This thread function is for the 2nd menu for motor while motor is running
void *Second_Menu(void *arg)
{

		char selection;
		double fresh_input;

		printf("Second Menu selections: \n");
		printf("f: Change value of sample freq \n");
		printf("p: Change value of Kp \n");
		printf("i: Change value of Ti \n");
		printf("d: Change value of Td \n");
		printf("n: Change value of N \n");
		printf("s: Stop \n");



	while (stop == 0) {


		printf("Choose your selection \n");
		scanf(" %c", &selection);  //Get the input from the user

		switch (selection) {


		case 'f':

			// Prompt user for sample_freq;
			printf("What is the new value of the sample frequency in Hz? \n");
			scanf("%lf", &fresh_input);  //Get the input from the user
			getchar();
		// 	sem_wait(&wait_next_sample);

			while (proceed == 0) { }
			if (proceed == 1) {
			Fs = fresh_input; 
			no_of_samples = (int)(run_time*Fs);
			num_samples_3sec = Fs*3;
			}
		//	sem_post(&wait_next_sample);

			break;



		case 'p':

			// Prompt user for new value of Kp;
			printf("What is the new value of Kp? \n");
			scanf("%lf", &fresh_input);  //Get the input from the user
			getchar();
		//	sem_wait(&wait_next_sample);
			while (proceed == 0) { }
			if (proceed == 1) {
			Kp = fresh_input; 
			}
		//	sem_post(&wait_next_sample);

			break;


		case 'i':

			printf("What is the new value of Ti? \n");
			scanf("%lf", &fresh_input);  //Get the input from the user
			getchar();
		//	sem_wait(&wait_next_sample);
			while (proceed == 0) { }
			if (proceed == 1) {
			Ti = fresh_input; 
			}
		//	sem_post(&wait_next_sample);
			break;


		case 'd':

			printf("What is the new value of Td? \n");
			scanf("%lf", &fresh_input);  //Get the input from the user
			getchar();
			// sem_wait(&wait_next_sample);
			while (proceed == 0) { }
			if (proceed == 1) {
			Td = fresh_input; 
			}
			// sem_post(&wait_next_sample);
			break;


		case 'n':

			printf("What is the new value of N? \n");
			scanf("%lf", &fresh_input);  //Get the input from the user
			getchar();
			// sem_wait(&wait_next_sample);
			while (proceed == 0) { }
			if (proceed == 1) {
			N = fresh_input; 
			}
			// sem_post(&wait_next_sample);

			break;

		case 's':

			printf("Stopping motor \n");
			stop = 1;
			break;
		
		default:
			printf("Sir Error: Invalid selection! \n");

			
		}  // For Switch and Case
	}  // For while loop
	


	pthread_exit(NULL);

}




// This is the main program of server.c
int main(void *arg)
{


	char selection;
	int input;
	float magnitude;
	float frequency;
	float duty_cycle;
	pthread_t control_1;
	pthread_t menu_2;


	no_of_samples = (int)(run_time*Fs);

	// Initialize the reference input vector ref[k]. ref is in radians.
	int i = 0;
	while (i < no_of_samples) {

		ref[i] = 50*pi/180.0;
		ref_deg[i] = 50;
		i++;
	}




		printf("Menu selections: \n");
		printf("Note: Enter the inputs last. \n");
		printf("r: Run the control algorithm \n");
		printf("p: Change value of Kp \n");
		printf("f: Change value of sample freq \n");
		printf("t: Change value of run time \n");
		printf("i: Change value of Ti \n");
		printf("d: Change value of Td \n");
		printf("n: Change value of N \n");
		printf("u: Change the type of inputs (step or square) \n");
		printf("   For Step input, you will be prompt for the magnitude of the step \n");
		printf("   For Square input, you will be prompt for the magnitude, frequency and duty cycle \n");
		printf("g: Plot results on screen \n");
		printf("h: Save the Plot results in Postscript \n");
		printf("q: exit \n");


           /* https://cboard.cprogramming.com/c-programming/143734-compiler-keeps-skipping-over-scanf-statement-please-help.html */
	   /* Adak said "scanf() has a problem with %c. Since you are expecting just a char, and the newline left behind by scanf() always, 		   is a char, it will pull off one newline char '\n', from the keyboard buffer, and seem to "skip" the input completely.

	   In reality, it didn't skip anything. It took the newline, thought that was what you wanted, and continued.

	   Add a getchar() after each scanf(), to pull the newlines off the keyboard buffer."  */

	while (1) {

		
		printf("Choose your selection \n");
		scanf(" %c", &selection);  //Get the input from the user


		switch (selection) {


		case 'r':

			stop = 0;
			sem_init(&data_avail, 0, 0);
   		// sem_init(&wait_next_sample, 0, 1);
			Initialize(DLAB_SIMULATE, Fs, motor_number);

			if (pthread_create(&control_1, NULL, &Control, NULL) != 0) {
				 printf("Error creating control thread.\n");
				 exit(-1);
			}


			if (pthread_create(&menu_2, NULL, &Second_Menu, NULL) != 0) {
				 printf("Error creating second menu thread.\n");
				 exit(-1);
			}

 		   pthread_join(control_1, NULL);
 		   pthread_join(menu_2, NULL);


			Terminate();
			sem_destroy(&data_avail);
			// sem_destroy(&wait_next_sample);

			getchar();  // put getchar() here to remove the newline character generated from code above
			break;


		case 'u':
			// prompt user for type of input: Step or Square
			printf("For step input, type 1.  For square input, type 2. \n");
			scanf("%d", &input);  //Get the input from the user
			getchar();
			if (input == 1) {
				// prompt for magnitude of the step
				printf("What is the magnitude of the step in degrees? \n");
				scanf("%f", &magnitude);  //Get the input from the user
				getchar();

				//set up the step reference input {ref[k]}. ref is in radians.
				no_of_samples = (int)(run_time*Fs);
				i = 0;
				while (i < no_of_samples) {

					ref[i] = magnitude*pi/180.0;
					ref_deg[i] = magnitude;

					i++;
				}
			}
			if (input == 2) {
				// prompt for magnitude, frequency and duty cycle
				printf("What is the magnitude of the square in degrees? \n");
				scanf("%f", &magnitude);  //Get the input from the user
				getchar();

				printf("What is the frequency of the square in Hz? \n");
				scanf("%f", &frequency);  //Get the input from the user
				getchar();

				printf("What is the duty cycle of the square in %? \n");
				scanf("%f", &duty_cycle);  //Get the input from the user
				getchar();

				// set up {ref[k]} using DLaB function Square().  ref is in radians.
				no_of_samples = (int)(run_time*Fs);

				Square (ref, no_of_samples, Fs, magnitude*pi/180.0, frequency, duty_cycle);


				i = 0;
				while (i < no_of_samples) {

					ref_deg[i] = ref[i]/pi*180.0;

					i++;
				}

			}

			break;


		case 'f':

			// Prompt user for sample_freq;
			printf("What is the new value of the sample frequency in Hz? \n");
			scanf("%lf", &Fs);  //Get the input from the user
			getchar();
			no_of_samples = (int)(run_time*Fs);

			break;

		case 'p':

			// Prompt user for new value of Kp;
           /* https://cboard.cprogramming.com/c-programming/143734-compiler-keeps-skipping-over-scanf-statement-please-help.html */
	   /* Adak said "scanf() has a problem with %c. Since you are expecting just a char, and the newline left behind by scanf() always, 		   is a char, it will pull off one newline char '\n', from the keyboard buffer, and seem to "skip" the input completely.

	   In reality, it didn't skip anything. It took the newline, thought that was what you wanted, and continued.

	   Add a getchar() after each scanf(), to pull the newlines off the keyboard buffer."  */
			printf("What is the new value of Kp? \n");
			scanf("%lf", &Kp);  //Get the input from the user
			getchar();

			break;


		case 'i':

			printf("What is the new value of Ti? \n");
			scanf("%lf", &Ti);  //Get the input from the user
			getchar();

			break;


		case 'd':

			printf("What is the new value of Td? \n");
			scanf("%lf", &Td);  //Get the input from the user
			getchar();

			break;


		case 'n':

			printf("What is the new value of N? \n");
			scanf("%lf", &N);  //Get the input from the user
			getchar();

			break;



		case 't':

			// Prompt user for new value of run_time;
			printf("What is the new value of run time in seconds? \n");
			scanf("%lf", &run_time);  //Get the input from the user
			getchar();
			no_of_samples = (int)(run_time*Fs);

			break;


		case 'g':

			// Plot the results on screen
			// Plot the graph of v1 and v2 vs time on the screen
			plot(three_second_data_ref, three_second_data_output, Fs, num_samples_3sec, SCREEN, "Response of Motor", "Time (seconds)", "Load Shaft position (degrees)");

			getchar();  // put getchar() here to remove the newline character generated from code above

			break;


		case 'h':

			// Save the plot results in Postscript;
			// Save the graph of v1 and v2 vs time in Postscript
			plot(three_second_data_ref, three_second_data_output, Fs, num_samples_3sec, PS, "Response of Motor", "Time (seconds)", "Load Shaft position (degrees)");


			getchar();  // put getchar() here to remove the newline character generated from code above
			break;


		case 'z':

			// Save the plot results in Postscript;
			// Save the graph of v1 and v2 vs time in Postscript
			// Plot the results on screen
			// Plot the graph of v1 and v2 vs time on the screen
			plot(theta_deg, ref_deg, Fs, no_of_samples, SCREEN, "Response of Motor", "Time (seconds)", "Load Shaft position (degrees)");
			getchar();  // put getchar() here to remove the newline character generated from code above
			break;

		case 'q':

			printf("All done! \n");
			exit(0);
		
		default:
			printf("Error: Invalid selection! \n");

			
		}  // For Switch and Case
	}  // For while loop
}  // For int main(void *arg)




