/**
 * @file day05.c
 * @author G.J.J. van den Burg
 * @date 2020-12-05
 * @brief Advent of Code 2020, Day 05

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
#define maximum(a, b) ((a) > (b)) ? (a) : (b)

char **read_file(char *filename, int *N)
{
	FILE *fp = NULL;
	char buf[BUFSIZE], *bp, **bps;
	int i, l, n = 0;

	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "Error opening file %s for reading.\n", filename);
		exit(EXIT_FAILURE);
	}

	while (fgets(buf, BUFSIZE, fp) != NULL) {
		n++;
	}

	fseek(fp, 0, SEEK_SET);

	bps = malloc(n * sizeof(char *));

	for (i=0; i<n; i++) {
		fgets(buf, BUFSIZE, fp);
		l = strlen(buf);
		bp = malloc(sizeof(char) * l);
		strncpy(bp, buf, l-1);
		bp[l-1] = '\0';
		bps[i] = bp;
	}

	fclose(fp);
	*N = n;
	return bps;
}

int bin_part(char *c, int l, int r) {
	if (c[0] == 'F' || c[0] == 'L') {
		if (l+2 == r)
			return l;
		return bin_part(c+1, l, l + (r - l)/2);
	} else {
		if (l+2 == r)
			return r-1;
		return bin_part(c+1, l + (r - l)/2, r);
	}
}

int get_seat_id(char *bp)
{
	return 8 * bin_part(bp, 0, 128) + bin_part(bp+7, 0, 8);
}

int find_seat(char **bps, int N)
{
	int i, seat;

	bool *present = malloc(sizeof(bool) * 128 * 8);
	for (i=0; i<128*8; i++) present[i] = false;

	for (i=0; i<N; i++)
		present[get_seat_id(bps[i])] = true;

	for (i=1; i<N-1; i++)
		if (present[i-1] && !present[i] && present[i+1])
			seat = i;

	free(present);
	return seat;
}


int main(int argc, char **argv)
{

	if (argc != 2) {
		printf("Usage: %s input_file\n", argv[0]);
		return EXIT_FAILURE;
	}

	int i, N, ans = 0;
	char **boarding_passes = read_file(argv[1], &N);

	for (i=0; i<N; i++)
		ans = maximum(ans, get_seat_id(boarding_passes[i]));
	printf("Solution part 1: %d\n", ans);

	ans = find_seat(boarding_passes, N);
	printf("Solution part 2: %d\n", ans);

	for (i=0; i<N; i++)
		free(boarding_passes[i]);
	free(boarding_passes);

	return EXIT_SUCCESS;
}
