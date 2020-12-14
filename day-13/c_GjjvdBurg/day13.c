/**
 * @file aoc2020.c
 * @author G.J.J. van den Burg
 * @date 2020-12-12
 * @brief Advent of Code 2020, Day 13

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
#include<limits.h>
#include<gmp.h>

#define BUFSIZE 1024

char *read_file(char *filename, long *earliest)
{
	FILE *fp = NULL;
	char buf[BUFSIZE];

	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "Error opening file %s for reading.\n", filename);
		exit(EXIT_FAILURE);
	}

	fgets(buf, BUFSIZE, fp);
	*earliest = atol(buf);
	fgets(buf, BUFSIZE, fp);

	char *schedule = malloc(sizeof(char) * (strlen(buf) + 1));
	strcpy(schedule, buf);

	fclose(fp);
	return schedule;
}

long solution_part_one(const char *schedule, long earliest)
{
	char *token = NULL;
	long bus_id, wait, best_id, best_wait, x;
	best_id = -1;
	best_wait = LONG_MAX;

	char *copy = malloc(sizeof(char) * (strlen(schedule) + 1));
	copy = strcpy(copy, schedule);
	char *ptr = copy;

	while ((token = strtok(copy, ",")) != NULL) {
		copy = NULL;
		if (strcmp(token, "x") == 0)
			continue;

		bus_id = atol(token);
		x = bus_id;
		while (x < earliest)
			x += bus_id;

		wait = x - earliest;
		if (wait < best_wait) {
			best_wait = wait;
			best_id = bus_id;
		}
	}

	free(ptr);
	return best_wait * best_id;
}

long parse_schedule(const char *schedule, long **ids, long **off)
{
	long i = 0, n = 0;
	long *bus_ids = NULL;
	long *offsets = NULL;

	char *token = NULL;
	char *copy = malloc(sizeof(char) * (strlen(schedule) + 1));
	copy = strcpy(copy, schedule);
	char *ptr = copy;

	while ((token = strtok(copy, ",")) != NULL) {
		copy = NULL;
		i++;

		if (strcmp(token, "x") == 0)
			continue;

		n++;
		bus_ids = realloc(bus_ids, sizeof(long) * n);
		offsets = realloc(offsets, sizeof(long) * n);
		if (bus_ids == NULL || offsets == NULL) {
			fprintf(stderr, "Error allocating memory.\n");
			exit(EXIT_FAILURE);
		}

		bus_ids[n-1] = atol(token);
		offsets[n-1] = i-1;
	}

	free(ptr);

	*ids = bus_ids;
	*off = offsets;
	return n;
}

long gcd(long a, long b)
{
	if (a == b)
		return a;
	if (a > b)
		return gcd(a - b, b);
	return gcd(a, b - a);
}

bool all_coprime(long *S, long n)
{
	int i, j;
	for (i=0; i<n; i++) {
		for (j=i+1; j<n; j++) {
			if (gcd(S[i], S[j]) != 1)
				return false;
		}
	}
	return true;
}

// mpz actually has this, apparently
void mpz_extended_gcd(mpz_t a, mpz_t b, mpz_t x, mpz_t y, mpz_t gcd)
{
	// finds a * x + b * y = gcd(a, b) and returns x, y, gcd(a, b)
	mpz_t old_r, old_s, old_t, r, s, t, tmp, tmp2, quotient;
	mpz_inits(old_r, old_s, old_t, r, s, t, tmp, tmp2, quotient, NULL);

	mpz_set(old_r, a);
	mpz_set_ui(old_s, 1);
	mpz_set_ui(old_t, 0);
	mpz_set(r, b);
	mpz_set_ui(s, 0);
	mpz_set_ui(t, 1);

	while (mpz_sgn(r) != 0) {
		mpz_fdiv_q(quotient, old_r, r);

		mpz_set(tmp, old_r);
		mpz_set(old_r, r);
		mpz_mul(tmp2, quotient, r);
		mpz_sub(r, tmp, tmp2);

		mpz_set(tmp, old_s);
		mpz_set(old_s, s);
		mpz_mul(tmp2, quotient, s);
		mpz_sub(s, tmp, tmp2);

		mpz_set(tmp, old_t);
		mpz_set(old_t, t);
		mpz_mul(tmp2, quotient, t);
		mpz_sub(t, tmp, tmp2);
	}

	mpz_set(x, old_s);
	mpz_set(y, old_t);
	mpz_set(gcd, old_r);

	mpz_clears(old_r, old_s, old_t, r, s, t, tmp, tmp2, quotient, NULL);
}

void mpz_construct_solution(mpz_t result, mpz_t a1, mpz_t n1, mpz_t a2, mpz_t n2)
{
	//long m1, m2, n12, x, g;
	mpz_t m1, m2, g, n12, x, u, v;
	mpz_inits(m1, m2, g, n12, x, u, v, NULL);

	// find m1, m2 from the extended gcd algorithm, such that:
	// m1 * n1 + m2 * n2 = g
	mpz_extended_gcd(n1, n2, m1, m2, g);

	// n12 = n1 * n2
	mpz_mul(n12, n1, n2);

	// u = a1 * m2 * n2
	mpz_mul(u, a1, m2);
	mpz_mul(u, u, n2);

	// v = a2 * m1 * n1
	mpz_mul(v, a2, m1);
	mpz_mul(v, v, n1);

	// x = (u + v) % n12
	mpz_add(x, u, v);
	mpz_mod(x, x, n12);

	mpz_set(result, x);
	mpz_clears(m1, m2, g, n12, x, u, v, NULL);
}

void mpz_chinese_remainder(mpz_t result, long *A, long *N, long k)
{
	long i;
	mpz_t a1, a2, n1, n2, n12, x;
	mpz_inits(a1, a2, n1, n2, n12, x, NULL);

	mpz_t *a = malloc(sizeof(mpz_t) * k);
	mpz_t *n = malloc(sizeof(mpz_t) * k);
	for (i=0; i<k; i++) {
		mpz_init(a[i]);
		mpz_set_si(a[i], -A[i]);
		mpz_init(n[i]);
		mpz_set_si(n[i], N[i]);
	}

	for (i=0; i<k-1; i++) {
		mpz_set(a1, a[i]);
		mpz_set(a2, a[i+1]);
		mpz_set(n1, n[i]);
		mpz_set(n2, n[i+1]);

		mpz_construct_solution(x, a1, n1, a2, n2);

		mpz_mul(n12, n1, n2);
		mpz_set(a[i+1], x);
		mpz_set(n[i+1], n12);
	}

	mpz_mod(x, a[k-1], n[k-1]);
	mpz_set(result, x);

	for (i=0; i<k; i++) {
		mpz_clear(a[i]);
		mpz_clear(n[i]);
	}
	mpz_clears(a1, a2, n1, n2, n12, x, NULL);
	free(a);
	free(n);
}

char *solution_part_two(char *schedule)
{
	long n;
	long *bus_ids = NULL;
	long *offsets = NULL;

	n = parse_schedule(schedule, &bus_ids, &offsets);

	// we want to find t such that: t + off_i == ID_i * N_i for all i
	// equivalently: remainder(t + off_i, ID_i) == 0
	// or: t % ID_i = off_i, which suggests the CRT.
	// Are the ID's mutually coprime?
	if (!all_coprime(bus_ids, n)) {
		fprintf(stderr, "Algorithm for non-coprime bus IDs not implemented.\n");
		exit(EXIT_FAILURE);
	}

	mpz_t t;
	mpz_init(t);
	mpz_chinese_remainder(t, offsets, bus_ids, n);

	free(bus_ids);
	free(offsets);

	char *s = mpz_get_str(NULL, 10, t);
	mpz_clear(t);
	return s;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s input_file\n", argv[0]);
		return EXIT_FAILURE;
	}

	long earliest;
	char *schedule = read_file(argv[1], &earliest);

	long ans1 = solution_part_one(schedule, earliest);
	printf("Solution part 1: %ld\n", ans1);

	char *ans2 = solution_part_two(schedule);
	printf("Solution part 2: %s\n", ans2);

	free(schedule);
	free(ans2);

	return EXIT_SUCCESS;
}
