/**
 * @file day04.c
 * @author G.J.J. van den Burg
 * @date 2020-12-04
 * @brief Advent of Code, Day 04

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

struct Passport {
	char *byr;
	char *iyr;
	char *eyr;
	char *hgt;
	char *hcl;
	char *ecl;
	char *pid;
	char *cid;
};

struct Passport *passport_init(void)
{
	struct Passport *p = malloc(sizeof(struct Passport));
	p->byr = NULL;
	p->iyr = NULL;
	p->eyr = NULL;
	p->hgt = NULL;
	p->hcl = NULL;
	p->ecl = NULL;
	p->pid = NULL;
	p->cid = NULL;
	return p;
}

void passport_free(struct Passport *p)
{
	free(p->byr);
	free(p->iyr);
	free(p->eyr);
	free(p->hgt);
	free(p->hcl);
	free(p->ecl);
	free(p->pid);
	free(p->cid);
	free(p);
	p = NULL;
}

void passport_print(struct Passport *p)
{
	printf("Passport:\n");
	printf("\tbyr = %s\n", p->byr == NULL ? "" : p->byr);
	printf("\tiyr = %s\n", p->iyr == NULL ? "" : p->iyr);
	printf("\teyr = %s\n", p->eyr == NULL ? "" : p->eyr);
	printf("\thgt = %s\n", p->hgt == NULL ? "" : p->hgt);
	printf("\thcl = %s\n", p->hcl == NULL ? "" : p->hcl);
	printf("\tecl = %s\n", p->ecl == NULL ? "" : p->ecl);
	printf("\tpid = %s\n", p->pid == NULL ? "" : p->pid);
	printf("\tcid = %s\n", p->cid == NULL ? "" : p->cid);
	printf("\n");
}

void passport_set(struct Passport *p, char *key, char *val)
{
	size_t l = strlen(val);
	char *buf = calloc(l+1, sizeof(char));
	if (strcpy(buf, val) == NULL) {
		fprintf(stderr, "Error copying value string.\n");
		exit(EXIT_FAILURE);
	}

	if (strcmp(key, "byr") == 0) {
		p->byr = buf;
	} else if (strcmp(key, "iyr") == 0) {
		p->iyr = buf;
	} else if (strcmp(key, "eyr") == 0) {
		p->eyr = buf;
	} else if (strcmp(key, "hgt") == 0) {
		p->hgt = buf;
	} else if (strcmp(key, "hcl") == 0) {
		p->hcl = buf;
	} else if (strcmp(key, "ecl") == 0) {
		p->ecl = buf;
	} else if (strcmp(key, "pid") == 0) {
		p->pid = buf;
	} else if (strcmp(key, "cid") == 0) {
		p->cid = buf;
	} else {
		fprintf(stderr, "Unknown key '%s', skipping.\n", key);
		free(buf);
	}
}

struct Passport **read_file(char *filename, int *N)
{
	char u, v;
	char *key = NULL,
	     *val = NULL;
	bool is_key = true;
	FILE *fp = NULL;
	struct Passport **pps = NULL;
	struct Passport *p = NULL;
	int n = 0;

	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "Error opening file %s for reading.\n", filename);
		exit(EXIT_FAILURE);
	}

	key = calloc(BUFSIZE, sizeof(char));
	val = calloc(BUFSIZE, sizeof(char));

	v = fgetc(fp);
	while ((u = v) && (v = fgetc(fp)) && (u != EOF)) {
		if (u == '\n' && v == '\n') {
			passport_set(p, key, val);
			memset(key, 0, BUFSIZE);
			memset(val, 0, BUFSIZE);
			is_key = true;

			pps = realloc(pps, (n+1) * sizeof(struct Passport *));
			pps[n++] = p;
			p = NULL;
			continue;
		} else if (u == '\n' || u == ' ') {
			p = (p == NULL) ? passport_init() : p;
			if (!(strcmp(key, "") == 0)) { // hacky
				passport_set(p, key, val);
			}
			memset(key, 0, BUFSIZE);
			memset(val, 0, BUFSIZE);
			is_key = true;
			continue;
		} else if (u == ':') {
			is_key = false;
			continue;
		}
		if (is_key)
			strncat(key, &u, 1);
		else
			strncat(val, &u, 1);
	}
	if (p != NULL) {
		pps = realloc(pps, (n+1) * sizeof(struct Passport *));
		pps[n++] = p;
	}

	fclose(fp);
	free(key);
	free(val);

	*N = n;
	return pps;
}

bool is_passport_valid_one(struct Passport *p)
{
	bool is_valid = true;
	is_valid &= (p->byr != NULL);
	is_valid &= (p->iyr != NULL);
	is_valid &= (p->eyr != NULL);
	is_valid &= (p->hgt != NULL);
	is_valid &= (p->hcl != NULL);
	is_valid &= (p->ecl != NULL);
	is_valid &= (p->pid != NULL);
	return is_valid;
}

bool is_valid_year(char *str, int year_min, int year_max)
{
	int year = atoi(str);
	if (year == 0)
		return false;
	return (year_min <= year && year <= year_max);
}

bool is_valid_height(char *hgt)
{
	size_t l = strlen(hgt);
	if (l < 4 || l > 5)
		return false;
	char *sub = calloc(5, sizeof(char));
	int num = 0;
	if (hgt[3] == 'c' && hgt[4] == 'm') {
		strncpy(sub, hgt, 3);
		num = atoi(sub);
		free(sub);
		return (150 <= num && num <= 193);
	} else if (hgt[2] == 'i' && hgt[3] == 'n') {
		strncpy(sub, hgt, 2);
		num = atoi(sub);
		free(sub);
		return (59 <= num && num <= 76);
	}
	free(sub);
	return false;
}

bool is_valid_hair_color(char *hcl)
{
	if (strlen(hcl) != 7)
		return false;
	if (hcl[0] != '#')
		return false;
	for (int i=1; i<7; i++)
		if (!('0' <= hcl[i] && hcl[i] <= '9') && (!('a' <= hcl[i] && hcl[i] <= 'z')))
			return false;
	return true;
}

bool is_valid_eye_color(char *ecl)
{
	if (strcmp(ecl, "amb") == 0) return true;
	if (strcmp(ecl, "blu") == 0) return true;
	if (strcmp(ecl, "brn") == 0) return true;
	if (strcmp(ecl, "gry") == 0) return true;
	if (strcmp(ecl, "grn") == 0) return true;
	if (strcmp(ecl, "hzl") == 0) return true;
	if (strcmp(ecl, "oth") == 0) return true;
	return false;
}

bool is_valid_passport_id(char *pid)
{
	if (strlen(pid) != 9)
		return false;
	for (int i=0; i<9; i++)
		if (!('0' <= pid[i] && pid[i] <= '9'))
			return false;
	return true;
}


bool is_passport_valid_two(struct Passport *p)
{
	if (!is_passport_valid_one(p))
		return false;
	if (!is_valid_year(p->byr, 1920, 2002))
		return false;
	if (!is_valid_year(p->iyr, 2010, 2020))
		return false;
	if (!is_valid_year(p->eyr, 2020, 2030))
		return false;
	if (!is_valid_height(p->hgt))
		return false;
	if (!is_valid_hair_color(p->hcl))
		return false;
	if (!is_valid_eye_color(p->ecl))
		return false;
	if (!is_valid_passport_id(p->pid))
		return false;
	return true;
}
int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("Usage: %s input_file\n", argv[0]);
		return EXIT_FAILURE;
	}
	int ans, i, N;
	struct Passport **passports = NULL;

	passports = read_file(argv[1], &N);

	ans = 0;
	for (i=0; i<N; i++)
		ans += is_passport_valid_one(passports[i]);
	printf("Solution part 1: %d\n", ans);

	ans = 0;
	for (i=0; i<N; i++)
		ans += is_passport_valid_two(passports[i]);
	printf("Solution part 2: %d\n", ans);

	for (i=0; i<N; i++)
		passport_free(passports[i]);
	free(passports);

	return EXIT_SUCCESS;
}
