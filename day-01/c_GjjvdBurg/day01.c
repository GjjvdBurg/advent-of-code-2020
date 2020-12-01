/**
 * @file day01.c
 * @author G.J.J. van den Burg
 * @date 2020-12-01
 * @brief Solution to day 01 of AOC 2020

 * Copyright (C) 2020, G.J.J. van den Burg

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

// compile with gcc -Wall -o day01 ./day01.c
// run with ./day01 input_day01.txt

#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>

#define YEAR 2020
#define BUFSIZE 1024

int *read_numbers(char *filename, int *N) {

	char buf[BUFSIZE];
	int i, n_lines = 0;
	int *nums = NULL;
	FILE *fp = NULL;

	// figure out how many lines we have
	fp = fopen(filename, "r");
	while (fgets(buf, BUFSIZE, fp) != NULL)
		n_lines++;

	fseek(fp, 0, SEEK_SET);

	// read file into array
	nums = malloc(n_lines * sizeof(int));
	for (i=0; i<n_lines; i++) {
		fgets(buf, BUFSIZE, fp);
		nums[i] = atoi(buf);
	}
	fclose(fp);

	*N = n_lines;

	return nums;
}

int cmp(const void *a, const void *b) {
	return (*(int *)a - *(int *)b);
}

int find_product_two(int *nums, int N)
{
	int i, ans, lidx, ridx;
	int *diffs = NULL;

	// sort the input array
	qsort(nums, N, sizeof(int), cmp);

	// create a difference array
	diffs = malloc(N * sizeof(int));
	for (i=0; i<N; i++)
		diffs[i] = YEAR - nums[i];

	lidx = 0;
	ridx = N - 1;

	// when we find the same number we're done
	while (true) {
		if (nums[lidx] == diffs[ridx])
			break;
		else if (nums[lidx] > diffs[ridx])
			ridx--;
		else if (nums[lidx] < diffs[ridx])
			lidx++;
		if (ridx < 0)
			return -1;
		if (lidx > N - 1)
			return -1;
	}

	ans = nums[lidx] * (YEAR - diffs[ridx]);
	free(diffs);
	return ans;
}

int find_product_three(int *nums, int N)
{
	bool breakout = false;
	int ans, lidx, ridx, midx;
	int L, R, M;

	// sort the array
	qsort(nums, N, sizeof(int), cmp);

	lidx = 0;
	ridx = N-1;

	while (lidx < ridx - 1) {
		L = nums[lidx];
		R = nums[ridx];

		// if we're always larger than the target, we need to reduce
		// the largest number
		if (L + nums[lidx+1] + R > YEAR) {
			ridx--;
			if (ridx <= lidx+1)
				return -1;
			continue;
		}

		// if we're always smaller than the target, we need to
		// increase the smallest number
		if (L + nums[ridx-1] + R < YEAR) {
			lidx++;
			if (lidx >= ridx-1)
				return -1;
			continue;
		}

		// look for the intermediate number that makes the sum
		for (midx=lidx+1; midx<ridx; midx++) {
			M = nums[midx];
			if (L + M + R == YEAR) {
				breakout = true;
				break;
			}
		}

		// if we have a solution, break out of the while loop
		if (breakout)
			break;

		// if we haven't found a solution, increase the smallest
		// number (why the smallest? Because if we decrease the
		// largest we may eliminate the solution).
		lidx++;
	}
	ans = L * M * R;
	return ans;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("Usage: %s input_file\n", argv[0]);
		return EXIT_FAILURE;
	}

	int N, ans;
	int *nums = read_numbers(argv[1], &N);

	ans = find_product_two(nums, N);
	if (ans == -1)
		printf("Part 1: No solution.\n");
	else
		printf("Part 1: Solution: %d\n", ans);

	ans = find_product_three(nums, N);
	if (ans == -1)
		printf("Part 2: No solution.\n");
	else
		printf("Part 2: Solution: %d\n", ans);

	free(nums);

	return EXIT_SUCCESS;
}
