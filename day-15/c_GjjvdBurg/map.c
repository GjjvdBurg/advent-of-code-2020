/**
 * @file hash_map.c
 * @author G.J.J. van den Burg
 * @date 2020-12-15
 * @brief Quick Map implementation

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

struct Item *init_item(void)
{
	struct Item *it = Malloc(sizeof(struct Item));
	it->key = 0;
	it->value = 0;
	it->next = NULL;
	return it;
}

struct Item *create_item(int key, int value)
{
	struct Item *it = init_item();
	it->key = key;
	it->value = value;
	return it;
}

void free_item(struct Item *it)
{
	if (it->next != NULL)
		free_item(it->next);
	free(it);
}

int hash_function(int key, int capacity)
{
	double sqrt5 = 2.23606797749978969641;
	double A = 0.5 * (sqrt5 - 1);
	double s = key * A;
	double x = s - ((int) s);
	int hash = capacity * x;
	//printf("Hash for key %i is %i\n", key, hash);
	return hash;
}

double occupancy(struct Map *m)
{
	return ((double) m->n) / ((double) m->capacity);
}

// returns whether a new top-level item was set
int set_item(struct Item **items, int index, int key, int value)
{
	struct Item *parent = NULL,
		    *current = items[index];

	if (!current) {
		items[index] = create_item(key, value);
		return 1;
	}

	while (current) {
		if (current->key == key) {
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

	new_n = 0;

	// map items to new indices
	for (i=0; i<m->capacity; i++) {
		it = m->items[i];
		while (it != NULL) {
			index = hash_function(it->key, new_capacity);
			new_n += set_item(new_items, index, it->key, it->value);
			prev = it;
			it = it->next;
			free(prev);
		}
	}
	free(m->items);

	m->capacity = new_capacity;
	m->items = new_items;
	m->n = new_n;
}

void map_set(struct Map *m, int key, int value)
{
	if (m->capacity == 0 || occupancy(m) > 0.7) {
		map_expand(m);
	}
	int index = hash_function(key, m->capacity);
	m->n += set_item(m->items, index, key, value);
}

int map_get(struct Map *m, int key)
{
	// TODO: currently assumes that key is present
	int index = hash_function(key, m->capacity);
	struct Item *it = m->items[index];
	while (it->key != key)
		it = it->next;
	return it->value;
}

bool map_has(struct Map *m, int key)
{
	int index = hash_function(key, m->capacity);
	struct Item *it = m->items[index];
	while (it != NULL) {
		if (it->key == key)
			return true;
		it = it->next;
	}
	return false;
}

void map_delete(struct Map *m, int key)
{
	int index = hash_function(key, m->capacity);
	struct Item *it = m->items[index];
	struct Item *child = it->next;
	m->items[index] = child;
	it->next = NULL;
	free_item(it);
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
			printf("\t%d:\t%d\n", it->key, it->value);
			it = it->next;
		}
	}
	printf("}\n");
}
