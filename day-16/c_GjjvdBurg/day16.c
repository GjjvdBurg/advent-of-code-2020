/**
 * @file aoc2020.c
 * @author G.J.J. van den Burg
 * @date 2020-12-12
 * @brief Advent of Code 2020, Day 16

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
#define matrix_set(M, cols, i, j, val) M[(i)*(cols)+(j)] = val
#define matrix_get(M, cols, i, j) M[(i)*(cols)+(j)]

struct Interval {
	int min;
	int max;
};

struct Ticket {
	int n;
	char **fields;
	int *values;
};

struct Note {
	int n;
	char *label;
	struct Interval **intervals;
};

void *Malloc(size_t size)
{
	void *out = malloc(size);
	if (out == NULL) {
		fprintf(stderr, "Error allocating memory.\n");
		exit(EXIT_FAILURE);
	}
	return out;
}

void *Realloc(void *ptr, size_t size)
{
	void *out = realloc(ptr, size);
	if (out == NULL) {
		fprintf(stderr, "Error allocating memory.\n");
		exit(EXIT_FAILURE);
	}
	return out;
}

struct Interval *init_interval(void)
{
	struct Interval *i = Malloc(sizeof(struct Interval));
	i->min = i->max = 0;
	return i;
}

bool in_interval(struct Interval *i, int num)
{
	return i->min <= num && num <= i->max;
}

void free_interval(struct Interval *i)
{
	free(i);
}

struct Ticket *init_ticket(void)
{
	struct Ticket *t = Malloc(sizeof(struct Ticket));
	t->n = 0;
	t->fields = NULL;
	t->values = NULL;
	return t;
}

void print_ticket(struct Ticket *t)
{
	printf("Ticket(");
	for (int i=0; i<t->n; i++) {
		if (t->fields)
			printf("%s=", t->fields[i]);
		printf("%d%s", t->values[i], (i == t->n - 1) ? "" : ", ");
	}
	printf(")\n");
}

void free_ticket(struct Ticket *t)
{
	if (t->fields)
		for (int i=0; i<t->n; i++)
			free(t->fields[i]);
	free(t->fields);
	free(t->values);
	free(t);
}

struct Note *init_note(void)
{
	struct Note *n = Malloc(sizeof(struct Note));
	n->n = 0;
	n->label = NULL;
	n->intervals = NULL;
	return n;
}

void print_note(struct Note *n)
{
	if (n->label)
		printf("Note(label=%s, [", n->label);
	else
		printf("Note([");
	for (int i=0; i<n->n; i++)
		printf("Interval(min=%d, max=%d)%s", n->intervals[i]->min,
				n->intervals[i]->max, (i < n->n-1) ? ", " : "");
	printf("])\n");
}

void free_note(struct Note *n)
{
	for (int i=0; i<n->n; i++)
		free_interval(n->intervals[i]);
	free(n->intervals);
	free(n->label);
	free(n);
}

bool str_startswith(const char *str, const char *pre)
{
	size_t lenpre = strlen(pre),
	       lenstr = strlen(str);
	return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}

void parse_ticket(struct Ticket *t, const char *str)
{
	char *token = NULL,
	     *ptr = NULL,
	     *copy = NULL;
	int n = 0, num, *numbers = NULL;

	copy = Malloc(sizeof(char) * (strlen(str)+1));
	copy = strcpy(copy, str);
	ptr = copy;

	while ((token = strtok(copy, ","))) {
		copy = NULL;
		num = atoi(token);

		numbers = Realloc(numbers, sizeof(int) * (++n));
		numbers[n - 1] = num;
	}

	t->n = n;
	t->values = numbers;

	free(ptr);
}

void parse_note(struct Note *note, const char *str)
{
	char *token = NULL,
	     *ptr = NULL,
	     *copy = NULL,
	     *label = NULL;
	int i, min, max, num;
	struct Interval *iv = NULL;
	min = max = 0;

	copy = Malloc(sizeof(char) * (strlen(str) + 1));
	copy = strcpy(copy, str);
	ptr = copy;

	token = strtok(copy, ":");
	label = Malloc(sizeof(char) * (strlen(token) + 1));
	label = strcpy(label, token);
	note->label = label;

	while ((token = strtok(NULL, " "))) {
		if (strcmp(token, "or") == 0)
			continue;

		num = min;
		for (i=0; i<strlen(token); i++) {
			if (token[i] == '-') {
				min = num;
				num = max;
				continue;
			}
			num *= 10;
			num += token[i] - '0';
		}
		max = num;

		iv = init_interval();
		iv->min = min;
		iv->max = max;

		note->intervals = Realloc(note->intervals, sizeof(struct Interval *) * (++note->n));
		note->intervals[note->n - 1] = iv;

		min = max = 0;
	}
	free(ptr);
}


void read_file(char *filename, struct Note ***notes, int *n_notes,
		struct Ticket **your_ticket, struct Ticket ***nearby_tickets,
		int *n_nearby)
{
	FILE *fp = NULL;
	bool read_note = true,
	     read_my_ticket = false,
	     read_other_tickets = false;
	char buf[BUFSIZE];
	int num_notes = 0,
	    num_tickets = 0;
	struct Note *note = NULL,
		    **all_notes = NULL;
	struct Ticket *my_ticket = NULL,
		      *next_ticket = NULL,
		      **other_tickets = NULL;

	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "Error opening file %s for reading.\n", filename);
		exit(EXIT_FAILURE);
	}

	while ((fgets(buf, BUFSIZE, fp)) != NULL) {
		if (str_startswith(buf, "your ticket:")) {
			read_note = false;
			read_my_ticket = true;
			continue;
		} else if (str_startswith(buf, "nearby tickets:")) {
			read_other_tickets = true;
			continue;
		} else if (strcmp(buf, "\n") == 0)
			continue;

		if (read_note) {
			note = init_note();
			parse_note(note, buf);
			all_notes = Realloc(all_notes,
					sizeof(struct Note *) * (++num_notes));
			all_notes[num_notes - 1] = note;
		}
		else if (read_my_ticket) {
			my_ticket = init_ticket();
			parse_ticket(my_ticket, buf);
			read_my_ticket = false;
		}
		else if (read_other_tickets) {
			next_ticket = init_ticket();
			parse_ticket(next_ticket, buf);
			other_tickets = Realloc(other_tickets,
					sizeof(struct Ticket *) * (++num_tickets));
			other_tickets[num_tickets - 1] = next_ticket;
		}
	}
	fclose(fp);

	*notes = all_notes;
	*your_ticket = my_ticket;
	*nearby_tickets = other_tickets;
	*n_notes = num_notes;
	*n_nearby = num_tickets;
}

bool value_satisfy_note(struct Note *note, int value)
{
	for (int i=0; i<note->n; i++)
		if (in_interval(note->intervals[i], value))
				return true;
	return false;
}

bool is_ticket_valid(struct Ticket *t, struct Note **notes, int n_notes)
{
	// for a ticket to be valid every value has to satisfy _some_ note
	bool any;
	int i, j, val;
	struct Note *note = NULL;

	for (j=0; j<t->n; j++) {
		any = false;
		val = t->values[j];
		for (i=0; i<n_notes; i++) {
			note = notes[i];
			if (value_satisfy_note(note, val))
				any = true;
		}
		if (!any)
			return false;
	}
	return true;
}

int solution_part_one(struct Note **notes, int n_notes, struct Ticket *mine,
		struct Ticket **nearby, int n_nearby)
{
	bool any;
	int i, j, k, value, total = 0;
	struct Ticket *t = NULL;
	struct Note *note = NULL;

	for (i=0; i<n_nearby; i++) {
		t = nearby[i];
		for (j=0; j<t->n; j++) {
			value = t->values[j];
			any = false;
			for (k=0; k<n_notes; k++) {
				note = notes[k];
				any |= value_satisfy_note(note, value);
			}
			if (!any)
				total += value;
		}
	}
	return total;
}

int *solve_assignment(bool *M, int rows, int columns)
{
	// solve the "assignment" problem using a boolean matrix as input. A 1 
	// in a cell in this matrix is considered "allowed" and an attempt is 
	// made to find an assignment of columns to rows such that all rows 
	// and columns are matched. This is returned as an assignment array of 
	// length columns where assignment[j] = i means row i is assigned to 
	// column j.
	int i, j, total, v_idx, n_idx, todo = columns;
	int *assignment = Malloc(sizeof(int) * columns);
	while (todo) {
		// find the column for which the sum is 1.
		for (j=0; j<columns; j++) {
			total = 0;
			for (i=0; i<rows; i++)
				total += matrix_get(M, columns, i, j);
			if (total == 1)
				break;
		}

		if (j == columns) {
			fprintf(stderr, "No suitable column found.\n");
			free(assignment);
			return NULL;
		}

		v_idx = j;
		n_idx = -1;
		for (i=0; i<rows; i++) {
			if (matrix_get(M, columns, i, v_idx)) {
				n_idx = i;
				break;
			}
		}

		assignment[v_idx] = n_idx;
		todo--;

		for (j=0; j<columns; j++)
			matrix_set(M, columns, n_idx, j, 0);
	}
	return assignment;
}

long solution_part_two_v2(struct Note **notes, int n_notes, struct Ticket *mine,
		struct Ticket **nearby, int n_nearby)
{
	bool all;
	int i, j, k, value, n_values = mine->n;
	int *assignment = NULL;
	long answer = -1;
	char *field = NULL;
	struct Ticket *ticket = NULL;
	struct Note *note = NULL;

	// the constraint matrix is a matrix of n_notes rows by n_values 
	// columns where the value at row i and column j signifies whether all 
	// valid tickets satisfy note[i] for ticket value j.
	bool *constraint_matrix = Malloc(sizeof(bool) * (n_notes * n_values));
	for (i=0; i<n_notes * n_values; i++) constraint_matrix[i] = false;

	// Build the matrix by checking all valid tickets against all notes
	for (i=0; i<n_notes; i++) {
		note = notes[i];
		for (j=0; j<n_values; j++) {
			all = true;
			for (k=0; k<n_nearby; k++) {
				ticket = nearby[k];
				if (!is_ticket_valid(ticket, notes, n_notes))
					continue;

				value = ticket->values[j];
				if (!value_satisfy_note(note, value)) {
					all = false;
					break;
				}
			}

			value = mine->values[j];
			if (!value_satisfy_note(note, value)) {
				all = false;
			}

			matrix_set(constraint_matrix, n_values, i, j, all);
		}
	}

	// solve the assignment problem
	assignment = solve_assignment(constraint_matrix, n_notes, n_values);

	if (assignment == NULL) {
		fprintf(stderr, "Couldn't solve assignment problem.\n");
		goto cleanup;
	}

	// set ticket fields for my ticket for verification
	mine->fields = Malloc(sizeof(char *) * (mine->n));
	for (i=0; i<mine->n; i++) {
		j = assignment[i];
		field = Malloc(sizeof(char) * (strlen(notes[j]->label) + 1));
		field = strcpy(field, notes[j]->label);
		mine->fields[i] = field;
	}

	// compute the answer
	answer = 1;
	for (i=0; i<mine->n; i++) {
		if (str_startswith(mine->fields[i], "departure"))
			answer *= mine->values[i];
	}

cleanup:
	free(assignment);
	free(constraint_matrix);
	return answer;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s input_file\n", argv[0]);
		return EXIT_FAILURE;
	}

	int i, n_notes, n_nearby;
	long ans;
	struct Note **notes = NULL;
	struct Ticket *mine = NULL;
	struct Ticket **nearby = NULL;

	read_file(argv[1], &notes, &n_notes, &mine, &nearby, &n_nearby);

	ans = solution_part_one(notes, n_notes, mine, nearby, n_nearby);
	printf("Solution part 1: %ld\n", ans);

	ans = solution_part_two_v2(notes, n_notes, mine, nearby, n_nearby);
	printf("Solution part 2: %ld\n", ans);

	for (i=0; i<n_notes; i++)
		free_note(notes[i]);
	free(notes);
	for (i=0; i<n_nearby; i++)
		free_ticket(nearby[i]);
	free(nearby);

#ifdef DEBUG
	printf("My ticket:\n");
	print_ticket(mine);
#endif
	free_ticket(mine);

	return EXIT_SUCCESS;
}
