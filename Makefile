all: balance balance2 balance3

balance: balance.c
	$(CC) balance.c -o balance -Wall -Wextra -pedantic -std=c99

balance2: balance2.c
	$(CC) balance2.c -o balance2 -Wall -Wextra -pedantic -std=c99

balance3: balance3.c
	$(CC) balance3.c -o balance3 -Wall -Wextra -pedantic -std=c99 -lpthread
