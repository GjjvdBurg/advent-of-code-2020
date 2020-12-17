/**
 * @file map.h
 * @author G.J.J. van den Burg
 * @date 2020-12-15
 * @brief Header file for map.c

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

#ifndef _MAP_H_
#define _MAP_H_

#include <stdbool.h>

struct Cube {
	bool active;
	int x;
	int y;
	int z;
	int w;
};

struct Item {
	struct Cube *key;
	int value;
	struct Item *next;
};

struct Map {
	int capacity;
	int n;
	struct Item **items;
};

struct Cube *init_cube(void);
struct Cube *copy_cube(struct Cube *c);
struct Cube *copy_key(struct Cube *k);

bool key_equal(struct Cube *a, struct Cube *b);
void free_key(struct Cube *k);

struct Item *init_item(void);
struct Item *create_item(struct Cube *key, int value);
void free_item(struct Item *it);

bool map_has(struct Map *m, struct Cube *key);
void map_set(struct Map *m, struct Cube *key, int value);
int map_get(struct Map *m, struct Cube *key);
struct Cube *map_get_key(struct Map *m, struct Cube *key);
struct Cube **map_list_keys(struct Map *m, int *n);
void map_delete(struct Map *m, struct Cube *key);

struct Map *init_map(void);
void free_map(struct Map *m);
void print_map(struct Map *m);

void *Realloc(void *ptr, size_t size);
void *Malloc(size_t size);

#endif
