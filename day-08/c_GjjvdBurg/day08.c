/**
 * @file day08.c
 * @author G.J.J. van den Burg
 * @date 2020-12-08
 * @brief Advent of Code 2020, Day 08.

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
#include<string.h>
#include<stdbool.h>

#define BUFSIZE 1024

struct Instruction {
	char *in;
	int arg;
	bool visited;
};

struct Instruction *init_instruction(void)
{
	struct Instruction *in = malloc(sizeof(struct Instruction));
	in->in = NULL;
	in->arg = 0;
	in->visited = false;
	return in;
}

void print_instruction(struct Instruction *in)
{
	printf("Instruction: %s (%d)\n", in->in, in->arg);
}

void free_instruction(struct Instruction *in)
{
	free(in->in);
	free(in);
	in = NULL;
}

struct Instruction *parse_line(char *line) {
	char *token = NULL;
	char *tmp = NULL;
	struct Instruction *in = init_instruction();

	token = strtok(line, " ");
	in->in = malloc(sizeof(char) * (strlen(token) + 1));
	strcpy(in->in, token);

	token = strtok(NULL, " ");
	tmp = malloc(sizeof(char) * strlen(token) + 1);
	strcpy(tmp, token);
	in->arg = atoi(tmp);
	free(tmp);

	return in;
}

struct Instruction **read_file(char *filename, int *N)
{
	FILE *fp = NULL;
	char buf[BUFSIZE];
	struct Instruction *instruct = NULL;
	struct Instruction **list = NULL;
	int n = 0;

	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "Error opening file %s for reading.\n", filename);
		exit(EXIT_FAILURE);
	}

	while ((fgets(buf, BUFSIZE, fp)) != NULL) {
		instruct = parse_line(buf);
		if (instruct == NULL) {
			fprintf(stderr, "Error reading line: '%s'\n", buf);
			exit(EXIT_FAILURE);
		}

		list = realloc(list, ++n * sizeof(struct Instruction *));
		if (list == NULL) {
			fprintf(stderr, "Error allocating memory for list\n");
			exit(EXIT_FAILURE);
		}

		list[n-1] = instruct;
	}

	fclose(fp);

	*N = n;
	return list;
}

int accumulator_part_one(struct Instruction **list, int N)
{
	int pos = 0;
	int acc = 0;
	struct Instruction *in = NULL;
	for (pos=0; pos<N; pos++)
		list[pos]->visited = false;

	pos = 0;
	while (pos <= N-1) {
		in = list[pos];
		if (in->visited)
			break;

		if (strcmp(in->in, "nop") == 0) {
			pos++;
		} else if (strcmp(in->in, "acc") == 0) {
			acc += in->arg;
			pos++;
		} else if (strcmp(in->in, "jmp") == 0) {
			pos += in->arg;
		}
		in->visited = true;
	}
	return acc;
}

bool will_loop_forever(struct Instruction **list, int N)
{
	int pos = 0;
	struct Instruction *in = NULL;
	for (pos=0; pos<N; pos++)
		list[pos]->visited = false;

	pos = 0;
	while (pos <= N-1) {
		in = list[pos];
		if (in->visited)
			return true;
		if (strcmp(in->in, "nop") == 0 || strcmp(in->in, "acc") == 0)
			pos++;
		else if (strcmp(in->in, "jmp") == 0)
			pos += in->arg;
		in->visited = true;
	}
	return false;
}

int accumulator_part_two(struct Instruction **list, int N)
{
	int pos = 0;
	int acc = 0;
	char *tmp = NULL;
	struct Instruction *in = NULL;

	for (pos=0; pos<N; pos++) {
		in = list[pos];
		if (strcmp(in->in, "nop") == 0 || strcmp(in->in, "acc") == 0)
			continue;

		tmp = in->in;
		in->in = "nop";
		if (!will_loop_forever(list, N)) {
			break;
		}
		in->in = tmp;
	}
	acc = accumulator_part_one(list, N);
	in->in = tmp;
	return acc;
}

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("Usage %s input_file\n", argv[0]);
		return EXIT_FAILURE;
	}

	int i, N, ans;
	struct Instruction **list = read_file(argv[1], &N);

	ans = accumulator_part_one(list, N);
	printf("Solution part 1: %d\n", ans);

	ans = accumulator_part_two(list, N);
	printf("Solution part 2: %d\n", ans);

	for (i=0; i<N; i++) {
		free_instruction(list[i]);
	}
	free(list);

	return EXIT_SUCCESS;
}
