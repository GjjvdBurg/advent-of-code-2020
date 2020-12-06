/**
 * @file day06.c
 * @author G.J.J. van den Burg
 * @date 2020-12-06
 * @brief Advent of Code 2020, Day 06

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

struct Group {
	int n;
	int *lengths;
	char **answers;
};

struct Group *init_group(void)
{
	struct Group *g = malloc(sizeof(struct Group));
	if (g == NULL) {
		fprintf(stderr, "Failed to allocate memory for group\n");
		exit(EXIT_FAILURE);
	}
	g->n = 0;
	g->lengths = NULL;
	g->answers = NULL;

	return g;
}

void free_group(struct Group *g)
{
	for (int i=0; i<g->n; i++)
		free(g->answers[i]);
	free(g->answers);
	free(g->lengths);
	free(g);
	g = NULL;
}

void print_group(struct Group *g)
{
	printf("Group:\n");
	for (int i=0; i<g->n; i++)
		printf("\t%02d: %s\n", i, g->answers[i]);
	printf("\n");
}

struct Group **read_file(char *filename, int *N)
{
	FILE *fp = NULL;
	char buf[BUFSIZE];
	char *answer = NULL;
	int l, n = 0;

	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "Error opening file %s for reading.\n", filename);
		exit(EXIT_FAILURE);
	}

	struct Group *g = NULL;
	struct Group **groups = NULL;

	while ((fgets(buf, BUFSIZE, fp)) != NULL) {
		if (g == NULL) {
			g = init_group();

			groups = realloc(groups, (++n) * sizeof(struct Group *));
			if (groups == NULL) {
				fprintf(stderr, "Error allocating memory for groups\n");
				exit(EXIT_FAILURE);
			}
			groups[n-1] = g;
		}

		if (strcmp(buf, "\n") == 0) {
			g = NULL;
			continue;
		}

		l = strlen(buf);
		answer = malloc(sizeof(char) * l);
		strncpy(answer, buf, l - 1);
		answer[l - 1] = '\0';

		g->answers = realloc(g->answers, (++g->n) * sizeof(char *));
		g->lengths = realloc(g->lengths, (g->n) * sizeof(int));
		g->answers[g->n-1] = answer;
		g->lengths[g->n-1] = l - 1;
	}

	fclose(fp);
	*N = n;
	return groups;
}

int group_count_one(struct Group *g)
{
	char *answer = NULL;
	int i, j;
	bool *answered = malloc(26 * sizeof(bool));
	for (j=0; j<26; j++) answered[j] = false;

	for (i=0; i<g->n; i++) {
		answer = g->answers[i];
		for (j=0; j<g->lengths[i]; j++)
			answered[answer[j] - 'a'] = true;
	}

	i = 0;
	for (j=0; j<26; j++)
		i += answered[j];

	free(answered);
	return i;
}

int group_count_two(struct Group *g)
{
	char *answer = NULL;
	int i, j;
	int *answered = malloc(26 * sizeof(int));
	for (j=0; j<26; j++) answered[j] = 0;

	for (i=0; i<g->n; i++) {
		answer = g->answers[i];
		for (j=0; j<g->lengths[i]; j++)
			answered[answer[j] - 'a']++;
	}

	i = 0;
	for (j=0; j<26; j++)
		i += answered[j] == g->n;
	free(answered);
	return i;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s input_file\n", argv[0]);
		return EXIT_FAILURE;
	}

	int i, n, ans = 0;
	struct Group **groups = read_file(argv[1], &n);

	for (i=0; i<n; i++)
		ans += group_count_one(groups[i]);
	printf("Solution part 1: %d\n", ans);

	ans = 0;
	for (i=0; i<n; i++)
		ans += group_count_two(groups[i]);
	printf("Solution part 2: %d\n", ans);

	for (i=0; i<n; i++)
		free_group(groups[i]);
	free(groups);

	return EXIT_SUCCESS;
}
