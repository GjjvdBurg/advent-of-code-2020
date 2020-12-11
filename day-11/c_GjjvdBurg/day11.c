/**
 * @file day11.c
 * @author G.J.J. van den Burg
 * @date 2020-12-11
 * @brief Advent of Code 2020, Day 11

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

#define BUFSIZE 1024
#define FLOOR 0
#define EMPTY 1
#define TAKEN 2

struct WaitingArea {
	int width;
	int height;
	int *grid;
};

struct WaitingArea *wa_init(void)
{
	struct WaitingArea *wa = malloc(sizeof(struct WaitingArea));
	wa->width = 0;
	wa->height = 0;
	wa->grid = NULL;
	return wa;
}

void wa_free(struct WaitingArea *wa)
{
	free(wa->grid);
	free(wa);
	wa = NULL;
}

struct WaitingArea *wa_copy(struct WaitingArea *wa)
{
	int i;
	struct WaitingArea *cp = wa_init();
	cp->width = wa->width;
	cp->height = wa->height;
	cp->grid = malloc(sizeof(int) * (cp->width * cp->height));
	for (i=0; i<(cp->width * cp->height); i++)
		cp->grid[i] = wa->grid[i];
	return cp;
}

bool wa_equal(struct WaitingArea *a, struct WaitingArea *b)
{
	int i;
	if (a->width != b->width)
		return false;
	if (a->height != b->height)
		return false;
	for (i=0; i<a->width * a->height; i++)
		if (a->grid[i] != b->grid[i])
			return false;
	return true;
}

int wa_get(struct WaitingArea *wa, int i, int j)
{
	return wa->grid[i*wa->width + j];
}

void wa_set(struct WaitingArea *wa, int i, int j, int c)
{
	wa->grid[i*wa->width + j] = c;
}

void wa_print(struct WaitingArea *wa)
{
	int i, j;
	printf("\nWaiting Area:\n");
	for (i=0; i<wa->height; i++) {
		for (j=0; j<wa->width; j++) {
			if (wa_get(wa, i, j) == FLOOR)
				printf(".");
			else if (wa_get(wa, i, j) == EMPTY)
				printf("L");
			else if (wa_get(wa, i, j) == TAKEN)
				printf("#");
		}
		printf("\n");
	}
}


int wa_occupied_adjacent(struct WaitingArea *wa, int i, int j)
{
	int u, v, count = 0;
	for (u=i-1; u<i+2; u++) {
		if (u < 0 || u == wa->height)
			continue;
		for (v=j-1; v<j+2; v++) {
			if (v < 0 || v == wa->width)
				continue;
			if (u == i && v == j)
				continue;
			count += wa_get(wa, u, v) == TAKEN;
		}
	}
	return count;
}

int wa_occupied_visible(struct WaitingArea *wa, int i, int j)
{
	int u, v, step_i, step_j, count = 0;

	for (step_i=-1; step_i<=1; step_i++) {
		for (step_j=-1; step_j<=1; step_j++) {
			if (step_i == 0 && step_j == 0)
				continue;
			u = i;
			v = j;
			while (true) {
				u += step_i;
				v += step_j;
				if (u < 0 || u >= wa->height)
					break;
				if (v < 0 || v >= wa->width)
					break;
				if (wa_get(wa, u, v) == FLOOR)
					continue;
				if (wa_get(wa, u, v) == TAKEN)
					count += 1;
				break;
			}
		}
	}
	return count;
}

struct WaitingArea *read_file(char *filename)
{
	FILE *fp = NULL;
	char buf[BUFSIZE];

	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "Error opening file %s for reading.\n", filename);
		exit(EXIT_FAILURE);
	}

	int i, j;
	struct WaitingArea *wa = wa_init();

	while ((fgets(buf, BUFSIZE, fp)) != NULL)
		wa->height++;
	wa->width = strlen(buf) - 1;
	wa->grid = malloc(sizeof(int) * (wa->width * wa->height));

	fseek(fp, 0, SEEK_SET);

	for (i=0; i<wa->height; i++) {
		fgets(buf, BUFSIZE, fp);
		for (j=0; j<wa->width; j++) {
			if (buf[j] == '.')
				wa_set(wa, i, j, FLOOR);
			else if (buf[j] == 'L')
				wa_set(wa, i, j, EMPTY);
		}
	}

	fclose(fp);
	return wa;
}

int update_seat_one(struct WaitingArea *wa, int i, int j)
{
	int count = wa_occupied_adjacent(wa, i, j);
	int state = wa_get(wa, i, j);
	if (state == EMPTY && count == 0)
		return TAKEN;
	else if (state == TAKEN && count >= 4)
		return EMPTY;
	return state;
}

int solution_part_one(struct WaitingArea *waiting_area)
{
	int i, j, ans = 0;
	struct WaitingArea *wa = wa_copy(waiting_area);
	struct WaitingArea *cp = NULL;

	while (true) {
		cp = wa_copy(wa);
		for (i=0; i<wa->height; i++)
			for (j=0; j<wa->width; j++)
				wa_set(cp, i, j, update_seat_one(wa, i, j));

		if (wa_equal(cp, wa))
			break;

		wa_free(wa);
		wa = cp;
	}

	for (i=0; i<wa->height; i++)
		for (j=0; j<wa->width; j++)
			ans += wa_get(wa, i, j) == TAKEN;

	wa_free(wa);
	wa_free(cp);
	return ans;
}

int update_seat_two(struct WaitingArea *wa, int i, int j)
{
	int count = wa_occupied_visible(wa, i, j);
	int state = wa_get(wa, i, j);
	if (state == EMPTY && count == 0)
		return TAKEN;
	if (state == TAKEN && count >= 5)
		return EMPTY;
	return state;
}

int solution_part_two(struct WaitingArea *waiting_area)
{
	int i, j, ans = 0;
	struct WaitingArea *wa = wa_copy(waiting_area);
	struct WaitingArea *cp = NULL;

	while (true) {
		cp = wa_copy(wa);
		for (i=0; i<wa->height; i++)
			for (j=0; j<wa->width; j++)
				wa_set(cp, i, j, update_seat_two(wa, i, j));

		if (wa_equal(cp, wa))
			break;

		wa_free(wa);
		wa = cp;
	}

	for (i=0; i<wa->height; i++)
		for (j=0; j<wa->width; j++)
			ans += wa_get(wa, i, j) == TAKEN;

	wa_free(wa);
	wa_free(cp);
	return ans;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s input_file\n", argv[0]);
		return EXIT_FAILURE;
	}

	struct WaitingArea *wa = read_file(argv[1]);

	int ans = solution_part_one(wa);
	printf("Solution part 1: %d\n", ans);

	ans = solution_part_two(wa);
	printf("Solution part 2: %d\n", ans);

	wa_free(wa);

	return EXIT_SUCCESS;
}
