/**
 * @file day03.c
 * @author G.J.J. van den Burg
 * @date 2020-12-03
 * @brief Advent of Code, Day 03

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

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>

#define CLEAR 0
#define TREE 1
#define BUFSIZE 1024

struct Field {
	int width;
	int height;
	int *map;
};

void map_set(struct Field *f, int r, int c, int v)
{
	// row-major order
	f->map[r*f->width + c] = v;
}

int map_get(struct Field *f, int r, int c) {
	return f->map[r*f->width + (c % f->width)];
}

struct Field *read_file(char *filename)
{
	char buf[BUFSIZE];
	FILE *fp = NULL;
	int i, j,
	    height = 0,
	    width = 0;

	fp = fopen(filename, "r");
	while (fgets(buf, BUFSIZE, fp) != NULL)
		height++;
	width = strlen(buf) - 1; // subtract newline

	fseek(fp, 0, SEEK_SET);

	struct Field *F = malloc(sizeof(struct Field));
	F->width = width;
	F->height = height;
	F->map = malloc(sizeof(int) * (width * height));

	for (i=0; i<height; i++) {
		fgets(buf, BUFSIZE, fp);
		for (j=0; j<width; j++) {
			assert(buf[j] == '.' || buf[j] == '#');
			if (buf[j] == '.')
				map_set(F, i, j, CLEAR);
			else
				map_set(F, i, j, TREE);
		}
	}

	fclose(fp);

	return F;
}

void print_field(struct Field *f)
{
	int i, j;
	for (i=0; i<f->height; i++) {
		for (j=0; j<f->width; j++)
			printf((map_get(f, i, j) == TREE) ? "#" : ".");
		printf("\n");
	}
}

int tree_count(struct Field *F, int right, int down)
{
	int r = 0,
	    c = 0,
	    trees = 0;
	while (r < F->height) {
		r += down;
		c += right;
		trees += map_get(F, r, c) == TREE;
	}
	return trees;
}

int solve_part_one(struct Field *F) {
	return tree_count(F, 3, 1);
}

long solve_part_two(struct Field *F) {
	long prod = 1;
	prod *= tree_count(F, 1, 1);
	prod *= tree_count(F, 3, 1);
	prod *= tree_count(F, 5, 1);
	prod *= tree_count(F, 7, 1);
	prod *= tree_count(F, 1, 2);
	return prod;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("Usage: %s input_file\n", argv[0]);
		return EXIT_FAILURE;
	}

	long ans = -1;

	struct Field *F = read_file(argv[1]);

	ans = solve_part_one(F);
	printf("Solution part 1: %ld\n", ans);

	ans = solve_part_two(F);
	printf("Solution part 2: %ld\n", ans);

	free(F->map);
	free(F);

	return EXIT_SUCCESS;
}
