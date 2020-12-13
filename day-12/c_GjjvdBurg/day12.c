/**
 * @file aoc2020.c
 * @author G.J.J. van den Burg
 * @date 2020-12-12
 * @brief Advent of Code 2020, Day XYZ

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

#include<math.h>
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define BUFSIZE 1024
#define PI 3.14159265358979323846

struct Ship {
	double angle;
	// interestingly, using ints for x and y leads to a difficult to find 
	// rounding error bugs.
	double x;
	double y;
};

struct Waypoint {
	double angle;
	double x;
	double y;
};

struct Waypoint *wap_init(void)
{
	struct Waypoint *w = malloc(sizeof(struct Waypoint));
	w->angle = 0;
	w->x = 10;
	w->y = 1;
	return w;
}

void wap_free(struct Waypoint *w)
{
	free(w);
	w = NULL;
}

struct Ship *ship_init(void)
{
	struct Ship *s = malloc(sizeof(struct Ship));
	s->angle = 0;
	s->x = s->y = 0;
	return s;
}

void ship_free(struct Ship *s)
{
	free(s);
	s = NULL;
}

void ship_move_one(struct Ship *s, char action, int units)
{
	if (action == 'N')
		s->y += units;
	else if (action == 'S')
		s->y -= units;
	else if (action == 'E')
		s->x += units;
	else if (action == 'W')
		s->x -= units;
	else if (action == 'L')
		s->angle += units;
	else if (action == 'R')
		s->angle -= units;
	else if (action == 'F') {
		s->x += units * cos(s->angle / 360 * 2 * PI);
		s->y += units * sin(s->angle / 360 * 2 * PI);
	} else {
		fprintf(stderr, "Unknown action: %c\n", action);
		exit(EXIT_FAILURE);
	}
}

struct Ship *read_file_one(char *filename)
{
	FILE *fp = NULL;
	int i, units, n = 0;
	char buf[BUFSIZE];
	char c;

	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "Error opening file %s for reading.\n", filename);
		exit(EXIT_FAILURE);
	}
	while (fgets(buf, BUFSIZE, fp) != NULL)
		n++;
	fseek(fp, 0, SEEK_SET);

	struct Ship *s = ship_init();
	for (i=0; i<n; i++) {
		fgets(buf, BUFSIZE, fp);

		c = buf[0];
		units = atoi(buf + 1);
		ship_move_one(s, c, units);
	}

	fclose(fp);
	return s;
}

void ship_move_two(struct Ship *s, struct Waypoint *w, char action, int units)
{
	double rad = ((double) units) / 360.0 * 2.0 * PI;
	double a, b;
	if (action == 'N')
		w->y += units;
	else if (action == 'S')
		w->y -= units;
	else if (action == 'E')
		w->x += units;
	else if (action == 'W')
		w->x -= units;
	else if (action == 'L') {
		a = w->x * cos(rad) - w->y * sin(rad);
		b = w->x * sin(rad) + w->y * cos(rad);
		w->x = a;
		w->y = b;
	} else if (action == 'R') {
		a = w->x * cos(-rad) - w->y * sin(-rad);
		b = w->x * sin(-rad) + w->y * cos(-rad);
		w->x = a;
		w->y = b;
	} else if (action == 'F') {
		s->x += units * w->x;
		s->y += units * w->y;
	} else {
		fprintf(stderr, "Unknown action: %c\n", action);
		exit(EXIT_FAILURE);
	}
}

struct Ship *read_file_two(char *filename)
{
	FILE *fp = NULL;
	int i, units, n = 0;
	char buf[BUFSIZE];
	char c;

	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "Error opening file %s for reading.\n", filename);
		exit(EXIT_FAILURE);
	}
	while (fgets(buf, BUFSIZE, fp) != NULL)
		n++;
	fseek(fp, 0, SEEK_SET);

	struct Ship *s = ship_init();
	struct Waypoint *w = wap_init();

	for (i=0; i<n; i++) {
		fgets(buf, BUFSIZE, fp);

		c = buf[0];
		units = atoi(buf + 1);
		ship_move_two(s, w, c, units);
	}
	fclose(fp);
	wap_free(w);
	return s;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s input_file\n", argv[0]);
		return EXIT_FAILURE;
	}
	int ans;
	struct Ship *s = NULL;

	s = read_file_one(argv[1]);
	ans = fabs(s->x) + fabs(s->y);
	printf("Solution part 1: %d\n", ans);
	ship_free(s);

	s = read_file_two(argv[1]);
	ans = fabs(s->x) + fabs(s->y);
	printf("Solution part 2: %d\n", ans);
	ship_free(s);

	return EXIT_SUCCESS;
}
