/*
 * ch2:rlimit_primes.c
 * 
 ***************************************************************
 * This program is part of the source code released for the book
 *  "Hands-on System Programming with Linux"
 *  (c) Author: Kaiwan N Billimoria
 *  Publisher:  Packt
 *
 * From:
 *  Ch 2 : Virtual Memory
 ****************************************************************
 * Brief Description:
 *
 * Demo the querying and setting of the process resource limits,
 * using the prlimit(2) system call.
 * We use (a very simplistic, unoptimized) function to generate primes;
 * we then use prlimit(2) to cut down available CPU time to the process.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "../common.h"

/*---------------- Globals, Macros ----------------------------*/
#define MAX    90100

/*---------------- Typedef's, constants, etc ------------------*/

/*---------------- Functions ----------------------------------*/
static void simple_primegen(int limit)
{
	int i, j, num = 2, isprime;

	printf("  2,  3, ");
	for (i = 4; i <= limit; i++) {
		isprime = 1;
		for (j = 2; j < limit / 2; j++) {
			if ((i != j) && (i % j == 0)) {
				isprime = 0;
				break;
			}
		}
		if (isprime) {
			num++;
			printf("%6d, ", i);
			if (num % 16 == 0)
				printf("\n");
		}
	}
	printf("\n");
}

/* 
 * Setup the CPU resource limit to 'cpulimit' seconds
 */
static void setup_cpu_rlimit(int cpulimit)
{
	struct rlimit rlim_new, rlim_old;

	if (cpulimit == -1)
		rlim_new.rlim_cur = rlim_new.rlim_max = RLIM_INFINITY;
	else
		rlim_new.rlim_cur = rlim_new.rlim_max = cpulimit;

	if (prlimit(0, RLIMIT_CPU, &rlim_new, &rlim_old) == -1)
		FATAL("prlimit:cpu failed\n");
	printf
	    ("CPU rlimit [soft,hard] new: [%ld:%ld]s : old [%ld:%ld]s (-1 = unlimited)\n",
	     rlim_new.rlim_cur, rlim_new.rlim_max, rlim_old.rlim_cur,
	     rlim_old.rlim_max);
}

int main(int argc, char **argv)
{
	int limit, nsec;

	if (argc < 3) {
		fprintf(stderr,
			"Usage: %s limit-to-generate-primes-upto CPU-time\n"
			" CPU-time: -1 = unlimited, 0 = 1s.\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	limit = atoi(argv[1]);
	if (limit <= 4 || limit > MAX) {
		fprintf(stderr,
			"%s: invalid value (%d); pl pass a value within "
			"the range [4 - %d].\n", argv[0], limit, MAX);
		exit(EXIT_FAILURE);
	}
	nsec = atoi(argv[2]);
	if (nsec == 0)
		nsec = 1;
	else if (nsec < -1)
		nsec = -1;

	setup_cpu_rlimit(nsec);
	simple_primegen(limit);

	exit(EXIT_SUCCESS);
}

/* vi: ts=8 */
