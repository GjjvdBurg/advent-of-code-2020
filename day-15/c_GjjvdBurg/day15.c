/**
 * @file aoc2020.c
 * @author G.J.J. van den Burg
 * @date 2020-12-12
 * @brief Advent of Code 2020, Day 15

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

#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "map.h"

#define BUFSIZE 1024

int *read_file(char *filename, int *N)
{
	FILE *fp = NULL;
	int num, n = 0;
	int *nums = NULL;
	char buf[BUFSIZE];

	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "Error opening file %s for reading.\n", filename);
		exit(EXIT_FAILURE);
	}

	if ((fgets(buf, BUFSIZE, fp)) == NULL) {
		fprintf(stderr, "File %s contains no data.\n", filename);
		exit(EXIT_FAILURE);
	}

	char *token = NULL,
	     *copy = malloc(sizeof(char) * (strlen(buf) + 1));
	copy = strcpy(copy, buf);
	char *ptr = copy;

	while ((token = strtok(copy, ",")) != NULL) {
		copy = NULL;
		num = atoi(token);
		nums = Realloc(nums, sizeof(int) * (++n));
		nums[n-1] = num;
	}

	fclose(fp);
	free(ptr);

	*N = n;
	return nums;
}

int memory_game(int *nums, int n, int turn_target)
{
	int i, age, last, turn_said, say, turn;

	// map number -> turn last spoken
	struct Map *m = init_map();

	for (i=0; i<n-1; i++)
		map_set(m, nums[i], i+1);

	last = nums[n-1];
	turn = n+1;
	while (turn <= turn_target) {
		// was the last spoken digit spoken before?
		if (map_has(m, last)) {
			turn_said = map_get(m, last);
			age = (turn - 1) - turn_said;
			say = age;
		} else {
			say = 0;
		}

		// push the last spoken number (from previous turn) to map
		map_set(m, last, turn-1);

		// "say" the number
		last = say;
		turn++;
	}

	free_map(m);
	return last;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s input_file\n", argv[0]);
		return EXIT_FAILURE;
	}

	int n, ans, *nums;
	nums = read_file(argv[1], &n);

	ans = memory_game(nums, n, 2020);
	printf("Solution part 1: %d\n", ans);

	ans = memory_game(nums, n, 30000000);
	printf("Solution part 2: %d\n", ans);

	free(nums);

	return EXIT_SUCCESS;
}
