
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

double Kp = 1.0; 		   // Initialize Kp to 1.
double run_time = 20.0;   // Set the initial run time to 3 seconds.
double Fs = 200.0; 	   // Set the initial sampling frequency to 200 Hz.
int motor_number = 2; // See lab_layout.pdf for your motor_number.
int no_of_samples;

sem_t data_avail; // Do not change the name of this semaphore


// This thread function is for control
void *Control(void *arg)
{
	
	
	double ek;
	double uk;
	float motor_position;

	int k = 0;

	no_of_samples = (int)(run_time*Fs);
	
	while (k < no_of_samples) {
		sem_wait(&data_avail);
		motor_position = EtoR(ReadEncoder());

		// calculate tracking error
	   ek = ref[k] - motor_position;

		// calculate control value
		uk = Kp*ek;

		DtoA(VtoD(uk));
		theta[k] = motor_position;
		theta_deg[k] = motor_position/pi*180;

		k++;
	}
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



	no_of_samples = (int)(run_time*Fs);

	// Initialize the reference input vector ref[k]. ref is in radians.
	int i = 0;
	while (i < MAXS) {

		ref[i] = 50*pi/180.0;
		ref_deg[i] = 50;
		i++;
	}




           /* https://cboard.cprogramming.com/c-programming/143734-compiler-keeps-skipping-over-scanf-statement-please-help.html */
	   /* Adak said "scanf() has a problem with %c. Since you are expecting just a char, and the newline left behind by scanf() always, 		   is a char, it will pull off one newline char '\n', from the keyboard buffer, and seem to "skip" the input completely.

	   In reality, it didn't skip anything. It took the newline, thought that was what you wanted, and continued.

	   Add a getchar() after each scanf(), to pull the newlines off the keyboard buffer."  */

	while (1) {

		
		printf("Choose your selection \n");
		scanf("%c", &selection);  //Get the input from the user


		switch (selection) {


		case 'r':

			sem_init(&data_avail, 0, 0);
			Initialize(DLAB_SIMULATE, Fs, motor_number);

			if (pthread_create(&control_1, NULL, &Control, NULL) != 0) {
				 printf("Error creating control thread.\n");
				 exit(-1);
			}

 		   pthread_join(control_1, NULL);

			Terminate();
			sem_destroy(&data_avail);
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
			scanf(" %lf", &Kp);  //Get the input from the user
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
			plot(theta_deg, ref_deg, Fs, no_of_samples, PS, "Response of Motor", "Time (seconds)", "Load Shaft position (degrees)");
			getchar();  // put getchar() here to remove the newline character generated from code above

			break;


		case 'h':

			// Save the plot results in Postscript;
			// Save the graph of v1 and v2 vs time in Postscript
			plot(theta_deg, ref_deg, Fs, no_of_samples, PS, "Response of Motor", "Time (seconds)", "Load Shaft position (degrees)");
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




