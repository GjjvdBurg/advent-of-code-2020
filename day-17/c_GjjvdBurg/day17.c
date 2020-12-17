/**
 * @file aoc2020.c
 * @author G.J.J. van den Burg
 * @date 2020-12-12
 * @brief Advent of Code 2020, Day 17

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

struct CubeList {
	int n;
	struct Cube **cubes;
};

void free_cube(struct Cube *c)
{
	free(c);
}

struct CubeList *init_cubelist(void)
{
	struct CubeList *cl = Malloc(sizeof(struct CubeList));
	cl->n = 0;
	cl->cubes = NULL;
	return cl;
}

void print_cubelist(struct CubeList *cl)
{
	printf("CubeList:\n");
	for (int i=0; i<cl->n; i++) {
		printf("\t(%d,%d,%d,%d) -> %s\n", cl->cubes[i]->x,
			       	cl->cubes[i]->y, cl->cubes[i]->z,
				cl->cubes[i]->w,
				cl->cubes[i]->active ? "active" : "inactive");
	}
	printf("\n");
}

void free_cubelist(struct CubeList *cl)
{
	for (int i=0; i<cl->n; i++)
		free_cube(cl->cubes[i]);
	free(cl->cubes);
	free(cl);
}

struct CubeList *read_file(char *filename)
{
	FILE *fp = NULL;
	int x, y, z, w, n = 0;
	char buf[BUFSIZE];
	struct Cube *cube = NULL,
		    **cubes = NULL;
	struct CubeList *cube_list = NULL;

	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "Error opening file %s for reading.\n", filename);
		exit(EXIT_FAILURE);
	}

	cube_list = init_cubelist();

	y = z = w = 0;
	while ((fgets(buf, BUFSIZE, fp)) != NULL) {
		for (x=0; x<strlen(buf); x++) {
			if (buf[x] != '#')
				continue;
			cube = init_cube();
			cube->x = x;
			cube->y = y;
			cube->z = z;
			cube->w = w;
			cube->active = true;

			cubes = Realloc(cubes, sizeof(struct Cube *) * (++n));
			cubes[n-1] = cube;
		}
		y++;
	}

	fclose(fp);

	cube_list->n = n;
	cube_list->cubes = cubes;

	return cube_list;
}

// map coordinate to the number of active neighbors at that coordinate
struct Map *build_neighbor_map_v1(struct CubeList *cl)
{
	int i, x, y, z;
	struct Cube *cube = NULL;
	struct Cube *key = init_cube();
	struct Map *m = init_map();

	for (i=0; i<cl->n; i++) {
		cube = cl->cubes[i];
		for (x=-1; x<=1; x++) {
			for (y=-1; y<=1; y++) {
				for (z=-1; z<=1; z++) {
					if (x == 0 && y == 0 && z == 0)
						continue;

					key->x = cube->x + x;
					key->y = cube->y + y;
					key->z = cube->z + z;

					if (!map_has(m, key)) {
						map_set(m, key, 0);
					}
					map_set(m, key, map_get(m, key) + 1);
				}
			}
		}
	}
	free(key);

	return m;
}

// map coordinate to the number of active neighbors at that coordinate
struct Map *build_neighbor_map_v2(struct CubeList *cl)
{
	int i, x, y, z, w;
	struct Cube *cube = NULL;
	struct Cube *key = init_cube();
	struct Map *m = init_map();

	// who doesn't like a deep for loop?
	for (i=0; i<cl->n; i++) {
		cube = cl->cubes[i];
		for (x=-1; x<=1; x++) {
			for (y=-1; y<=1; y++) {
				for (z=-1; z<=1; z++) {
					for (w=-1; w<=1; w++) {
						if (x == 0 && y == 0 && z == 0
								&& w == 0)
							continue;

						key->x = cube->x + x;
						key->y = cube->y + y;
						key->z = cube->z + z;
						key->w = cube->w + w;

						if (!map_has(m, key)) {
							map_set(m, key, 0);
						}
						map_set(m, key, map_get(m, key) + 1);
					}
				}
			}
		}
	}
	free(key);

	return m;
}

struct CubeList *cycle(struct CubeList *cl,
	       	struct Map *map_builder(struct CubeList *))
{
	int i, nnb, n_keys, N = 0;
	struct Cube *cp = NULL,
		    *key = NULL;
	struct Cube *cube = NULL,
		    **new_cubes = NULL;
	struct CubeList *new_cl = NULL;

	// map coordinate to the number of active neighbors at that coordinate
	struct Map *m = map_builder(cl);

	// for the cubes that we have in our list, set the matching key in the 
	// map to the corresponding state. If it is not in the map, add it 
	// with n_neighbors = 0
	for (i=0; i<cl->n; i++) {
		cube = cl->cubes[i];
		key = map_get_key(m, cube);
		if (key == NULL)
			map_set(m, cube, 0);
		else
			key->active = cube->active;
	}

	// create a new list of cubes by applying the rules on each of the 
	// keys in the map
	struct Cube **keys = map_list_keys(m, &n_keys);
	for (i=0; i<n_keys; i++) {
		cube = keys[i];
		nnb = map_get(m, cube);
		if (cube->active && (nnb == 2 || nnb == 3)) {
			cp = copy_cube(cube);
		} else if (!cube->active && nnb == 3) {
			cp = copy_cube(cube);
			cp->active = true;
		} else {
			continue;
		}

		new_cubes = Realloc(new_cubes, sizeof(struct Cube *) * (++N));
		new_cubes[N - 1] = cp;
	}

	for (i=0; i<n_keys; i++)
		free_cube(keys[i]);
	free(keys);

	free_map(m);
	free_cubelist(cl);

	new_cl = init_cubelist();
	new_cl->n = N;
	new_cl->cubes = new_cubes;
	return new_cl;
}

struct CubeList *solution_part_one(struct CubeList *cl)
{
	int i;
	for (i=0; i<6; i++)
		cl = cycle(cl, build_neighbor_map_v1);
	return cl;
}

struct CubeList *solution_part_two(struct CubeList *cl)
{
	int i;
	for (i=0; i<6; i++)
		cl = cycle(cl, build_neighbor_map_v2);
	return cl;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s input_file\n", argv[0]);
		return EXIT_FAILURE;
	}

	struct CubeList *cl = NULL;

	cl = read_file(argv[1]);
	cl = solution_part_one(cl);
	printf("Solution part 1: %d\n", cl->n);
	free_cubelist(cl);

	cl = read_file(argv[1]);
	cl = solution_part_two(cl);
	printf("Solution part 2: %d\n", cl->n);
	free_cubelist(cl);

	return EXIT_SUCCESS;
}
