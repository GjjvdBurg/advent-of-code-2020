/**
 * @file day10.c
 * @author G.J.J. van den Burg
 * @date 2020-12-10
 * @brief Advent of Code 2020, Day 10

 * Copyright (C) G.J.J. van den Burg

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <https://www.gnu.org/licenses/>.

 */

#include<stdio.h>
#include<stdlib.h>

#define BUFSIZE 1024

int *read_file(char *filename, int *N)
{
	FILE *fp = NULL;
	char buf[BUFSIZE];

	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "Error opening file %s for reading.\n", filename);
		exit(EXIT_FAILURE);
	}

	int n = 0;
	int num;
	int *jolts = NULL;

	while ((fgets(buf, BUFSIZE, fp)) != NULL) {
		num = atoi(buf);
		jolts = realloc(jolts, ++n * sizeof(int));
		if (jolts == NULL) {
			fprintf(stderr, "Error allocating memory.\n");
			exit(EXIT_FAILURE);
		}
		jolts[n-1] = num;
	}

	fclose(fp);

	*N = n;
	return jolts;
}

int cmp(const void *a, const void *b) {
	return (*(int *)a - *(int *)b);
}

int solution_part_one(int *jolts, int N)
{
	int n_one = 0,
	    n_three = 0;
	int i, jolt = 0;

	qsort(jolts, N, sizeof(int), cmp);

	for (i=0; i<N; i++) {
		if (jolts[i] - jolt == 1)
			n_one++;
		else if (jolts[i] - jolt == 3)
			n_three++;
		else {
			fprintf(stderr, "Unexpected difference: %d\n", jolts[i] - jolt);
		}
		jolt = jolts[i];
	}
	n_three++; // for builtin

	return n_one * n_three;
}

/** @brief Solve part 2 of the problem
 *
 * The way this works is that it walks backwards over the sorted array and for 
 * each position in the array we compute how we could have gotten there. At 
 * the source, we add the number of the current position. So, if we have the 
 * array
 *
 * 	7 10 11 12
 *
 * then we start the count at 12 with the number 1. We add that to both 10 and 
 * 11 because from those numbers we can get to 12. Then we move to 11, and we 
 * can only get there from 10, so we add 11's number to 10's number and 
 * continue on to 10.
 *
 */
long solution_part_two(int *jolts, int N)
{
	long out;
	int i, j, k;

	// assumes jolts is sorted
	int *copy = malloc(sizeof(long) * (N+1));
	for (i=0; i<N; i++)
		copy[i+1] = jolts[i];
	copy[0] = 0;

	long *counts = malloc(sizeof(long) * (N+1));
	for (i=0; i<N+1; i++) counts[i] = 0;

	counts[N] = 1;
	for (i=N; i>0; i--) {
		for (j=1; j<=3; j++) {
			if (i-j < 0)
				continue;
			for (k=1; k<=3; k++) {
				if (copy[i-j] == copy[i] - k)
					counts[i-j] += counts[i];
			}
		}
	}

	out = counts[0];
	free(counts);
	free(copy);

	return out;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s input_file\n", argv[1]);
		return EXIT_FAILURE;
	}

	long ans;
	int N;
	int *jolts = read_file(argv[1], &N);

	ans = solution_part_one(jolts, N);
	printf("Solution part 1: %ld\n", ans);

	ans = solution_part_two(jolts, N);
	printf("Solution part 2: %ld\n", ans);

	free(jolts);

	return EXIT_SUCCESS;
}
