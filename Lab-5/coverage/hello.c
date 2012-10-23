/* dumping gcov data at runtime - note: no error handling to keep it simple */
/* Compile: gcc -Wall -fprofile-arcs -ftest-coverage hello.c -o hello
 * Run: ./hello &
 *
 kill -s SIGUSR1 ‘pidof hello‘
 *
 * Author: Der Herr Hofr.at
 * Copyright: OpenTech EDV Research GmbH 2011
 * License: GPL V2
 */
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

static unsigned long long i = 0;
void __gcov_flush(void); /* check in gcc sources gcc/gcov-io.h for the prototype */

void my_handler(int signum)
{
	printf("received signal\n");
	printf("%llu\n", i);
	__gcov_flush(); /* dump coverage data on receiving SIGUSR1 */
}

int main(int argc, char **argv)
{
	struct sigaction new_action, old_action;
	int n;
	/* setup signal hander */
	new_action.sa_handler = my_handler;
	sigemptyset(&new_action.sa_mask);
	new_action.sa_flags = 0;
	sigaction(SIGUSR1, NULL, &old_action);
	if (old_action.sa_handler != SIG_IGN)
		sigaction (SIGUSR1, &new_action, NULL);
	/* infinite loop - to exemplify dumping coverage data while program runs */
	for (n = 0; ; n++)
		i++;
}
