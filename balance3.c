// Ivan and Mom :)
// Program (hopefully) calculates correct solution to Matt Parker's Math Puzzle #6
// Version 3 is basically the same as version 2 except it supports multithreading
// This makes it much much faster :)


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#define BIL 1000000000 // One billion


// Struct that stores all the key global environment information of the program
typedef struct Environment {
	long long goal;
	long long bounds;
	int threads;
	int progress;
} Environment;
Environment E;


// Struct that stores the key information about the current best solution
typedef struct Best_solution {
	int day1_input;
	int day2_input;
	int steps;
} Best_solution;


// Displays the evironment (for debugging)
void print_environment() {
	fprintf(stderr, "Goal: %lli, Bounds multiplier: %lli, Num threads: %d, Progress interval: %ds\n\n", 
			E.goal, E.bounds, E.threads, E.progress);
}


// Sets the Environment to the default values 
// Can be changed by input parameters to program if user desires
void set_default_environment() {
	E.goal = 1000000;
	E.bounds = 3;
	E.threads = 1;
	E.progress = 30;
}


typedef struct Thread_info {
	long long outer_lower_bound;
	long long outer_upper_bound;
	long long inner_lower_bound;
	long long inner_upper_bound;

	Best_solution best;
	long long num_processed;
	int num_best_solutions;
} Thread_info;


// Sets the key environment veriables that the program will use based on user input flags
void set_input_environment(int argc, char **argv) {
	// Set the default environment first
	set_default_environment();

	// Loop over all the input arguments (skipping the first one which is program name)
	for (int i = 1; i < argc - 1; ++i) {
		long val;
		if (strcmp(argv[i], "-g") == 0) {
			// Does not allow 0 or less to be entered as the goal
			if ((val = strtol(argv[i+1], NULL, 10)) > 0) {
				E.goal = val;
			} else {
				fprintf(stderr, "Invalid argument entered for -g flag\n");
				exit(1);
			}
		
		} else if (strcmp(argv[i], "-i") == 0) {
			// Does not allow 0 or less to be entered as the input bounds
			if ((val = strtol(argv[i+1], NULL, 10)) > 0) {
				E.bounds = val;
			} else {
				fprintf(stderr, "Invalid argument entered for -i flag\n");
				exit(1);
			}
		
		} else if (strcmp(argv[i], "-t") == 0) {
			// Does not allow 0 or less to be entered as the number of threads
			if ((val = strtol(argv[i+1], NULL, 10)) > 0) {
				E.threads = val;
			} else {
				fprintf(stderr, "Invalid argument entered for -i flag\n");
				exit(1);
			}

		} else if (strcmp(argv[i], "-p") == 0) {
			// Does not allow 0 or less to be entered as the progress interval
			if ((val = strtol(argv[i+1], NULL, 10)) > 0) {
				E.progress = val;
			} else {
				fprintf(stderr, "Invalid argument entered for -p flag\n");
				exit(1);
			}
		}
	}
}


// Prints out each step of most correct solution based on the inputs 
// displays whats stored in best
// Note that this is very similar to calculate() but for optimization
// purpouses during the main part of this program its a different function
// I don't really know if it matter tbh though
void display_best_solution(Best_solution best) {
	int very_old_bal = best.day1_input;
	int old_bal = best.day1_input + best.day2_input;
	int curr_bal = old_bal + very_old_bal;
	
	printf("Day: 1, Deposit: %d, Balance: %d\n", best.day1_input, very_old_bal);
	printf("Day: 2, Deposit: %d, Balance: %d\n", best.day2_input, old_bal);
	printf("Day: 3, Deposit: %d, Balance: %d\n", curr_bal - old_bal, curr_bal);

	for (int i = 0; i < best.steps; ++i) {
		very_old_bal = old_bal;
		old_bal = curr_bal;
		curr_bal = old_bal + very_old_bal;

		printf("Day: %d, Deposit: %d, Balance: %d\n", i+4, curr_bal - old_bal, curr_bal);
	}
}


// Does the actual bank calculation
// Returns 1 if reached the goal, 0 otherwise
// Sets steps value to the number of steps before termination
int calculate(int deposit_day1, int deposit_day2, int *steps) {
	int very_old_bal = deposit_day1;
	int old_bal = deposit_day1 + deposit_day2;
	int curr_bal = old_bal + very_old_bal;

	// Since input goal must be positive,
	// as soon as both previous balances are greater than the goal
	// it is impossible to reach the goal using the bank calculation
	while (curr_bal < E.goal) {
		very_old_bal = old_bal;
		old_bal = curr_bal;
		curr_bal = old_bal + very_old_bal;
		*steps = *steps + 1;
		
		// Same reasoning as in main()
		// as these will be the inputs in the next iteration
		if (old_bal <= 0 && curr_bal <= 0) {
			break;
		}

		// This is just to make sure we don't get an infinite loop (shouldn't happen lol)
		if (*steps >= 100000) {
			fprintf(stderr, "calculate while() loop ran too long\n");
			exit(1);
		}
	}

	return (curr_bal == E.goal);
}


// All the calculating threads start executing here
void *start_calculating(void *val) {
	// Gets this thread's bounds from the input arguments to it
	Thread_info *thread_info = val;
	
	// All bounds are inclusive
	long long outer_lower_bound = thread_info->outer_lower_bound;
	long long outer_upper_bound = thread_info->outer_upper_bound;
	long long inner_lower_bound = thread_info->inner_lower_bound;
	long long inner_upper_bound = thread_info->inner_upper_bound;

	// Loop over the input space doing the bank calculation
	for (int deposit_day1 = outer_lower_bound; deposit_day1 <= outer_upper_bound; ++deposit_day1) {
		for (int deposit_day2 = inner_lower_bound; deposit_day2 <= inner_upper_bound; ++deposit_day2) {
			// Skip all cases where both inputs are non positive
			// because they will never get to some positive goal
			if (deposit_day1 <= 0 && deposit_day2 <= 0) { 
				deposit_day2 = 1;
			}

			// Skip all cases where both inputs are positive
			// because they will get to a positive goal too fast
			else if (deposit_day1 > 0 && deposit_day2 > 0) { break; }

			// Run the bank calculation on these inputs and see if they reach the goal
			int num_steps = 0;

			if (calculate(deposit_day1, deposit_day2, &num_steps)) {
				// Update best stored solution if found a better solution
				if (thread_info->best.steps < num_steps) {
					thread_info->num_best_solutions = 1;
					thread_info->best.day1_input = deposit_day1;
					thread_info->best.day2_input = deposit_day2;
					thread_info->best.steps = num_steps;	
				
				// If found just as good solution as stored best, increment number of best solutions
				} else if (thread_info->best.steps == num_steps) {
					thread_info->num_best_solutions++;
				}
			}

			// Because of the two if statements at the top, we skip half of all the inputs 
			// which isn't counted towards total processed, to adjust for this divide the 
			// total number to be processed by two when displaying to user
			thread_info->num_processed++;
		}
	}


	return 0;
}

typedef struct Progress_stuff {
	time_t start_time;
	
	long long num_total;
	long long *num_processed[256];
} Progress_stuff;

int prog_die;

// The progress thread will start here
// It will periodically output to screen the progress of the program
// At termination it will return the total elapsed time of the program
void *start_progress_display(void *value) {
	Progress_stuff *val = value;

	time_t prev_checkpoint_time = val->start_time;

	// Runs while the main thread doesn't tell it to die
	while (!prog_die) {
		time_t curr_time = time(NULL); 

		// Enough time has passed since last update to 
		// do a new one
		if (curr_time - prev_checkpoint_time >= E.progress) {
			long long num_total_processed = 0;

			// Calculate the total number processed
			for (int i = 0; i < E.threads; ++i) {
				num_total_processed += *(val->num_processed[i]);
			}

			long long num_total_processed_bil = num_total_processed / BIL;

			// Divide by 2 because auto discard half inputs
			long long num_total_bil = val->num_total / BIL / 2;
			long long total_time_elapsed_min = (curr_time - val->start_time) / 60;

			long long proc_per_sec = (num_total_processed) / (curr_time - val->start_time);

			//Divide by 2 for same reason as above (this is least accurate of all metrics)
			long long total_time_rem_min = (val->num_total - num_total_processed) / proc_per_sec / 60 / 2;

			fprintf(stderr, "Total Processed (BIL): %lli / %lli, Time Elapsed: %llim, Time Remaining: %llim\n",
					num_total_processed_bil, num_total_bil, total_time_elapsed_min, total_time_rem_min);

			prev_checkpoint_time = curr_time;
		}

		// Call a sleep here because this only needs to run every few seconds for optimization
		sleep(E.progress);
	}

	return 0;
}



int main(int argc, char **argv) {
	// Set the environment variables
	set_input_environment(argc, argv);
	print_environment();

	// Calculate the actual input bounds values
	long long input_min = -E.goal * E.bounds;
	long long input_max = E.goal * E.bounds + 1;

	// Counts the number of unique best solutions given the input bounds
	// should hopefullly be 1 for true correct solution

	// Create the thread group
	pthread_t *thread_group = malloc(sizeof(pthread_t) * E.threads);

	// Create array of structs that will be passed to the threads with bounds info
	Thread_info *thread_info = malloc(sizeof(Thread_info) * E.threads);

	// Set the lower and upper bounds for each thread
	long long thread_range = (input_max - input_min) / E.threads;

	// Create the progress struct
	Progress_stuff *prog_stuff = malloc(sizeof(Progress_stuff));
	time_t start_time = time(NULL);

	prog_stuff->start_time = start_time;
	prog_stuff->num_total = (input_max - input_min) * (input_max - input_min);
	
	// Initialize and start all the calculating threads
	for (int i = 0; i < E.threads; ++i) {	
		// These bounds are all inclusive
		thread_info[i].outer_lower_bound = input_min + thread_range * i;
		thread_info[i].outer_upper_bound = input_min + thread_range * (i+1) - 1;
		thread_info[i].inner_lower_bound = input_min;
		thread_info[i].inner_upper_bound = input_max;

		thread_info[i].best.day1_input = 0;
		thread_info[i].best.day2_input = 0;
		thread_info[i].best.steps = 0;

		thread_info[i].num_processed = 0;
		thread_info[i].num_best_solutions = 0;

		pthread_create(&thread_group[i], NULL, start_calculating, &thread_info[i]);

		prog_stuff->num_processed[i] = &thread_info[i].num_processed;
	}

	// Create a progress thread which gives progess info to the user based on the other threads
	prog_die = 0;

	pthread_t *prog_thread = malloc(sizeof(pthread_t));
	pthread_create(prog_thread, NULL, start_progress_display, prog_stuff);

		
	// These vars hold the info we use to output correct data at the end
	int best_index = 0;
	int num_best_solutions = 0;

	// Wait for all the calculating threads to terminate and get their results
	for (int i = 0; i < E.threads; ++i) {
		pthread_join(thread_group[i], NULL);

		// Check if this thread has the best solution so far
		if (thread_info[best_index].best.steps < thread_info[i].best.steps) {
			best_index = i;
			num_best_solutions = thread_info[i].num_best_solutions;
		
		// Check if this thread has an equally good solution
		// This will correctly always be true on the first iteration
		} else if (thread_info[best_index].best.steps == thread_info[i].best.steps) {
			num_best_solutions += thread_info[i].num_best_solutions;
		}
	}

	// After all calculating threads are done we can terminate the progress thread
	prog_die = 1;

	pthread_join(*prog_thread, NULL);

	// Output the final results
	fprintf(stderr, "\n\n");
       	printf("Input goal: %lli, Lower input bound: %lli, Upper input bound: %lli\n", E.goal, input_min, input_max);
	printf("Total time elapsed: %lim\n", (time(NULL) - start_time) / 60);
	printf("Num unique solutions with most steps (hopefully 1): %d\n", num_best_solutions);

	printf("\nBest Solution (most steps):\n");

	// Add 3 to steps to account for starting loops on 4th day
	printf("Day 1: %d, Day2: %d, Steps: %d\n\n", thread_info[best_index].best.day1_input, 
			thread_info[best_index].best.day2_input, thread_info[best_index].best.steps + 3);

	display_best_solution(thread_info[best_index].best);

	return 0;
}
