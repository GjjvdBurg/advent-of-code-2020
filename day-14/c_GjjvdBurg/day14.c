/**
 * @file aoc2020.c
 * @author G.J.J. van den Burg
 * @date 2020-12-12
 * @brief Advent of Code 2020, Day 14

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

// this one is a bit messy.

#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define BUFSIZE 1024
#define MEMSIZE 36

struct Memory {
	int n;
	long *idx;
	long *val;
};

struct Memory *memory_init(void)
{
	struct Memory *m = malloc(sizeof(struct Memory));
	m->n = 0;
	m->idx = NULL;
	m->val = NULL;
	return m;
}

void memory_free(struct Memory *m)
{
	free(m->idx);
	free(m->val);
	free(m);
	m = NULL;
}

bool str_startswith(const char *str, const char *pre)
{
	size_t lenpre = strlen(pre),
	       lenstr = strlen(str);
	return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}

char *parse_mask(const char *buf)
{
	char *token = NULL,
	     *ptr = NULL,
	     *mask = NULL;

	char *copy = malloc(sizeof(char) * (strlen(buf) + 1));
	if (copy == NULL) {
		fprintf(stderr, "Error allocating memory.\n");
		exit(EXIT_FAILURE);
	}
	copy = strcpy(copy, buf);
	ptr = copy;

	token = strtok(copy, "=");
	token = strtok(NULL, "=");

	mask = malloc(sizeof(char) * strlen(token));
	if (mask == NULL) {
		fprintf(stderr, "Error allocating memory.\n");
		exit(EXIT_FAILURE);
	}
	strncpy(mask, token + 1, strlen(token) - 1);

	free(ptr);
	return mask;
}

void parse_mem(const char *buf, long *idx, long *val)
{
	int i;
	long index = 0,
	     value = 0;
	bool read_index = false,
	     read_value = false;

	for (i=0; i<strlen(buf)-1; i++) {
		if (buf[i] == '[') {
			read_index = true;
			continue;
		}
		if (buf[i] == ']') {
			read_index = false;
			continue;
		}
		if (buf[i] == ' ' && buf[i-1] == '=') {
			read_value = true;
			continue;
		}
		if (read_index) {
			index *= 10;
			index += buf[i] - '0';
		}
		if (read_value) {
			value *= 10;
			value += buf[i] - '0';
		}
	}

	*idx = index;
	*val = value;
}

int *to_bit_array(long val)
{
	int i;
	int *arr = malloc(sizeof(int) * MEMSIZE);
	if (arr == NULL) {
		fprintf(stderr, "Error allocating memory.\n");
		exit(EXIT_FAILURE);
	}
	for (i=0; i<MEMSIZE; i++) {
		arr[MEMSIZE - i - 1] = val & 1;
		val >>= 1;
	}
	return arr;
}

long from_bit_array(int *arr)
{
	long i, val = 0;
	for (i=0; i<MEMSIZE; i++) {
		val <<= 1;
		val |= arr[i];
	}
	return val;
}

long apply_mask_v1(long val, char *mask)
{
	size_t i;
	int *bits = to_bit_array(val);
	long new_val;
	for (i=0; i<MEMSIZE; i++) {
		if (mask[i] == 'X')
			continue;
		bits[i] = (mask[i] == '1') ? 1 : 0;
	}
	new_val = from_bit_array(bits);
	free(bits);
	return new_val;
}

char *apply_mask_v2(long val, char *mask)
{
	char *result = malloc(sizeof(char) * MEMSIZE);
	int i, *bits = to_bit_array(val);
	for (i=0; i<MEMSIZE; i++) {
		if (mask[i] == 'X')
			result[i] = 'X';
		else if (mask[i] == '1')
			result[i] = '1';
		else
			result[i] = bits[i] == 1 ? '1' : '0';
	}
	free(bits);
	return result;
}

void mem_set(struct Memory *m, long idx, long val)
{
	int i, pos = -1;
	for (i=0; i<m->n; i++) pos = (m->idx[i] == idx) ? i : pos;
	if (pos == -1) {
		m->n++;
		m->idx = realloc(m->idx, sizeof(long) * m->n);
		m->val = realloc(m->val, sizeof(long) * m->n);
		if (m->idx == NULL || m->val == NULL) {
			fprintf(stderr, "Error allocating memory.\n");
			exit(EXIT_FAILURE);
		}
		pos = m->n-1;
	}
	m->idx[pos] = idx;
	m->val[pos] = val;
}

void update_memory_v1(struct Memory *m, char *mask, long idx, long val)
{
	long new_value = apply_mask_v1(val, mask);
	mem_set(m, idx, new_value);
}

void set_float_mask(struct Memory *m, char *floatmask, long val)
{
	int i, *bits = NULL;
	long idx;
	bool no_X = true;
	char *cp = NULL;
	for (i=0; i<MEMSIZE; i++) no_X &= floatmask[i] != 'X';

	if (no_X) {
		bits = malloc(sizeof(int) * MEMSIZE);
		for (i=0; i<MEMSIZE; i++) bits[i] = floatmask[i] - '0';
		idx = from_bit_array(bits);
		free(bits);
		mem_set(m, idx, val);
		return;
	}

	cp = malloc(sizeof(char) * MEMSIZE);
	for (i=0; i<MEMSIZE; i++) cp[i] = floatmask[i];

	for (i=0; i<MEMSIZE; i++) if (floatmask[i] == 'X') break;
	cp[i] = '0';
	set_float_mask(m, cp, val);
	cp[i] = '1';
	set_float_mask(m, cp, val);
	free(cp);
}

void update_memory_v2(struct Memory *m, char *mask, long idx, long val)
{
	char *result = apply_mask_v2(idx, mask);
	set_float_mask(m, result, val);
	free(result);
}

long solve_problem(char *filename, void update_memory(struct Memory *, char *, long, long))
{
	FILE *fp = NULL;
	int i;
	long mem_idx, mem_val, answer = 0;
	char buf[BUFSIZE], *mask = NULL;
	struct Memory *memory = NULL;

	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "Error opening file %s for reading.\n", filename);
		exit(EXIT_FAILURE);
	}

	memory = memory_init();
	while ((fgets(buf, BUFSIZE, fp)) != NULL) {
		if (str_startswith(buf, "mask")) {
			free(mask);
			mask = parse_mask(buf);
			continue;
		} else if (str_startswith(buf, "mem")) {
			parse_mem(buf, &mem_idx, &mem_val);
		} else {
			fprintf(stderr, "Don't know how to parse line: '%s'. Skipping.\n",
					buf);
			continue;
		}
		update_memory(memory, mask, mem_idx, mem_val);
	}

	fclose(fp);
	free(mask);

	for (i=0; i<memory->n; i++)
		answer += memory->val[i];

	memory_free(memory);
	return answer;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s input_file\n", argv[0]);
		return EXIT_FAILURE;
	}
	long answer;

	answer = solve_problem(argv[1], update_memory_v1);
	printf("Solution part 1: %ld\n", answer);

	answer = solve_problem(argv[1], update_memory_v2);
	printf("Solution part 2: %ld\n", answer);

	return EXIT_SUCCESS;
}
