/**
 * @file hash_map.c
 * @author G.J.J. van den Burg
 * @date 2020-12-15
 * @brief Map implementation for 3-int keys

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

#include "map.h"

// NOTE: A map owns all the memory of the items and keys it encapsulates.
//       It's almost as if you'd want to create a systems programming language 
//       that has strict rules for memory ownership, so you wouldn't need such 
//       notices. You could call it "patina", or something.

void *Realloc(void *ptr, size_t size)
{
	void *out = realloc(ptr, size);
	if (out == NULL) {
		fprintf(stderr, "Error allocating memory.\n");
		exit(EXIT_FAILURE);
	}
	return out;
}

void *Malloc(size_t size)
{
	void *out = malloc(size);
	if (out == NULL) {
		fprintf(stderr, "Error allocating memory.\n");
		exit(EXIT_FAILURE);
	}
	return out;
}

struct Cube *init_cube(void)
{
	struct Cube *c = Malloc(sizeof(struct Cube));
	c->x = c->y = c->z = c->w = 0;
	c->active = false;
	return c;
}

struct Cube *copy_cube(struct Cube *c)
{
	struct Cube *cp = Malloc(sizeof(struct Cube));
	cp->x = c->x;
	cp->y = c->y;
	cp->z = c->z;
	cp->w = c->w;
	cp->active = c->active;
	return cp;
}

struct Cube *copy_key(struct Cube *k)
{
	return copy_cube(k);
}

bool key_equal(struct Cube *a, struct Cube *b)
{
	// note: not comparing "active" field here on purpose!
	return a->x == b->x && a->y == b->y && a->z == b->z && a->w == b->w;
}

void free_key(struct Cube *k)
{
	free(k);
}

struct Item *init_item(void)
{
	struct Item *it = Malloc(sizeof(struct Item));
	it->key = init_cube();
	it->value = 0;
	it->next = NULL;
	return it;
}

struct Item *create_item(struct Cube *key, int value)
{
	struct Item *it = init_item();
	free_key(it->key);
	it->key = copy_key(key);
	it->value = value;
	return it;
}

void free_item(struct Item *it)
{
	if (it->next != NULL)
		free_item(it->next);
	free_key(it->key);
	free(it);
}

unsigned int hash_single(size_t key, int capacity)
{
	double sqrt5 = 2.23606797749978969641;
	double A = 0.5 * (sqrt5 - 1);
	double s = ((double) key) * A;
	double x = s - ((size_t) s);
	unsigned int hash = capacity * x;
	return hash;
}

size_t hash_function(struct Cube *key, int capacity)
{
	// borrowed from Python's tuple hash
	unsigned int t = 0x345678UL;
	int mult = 0xf4243;
	t = ((t ^ key->x) * mult);
	mult += (82520UL + 2 * (4 - 0));
	t = ((t ^ key->y) * mult);
	mult += (82520UL + 2 * (4 - 1));
	t = ((t ^ key->z) * mult);
	mult += (82520UL + 2 * (4 - 2));
	t = ((t ^ key->w) * mult);
	mult += (82520UL + 2 * (4 - 3));
	t += 97531UL;
	unsigned int h = hash_single(t, capacity);
	return h;
}

double occupancy(struct Map *m)
{
	return ((double) m->n) / ((double) m->capacity);
}

// returns whether a new top-level item was set
int set_item(struct Item **items, int index, struct Cube *key, int value)
{
	struct Item *parent = NULL,
		    *current = items[index];

	if (!current) {
		items[index] = create_item(key, value);
		return 1;
	}

	while (current) {
		if (key_equal(current->key, key)) {
			current->value = value;
			return 0;
		}
		parent = current;
		current = current->next;
	}
	parent->next = create_item(key, value);
	return 0;
}

void map_expand(struct Map *m)
{
	int i, index, new_capacity, new_n;
	struct Item *it = NULL,
		    *prev = NULL,
		    **new_items = NULL;

       	new_capacity = m->capacity == 0 ? 2 : m->capacity * 2;

	new_items = Malloc(sizeof(struct Item *) * new_capacity);
	for (i=0; i<new_capacity; i++) new_items[i] = NULL;

	// first use
	if (m->items == NULL) {
		m->capacity = new_capacity;
		m->items = new_items;
		return;
	}

	// map items to new indices
	new_n = 0;
	for (i=0; i<m->capacity; i++) {
		it = m->items[i];
		while (it != NULL) {
			index = hash_function(it->key, new_capacity);
			new_n += set_item(new_items, index, it->key, it->value);
			prev = it;
			it = it->next;
			free_key(prev->key);
			free(prev);
		}
	}
	free(m->items);

	m->capacity = new_capacity;
	m->items = new_items;
	m->n = new_n;
}

void map_set(struct Map *m, struct Cube *key, int value)
{
	if (m->capacity == 0 || occupancy(m) > 0.7) {
		map_expand(m);
	}
	int index = hash_function(key, m->capacity);
	m->n += set_item(m->items, index, key, value);
}

int map_get(struct Map *m, struct Cube *key)
{
	// TODO: currently assumes that key is present
	int index = hash_function(key, m->capacity);
	struct Item *it = m->items[index];
	while (!key_equal(it->key, key))
		it = it->next;
	return it->value;
}

// Get the pointer to the key in the map, for a given key.
// This is a bit odd, but seems useful? It's to do with pointing to the right 
// memory block.
struct Cube *map_get_key(struct Map *m, struct Cube *key)
{
	if (m->items == NULL)
		return NULL;
	int index = hash_function(key, m->capacity);
	struct Item *it = m->items[index];
	while (it != NULL) {
		if (key_equal(it->key, key))
			return it->key;
		it = it->next;
	}
	return NULL;
}

bool map_has(struct Map *m, struct Cube *key)
{
	if (m->items == NULL)
		return false;
	int index = hash_function(key, m->capacity);
	struct Item *it = m->items[index];
	while (it != NULL) {
		if (key_equal(it->key, key))
			return true;
		it = it->next;
	}
	return false;
}

void map_delete(struct Map *m, struct Cube *key)
{
	int index = hash_function(key, m->capacity);
	struct Item *it = m->items[index];
	struct Item *child = it->next;
	m->items[index] = child;
	it->next = NULL;
	free_item(it);
}

struct Cube **map_list_keys(struct Map *m, int *n) {
	struct Cube **keys = NULL;
	struct Item *it = NULL;
	int i, N = 0;
	for (i=0; i<m->capacity; i++) {
		it = m->items[i];
		while (it != NULL) {
			keys = Realloc(keys, sizeof(struct Cube *) * (++N));
			keys[N - 1] = copy_key(it->key);
			it = it->next;
		}
	}
	*n = N;
	return keys;
}

struct Map *init_map(void)
{
	struct Map *m = Malloc(sizeof(struct Map));
	m->capacity = 0;
	m->n = 0;
	m->items = NULL;
	return m;
}

void free_map(struct Map *m)
{
	int i;
	for (i=0; i<m->capacity; i++)
		if (m->items[i] != NULL)
			free_item(m->items[i]);
	free(m->items);
	free(m);
}

void print_map(struct Map *m)
{
	int i = 0;
	struct Item *it = NULL;
	printf("{\n");
	for (i=0; i<m->capacity; i++) {
		it = m->items[i];
		while (it != NULL) {
			printf("\t(%d,%d,%d,%d):\t%d\n", it->key->x, it->key->y,
					it->key->z, it->key->w, it->value);
			it = it->next;
		}
	}
	printf("}\n");
}
