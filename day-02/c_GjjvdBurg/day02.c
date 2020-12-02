/**
 * @file day02.c
 * @author G.J.J. van den Burg
 * @date 2020-12-02
 * @brief Advent of Code 2020, Day 02.

 * Copyright (C) 2020, G.J.J. van den Burg

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

#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define BUFSIZE 1024

struct Record {
	int min_range;
	int max_range;
	char letter;
	char *password;
};

struct Record **read_file(char *filename, int *N)
{
	int i, a, b, n_lines = 0;
	char buf[BUFSIZE];
	char c, pass[BUFSIZE];
	FILE *fp = NULL;

	// figure out how many lines we have
	fp = fopen(filename, "r");
	while (fgets(buf, BUFSIZE, fp) != NULL)
		n_lines++;

	fseek(fp, 0, SEEK_SET);

	struct Record **Rs = malloc(n_lines * sizeof(struct Record *));
	for (i=0; i<n_lines; i++) {
		fgets(buf, BUFSIZE, fp);
		sscanf(buf, "%d-%d %c: %s", &a, &b, &c, pass);

		Rs[i] = malloc(sizeof(struct Record));
		Rs[i]->min_range = a;
		Rs[i]->max_range = b;
		Rs[i]->letter = c;
		Rs[i]->password = malloc((strlen(pass)+1)*sizeof(char));
		strcpy(Rs[i]->password, pass);
	}
	fclose(fp);

	*N = n_lines;
	return Rs;
}

int count_letter_occurrence(char c, char *str)
{
	size_t i, count = 0;
	size_t n = strlen(str);
	for (i=0; i<n; i++)
		count += str[i] == c;
	return count;
}

bool is_valid_record_part_1(struct Record *r)
{
	int count = count_letter_occurrence(r->letter, r->password);
	return (r->min_range <= count && count <= r->max_range);
}

bool is_valid_record_part_2(struct Record *r)
{
	return (r->password[r->min_range - 1] == r->letter) ^
		(r->password[r->max_range - 1] == r->letter);
}

void print_record(struct Record *r)
{
	printf("Record(%d, %d, %c, %s)\n", r->min_range, r->max_range,
			r->letter, r->password);
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("Usage: %s input_file\n", argv[0]);
		return EXIT_FAILURE;
	}

	int i, N, ans = 0;
	struct Record **records = read_file(argv[1], &N);

	for (i=0; i<N; i++)
		ans += is_valid_record_part_1(records[i]);

	printf("Solution part 1: %d\n", ans);

	ans = 0;
	for (i=0; i<N; i++)
		ans += is_valid_record_part_2(records[i]);

	printf("Solution part 2: %d\n", ans);

	// clean up
	for (i=0; i<N; i++) {
		free(records[i]->password);
		free(records[i]);
	}
	free(records);

	return EXIT_SUCCESS;
}
