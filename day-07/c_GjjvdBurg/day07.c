/**
 * @file day07.c
 * @author G.J.J. van den Burg
 * @date 2020-12-07
 * @brief Advent of Code 2020, Day 07

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

#include<regex.h> // POSIX regex.h
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define BUFSIZE 1024

struct Rule {
	char *own_color;

	int n;
	int *counts;
	char **colors;
};

struct RuleList {
	int n;
	struct Rule **rules;
};

struct Rule *init_rule(void)
{
	struct Rule *r = malloc(sizeof(struct Rule));
	r->own_color = NULL;
	r->n = 0;
	r->counts = NULL;
	r->colors = NULL;
	return r;
}

void print_rule(struct Rule *r)
{
	printf("Rule: own color = %s, can contain:\n", r->own_color);
	for (int i=0; i<r->n; i++)
		printf("\t%d %s\n", r->counts[i], r->colors[i]);
	printf("\n");
}

void free_rule(struct Rule *r)
{
	free(r->own_color);
	free(r->counts);
	for (int i=0; i<r->n; i++)
		free(r->colors[i]);
	free(r->colors);
	free(r);
	r = NULL;
}

struct RuleList *init_rule_list(void)
{
	struct RuleList *l = malloc(sizeof(struct RuleList));
	l->n = 0;
	l->rules = NULL;
	return l;
}

void add_rule(struct RuleList *list, struct Rule *r)
{
	list->rules = realloc(list->rules, (++list->n)*sizeof(struct Rule *));
	if (list->rules == NULL) {
		fprintf(stderr, "Insufficient memory for rule list.\n");
		exit(EXIT_FAILURE);
	}
	list->rules[list->n-1] = r;
}

void print_rule_list(struct RuleList *list)
{
	for (int i=0; i<list->n; i++)
		print_rule(list->rules[i]);
}

void free_rule_list(struct RuleList *list)
{
	int i;
	for (i=0; i<list->n; i++)
		free_rule(list->rules[i]);
	free(list->rules);
	free(list);
	list = NULL;
}

char *str_from_match(const char *str, regmatch_t match)
{
	char *buf;
	regoff_t off, len;
	off = match.rm_so;
	len = match.rm_eo - match.rm_so;
	buf = malloc(sizeof(char) * (len + 1));
	strncpy(buf, str+off, len);
	buf[len] = '\0';
	return buf;
}

int int_from_match(const char *str, regmatch_t match)
{
	char *tmp = str_from_match(str, match);
	int num = atoi(tmp);
	free(tmp);
	return num;
}

struct Rule *parse_rule(const char *str)
{
	int cnt;
	char *clr = NULL;

	struct Rule *rule = init_rule();

	const char *re_1 = "^(.*) bags contain ";
	const char *re_2 = "([0-9]+) ([a-z\\ ]+) bags?";

	int retval;
	regex_t regex_1, regex_2;
	regmatch_t match_1[2], match_2[128];

	if (regcomp(&regex_1, re_1, REG_NEWLINE | REG_EXTENDED) != 0) {
		fprintf(stderr, "Error compiling regex: re_1\n");
		exit(EXIT_FAILURE);
	}
	if (regcomp(&regex_2, re_2, REG_NEWLINE | REG_EXTENDED) != 0) {
		fprintf(stderr, "Error compiling regex: re_2\n");
		exit(EXIT_FAILURE);
	}

	retval = regexec(&regex_1, str, 2, match_1, 0);
	if (retval != 0)
		goto cleanup;
	rule->own_color = str_from_match(str, match_1[1]);

	for (int i=0; ; i++) {
		if (regexec(&regex_2, str, 3, match_2, 0))
			break;

		cnt = int_from_match(str, match_2[1]);
		clr = str_from_match(str, match_2[2]);

		rule->n++;
		rule->counts = realloc(rule->counts, sizeof(int) * rule->n);
		rule->colors = realloc(rule->colors, sizeof(char *) * rule->n);

		rule->counts[rule->n - 1] = cnt;
		rule->colors[rule->n - 1] = clr;

		str += match_2[2].rm_eo;
	}

cleanup:
	regfree(&regex_1);
	regfree(&regex_2);

	return rule;
}

struct RuleList *read_file(char *filename)
{
	FILE *fp = NULL;
	char buf[BUFSIZE];

	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "Error opening file %s for reading.\n", filename);
		exit(EXIT_FAILURE);
	}

	struct RuleList *list = init_rule_list();

	while ((fgets(buf, BUFSIZE, fp)) != NULL) {
		buf[strlen(buf)-1] = '\0'; // trim newline
		struct Rule *rule = parse_rule(buf);
		if (rule == NULL)
			continue;
		add_rule(list, rule);
	}

	fclose(fp);
	return list;
}

int get_rule_index(struct RuleList *list, char *color)
{
	for (int i=0; i<list->n; i++)
		if (strcmp(list->rules[i]->own_color, color) == 0)
			return i;
	return -1;
}

// wildly inefficient, should use caching
bool bag_can_contain_other(struct Rule *r, char *other, struct RuleList *list)
{
	int i, ridx;
	// first check level one
	for (i=0; i<r->n; i++) {
		if (strcmp(r->colors[i], other) == 0)
			return true;
	}

	// now depth-first
	for (i=0; i<r->n; i++) {
		ridx = get_rule_index(list, r->colors[i]);
		if (bag_can_contain_other(list->rules[ridx], other, list))
			return true;
	}
	return false;
}


int solution_part_one(struct RuleList *list)
{
	int i, ans = 0;
	for (i=0; i<list->n; i++) {
		ans += bag_can_contain_other(list->rules[i], "shiny gold", list);
	}
	return ans;
}

// again, inefficient without caching
int contains_n_bags(struct Rule *bag, struct RuleList *list)
{
	int idx, n = 0;
	for (int i=0; i<bag->n; i++) {
		n += bag->counts[i];

		idx = get_rule_index(list, bag->colors[i]);
		n += bag->counts[i] * contains_n_bags(list->rules[idx], list);
	}
	return n;
}

int solution_part_two(struct RuleList *list)
{
	int sgidx = get_rule_index(list, "shiny gold");
	struct Rule *shinygold = list->rules[sgidx];
	return contains_n_bags(shinygold, list);
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s input_file\n", argv[0]);
		return EXIT_FAILURE;
	}
	int ans;

	struct RuleList *list = read_file(argv[1]);

	ans = solution_part_one(list);
	printf("Solution part 1: %d\n", ans);

	ans = solution_part_two(list);
	printf("Solution part 2: %d\n", ans);

	free_rule_list(list);

	return EXIT_SUCCESS;
}
