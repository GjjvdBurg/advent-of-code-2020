/**
 * @file day09.c
 * @author G.J.J. van den Burg
 * @date 2020-12-09
 * @brief Advent of Code 2020, Day 09

 * Copyright (C) G.J.J. van den Burg

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

 */

#include<stdbool.h>
#include<stdlib.h>
#include<stdio.h>

#define BUFSIZE 1024

long *read_file(char *filename, int *N)
{
	FILE *fp = NULL;
	char buf[BUFSIZE];

	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "Error opening file %s for reading.\n", filename);
		exit(EXIT_FAILURE);
	}

	int n = 0;
	long num;
	long *tape = NULL;

	while ((fgets(buf, BUFSIZE, fp)) != NULL) {
		num = atol(buf);
		tape = realloc(tape, ++n * sizeof(long));
		if (tape == NULL) {
			fprintf(stderr, "Error allocating memory.\n");
			exit(EXIT_FAILURE);
		}
		tape[n-1] = num;
	}

	fclose(fp);

	*N = n;
	return tape;
}

bool has_subset_sum(long *tape, int N, long num)
{
	// n^2, lazy
	int i, j;
	for (i=0; i<N; i++) {
		for (j=i+1; j<N; j++) {
			if (tape[i] + tape[j] == num)
				return true;
		}
	}
	return false;
}

long solution_part_one(long *tape, int N, int n_preamble)
{
	long *pos = tape;
	int idx = n_preamble;

	while (idx < N) {
		if (!has_subset_sum(pos, n_preamble, tape[idx]))
			return tape[idx];
		idx++;
		pos++;
	}
	return -1;
}

long sum_range(long *tape, int i, int j)
{
	long sum = 0;
	for (int k=i; k<j; k++)
		sum += tape[k];
	return sum;
}

long minimum(long *arr, long N)
{
	long min = arr[0];
	for (int i=1; i<N; i++)
		min = (arr[i] < min) ? arr[i] : min;
	return min;
}

long maximum(long *arr, long N)
{
	long max = arr[0];
	for (int i=1; i<N; i++)
		max = (arr[i] > max) ? arr[i] : max;
	return max;
}

long solution_part_two(long *tape, int N, int n_preamble, long invalid)
{
	long min, max;
	for (int len=2; len<N; len++) {
		for (int i=0; i+len<N; i++) {
			if (sum_range(tape, i, i+len) == invalid) {
				min = minimum((tape + i), len);
				max = maximum((tape + i), len);
				return min + max;
			}
		}
	}
	return -1;
}

int main(int argc, char **argv)
{
	if (argc < 2 || argc > 3) {
		fprintf(stderr, "Usage: %s input_file [preamble_length]\n", argv[0]);
		return EXIT_FAILURE;
	}

	int pre_length = (argc == 2) ? 25 : atoi(argv[2]);
	int N;
	long *tape = read_file(argv[1], &N);

	long ans = solution_part_one(tape, N, pre_length);
	printf("Solution part 1: %ld\n", ans);

	ans = solution_part_two(tape, N, pre_length, ans);
	printf("Solution part 2: %ld\n", ans);

	free(tape);

	return EXIT_SUCCESS;
}
