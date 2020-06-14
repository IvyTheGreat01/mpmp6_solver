This is a C program that brute forces the solution to Matt Parker's Math Puzzle 6 

Puzzle: https://youtu.be/ILrqPpLpwpE

Solution: https://youtu.be/qGlDSOu4X8Y

Note that this program gets the solution that is worth 100 bonus points but does so very inefficiently, and does not use the "correct" Fibonnaci sequence method. 

To run, compile it with the Makefile and run balance3, because it is by far the most optimized and fastest.
balance3 is also the only version that supports multi threading.


There are several flags you can input to the program:

-g : (goal) A positive value for the goal (final bank balance) the program is searching for.

-i : (input bounds) A positive value that is multiplied by the goal to get the upper bound of tested inputs 
     (negative of that is lower bound) for first and second deposit.
     
-t : (thread count) The number of threads to use, should be aprox. the number of logical processors of your cpu, for best performance.

-p : (progress interval) The number of seconds between progress updates of the program, 
     the updates include: inputs processed out of total, time elapsed, time remaining
     (the time remaining is usually really accurate near the beginning so its a good indication of how long the total runtime will be).
     
Example Usage:

./balance3 -g 1000 -i 10 -t 12 -p 30


This means the program will search for the best solution for the final bank balance of 1 thousand.
The inputs for day 1 and day 2 will be a combination of all the inputs between neg 10 thousand and pos 10 thousand.
The program will do the calculation using 12 threads.
The program will output progress updates every 30 seconds.


Running just ./balance3 without any arguments is equivalent to running ./balance3 -g 1000000 -i 3 -t 12 -p 30


The program only tries the bank computation on inputs where the day 1 deposit and day 2 deposits have different signs.
This is because if they are both negative they will never reach a positive goal,
and if they are both positive they will reach the goal too quickly.
That's why during the progress updates the total number to be processed is half of (upperbound - lowerbound) squared.


Also all the big values in the program use the C data type (long long int) which has min and max values of:
−9,223,372,036,854,775,807, +9,223,372,036,854,775,807
So (upperbound  - lowerbound) squared must be smaller than the aprox. +9 quintillion upper bound of (long long int) sorry :(
