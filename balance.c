// Ivan and Mom :)

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define AUTO_GOAL 1000000


int input_goal;


int calculate(int deposit_day1, int deposit_day2, int *steps) {
	int very_old_bal = deposit_day1;
	int old_bal = deposit_day1 + deposit_day2;
	int curr_bal = old_bal + very_old_bal;

	while (curr_bal < input_goal) {
		very_old_bal = old_bal;
		old_bal = curr_bal;
		curr_bal = old_bal + very_old_bal;
		*steps = *steps + 1;

		if (old_bal <= 0 && very_old_bal <= 0) {
			// fprintf(stderr, "day1: %d, day2: %d, num steps: %d (all zeros)\n", deposit_day1, deposit_day2, *steps);
			break;
		}		
		
		if (*steps >= 100000) {
			fprintf(stderr, "error while loop\n");
			exit(1);
			break;
		}	
	}

	return (curr_bal == input_goal);
}



void output (int deposit_day1, int deposit_day2, int steps) {
	fprintf(stderr, "day1: %d, day2: %d, num steps: %d\n", deposit_day1, deposit_day2, steps);
}


int main(int argc, char **argv) {

	long long input_max;
	long long input_min;
	
	// If only one argument thats the goal and the bounds are double it (pos and neg)
	if (argc == 2) {
		input_goal = strtol(argv[1], NULL, 10);
		input_max = input_goal * 2;
		input_min = -input_goal * 2;
	
	// If two arguments first is the goal and the bounds are the goal multiplied by the second parameter (pos and neg)
	} else if (argc == 3) {
		input_goal = strtol(argv[1], NULL, 10);
		input_max = input_goal * strtol(argv[2], NULL, 10);
		input_min = - input_goal * strtol(argv[2], NULL, 10);
	
	// If no argument he goal is one million and we double around it (pos and neg)
	} else if (argc == 1) {
		input_goal = AUTO_GOAL;
		input_max = input_goal * 2;
		input_min = -input_goal * 2;
	
	// Otherwise they called the process incorrectly
	} else {
		fprintf(stderr, "Invalid use of program. First argument is goal, second argument is bounds around goal.\n");
		fprintf(stderr, "If no second argument bounds are doubled around goal. If no first argumemnt goal is 1 million\n");
		exit(1);
	}

	// fprintf(stderr, "\nInput goal: %d, Upper bound: %lli, Lower bound %lli\n\n", input_goal, input_max, input_min);

	int most_day1 = 0;
	int most_day2 = 0;
	int most_steps = 0;
	
	int num_max = 0;
	
	long long checkpoint = 1000000000;
	long long checkpoint_counter = 0;
	long long num_checkpoints = 0;
	long long num_total = (input_max - input_min) * (input_max - input_min);
	long long num_total_checkpoints = (long long) (num_total / checkpoint);

	for (int deposit_day1 = input_min; deposit_day1 <= input_max; ++deposit_day1) {
		for (int deposit_day2 = input_min; deposit_day2 <= input_max; ++deposit_day2) {
			int num_steps = 0;

			if (calculate(deposit_day1, deposit_day2, &num_steps)) {
				//fprintf(stderr, "day1: %d, day2: %d, num_steps: %d\n", deposit_day1, deposit_day2, num_steps);
						
				if (most_steps < num_steps) {
					num_max = 1;
					most_steps = num_steps;
					most_day1 = deposit_day1;
					most_day2 = deposit_day2;
				
				} else if (most_steps == num_steps) {
					num_max++;
					most_steps = num_steps;
					most_day1 = deposit_day1;
					most_day2 = deposit_day2;

				}
			}
			
			checkpoint_counter++;

			if (checkpoint_counter == checkpoint) {
				clock_t end = clock();
				clock_t start;
				float seconds = (float) (end - start) / CLOCKS_PER_SEC; 
	
				fprintf(stderr, "time: %f\n", seconds);
				checkpoint_counter = 0;
				num_checkpoints++;
				fprintf(stderr, "Num Processed (in %llis): %lli / %lli\n", checkpoint, num_checkpoints, num_total_checkpoints);

				start = clock();
			}
		}
	}	
	
	printf("\nInput goal: %d, Upper input bound: %lli, Lower input bound %lli\n\n", input_goal, input_max, input_min);
	printf("Most Steps Solution:\n");
	printf("day 1: %d, day 2: %d, steps: %d\n\n", most_day1, most_day2, most_steps);
	printf("Num unique solutions with most steps: %d\n", num_max);

	return 0;
}
