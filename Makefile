CFLAGS = -Wall -Wextra  -Werror --std=c99
CC = gcc

re-ls: main.c
	$(CC) $(CFLAGS) main.c -o re-ls
