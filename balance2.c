// Ivan and Mom :)
// Program to get (hopefully) correct solution to Matt Parker's Math Puzzle #6
// version 2 runs approximately twice as fast as version 1, but doesn't support multithreading yet


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


// Struct that stores all the key global environment information of the program
typedef struct Environment {
	long long goal;
	long long bounds;
	int threads;
	int progress;
} Environment;
Environment E;

// Struct that stores the key information about the currently believed to be, best solution
typedef struct Best_solution {
	int day1_input;
	int day2_input;
	int steps;
} Best_solution;


// Sets the Environment to the default values 
// Can be changed by input parameters to program if user desires
void set_default_environment() {
	E.goal = 1000000;
	E.bounds = 3;
	E.threads = 1;
	E.progress = 30;
}


// Displays the evironment (for debugging)
void print_environment() {
	fprintf(stderr, "Goal: %lli, Bounds multiplier: %lli, Num threads: %d, Progress interval: %ds\n\n", E.goal, E.bounds, E.threads, E.progress);
}


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
		
		/* This version doesn't support multithreading	
		} else if (strcmp(argv[i], "-t") == 0) {
			// Does not allow 0 or less to be entered as the number of threads
			if ((val = strtol(argv[i+1], NULL, 10)) > 0) {
				E.threads = val;
			} else {
				fprintf(stderr, "Invalid argument entered for -i flag\n");
				exit(1);
			}
		*/

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


int main(int argc, char **argv) {
	// Set the envioronment variables
	set_input_environment(argc, argv);
	print_environment(); // Just for testing purposes

	// Calculate the actual input bounds values
	long long input_min = -E.goal * E.bounds;
	long long input_max = E.goal * E.bounds;
	
	// Counts the number of unique best solutions given the input bounds
	// should hopefully be 1 for true correct solution
	int num_best_solutions = 0;

	Best_solution best = { 0, 0, 0 };

	// Get teh start time of processing
	time_t start_time = time(NULL);
	
	// Set up the stuff to have correct progress information every E.progress seconds
	time_t prev_checkpoint_time = start_time;
	long long billion = 1000000000; // 1 billion
	long long num_total_processed;
	long long total_time_elapsed_min;

	long long num_total = (input_max - input_min) * (input_max - input_min);

	// Loop over all the possible inputs to the program
	for (int deposit_day1 = input_min; deposit_day1 <= input_max; ++deposit_day1) {
		for (int deposit_day2 = input_min; deposit_day2 <= input_max; ++deposit_day2) {
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
				if (best.steps < num_steps) {
					num_best_solutions = 1;
					best.day1_input = deposit_day1;
					best.day2_input = deposit_day2;
					best.steps = num_steps;	
				
				// If found just as good solution as stored best, increment number of best solutions
				} else if (best.steps == num_steps) {
					num_best_solutions++;
				}
			}

			// Because of the two if statements at the top, we skip half of all the inputs 
			// which isn't counted towards total processed, to adjust for this divide the 
			// total number to be processed by two when displaying to user
			num_total_processed++;
		}

		time_t curr_time = time(NULL);
		
		// Check time progress in outer loop because inner loop is really fast
		// so theres no point in doing too many of these checks for no reason as it slows it down
		if (curr_time - prev_checkpoint_time >= E.progress) {
			long long num_total_processed_in_billions = num_total_processed / billion;
			long long num_total_in_billions = num_total / billion / 2; // Divide by 2 because discard half inputs automatically
			total_time_elapsed_min = (curr_time - start_time) / 60;
			
			long long processed_per_second = (num_total_processed) / (curr_time - start_time);

			// Divide by 2 for the same reasons as above (this is by far the least accurate of all the metrics)
			long long total_time_remaining_min = (num_total - num_total_processed) / processed_per_second / 60 / 2;

			fprintf(stderr, "Total processed (Billions): %lli / %lli, Time elapsed: %llim, Time remaining: %llim\n", num_total_processed_in_billions, num_total_in_billions, total_time_elapsed_min, total_time_remaining_min);

			prev_checkpoint_time = curr_time;
		}
	}

	// Output the final results
	fprintf(stderr, "\n\n");
	printf("Input goal: %lli, Lower input bound: %lli, Upper input bound: %lli\n", E.goal, input_min, input_max);
	printf("Total time elapsed: %llim\n", total_time_elapsed_min);
	printf("Num unique solutions with most steps (hopefully 1): %d\n", num_best_solutions);

	printf("\nBest Solution (most steps):\n");
	// Add 3 to best.steps, to count the orignal 3 steps that happen in setup before the loop starts in calculate() and display_best_solution()
	printf("Day 1: %d, Day 2: %d, steps: %d\n\n", best.day1_input, best.day2_input, best.steps + 3);

	display_best_solution(best);

	return 0;
}
