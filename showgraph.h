#ifndef SHOWGRAPH_H
#define SHOWGRAPH_H

#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>

#define MAX_ALLOWED_WIDTH 120
#define MIN_ALLOWED_WIDTH 40
#define MAX_ALLOWED_SCALE 8

#define ERROR(...) printf("ERROR: "__VA_ARGS__)

typedef struct
{
	char   * name;
	char   * dim;
	uint32_t max;
	uint32_t min;
	uint8_t  scale;
} showgraph_axis_t;

typedef struct
{
	char           * title;
	showgraph_axis_t x;
	showgraph_axis_t y;
	double         * samples;
	uint32_t         samples_len;
	char           * line;
} showgraph_t;

int showgraph_create(showgraph_t ** showgraph);
int showgraph_set_title(showgraph_t * showgraph, char * title);
int showgraph_set_axis_text(showgraph_axis_t * axis, char * name, char * dim);
int showgraph_set_range(showgraph_t * showgraph, uint32_t max, uint32_t min);
int showgraph_set_scale(showgraph_t * showgraph, uint8_t scale);
int showgraph_set_samples(showgraph_t * showgraph, double * samples, uint32_t samples_len);
int showgraph_print(showgraph_t * showgraph);
int showgraph_destroy(showgraph_t * showgraph);

/*
	how it is used:

	showgraph_t * showgraph;
	showgraph_create(&showgraph);
	showgraph_set_title(showgraph, "You title");
	showgraph_set_axis_text(&showgraph.x, "X", "sec");
	showgraph_set_axis_text(&showgraph.x, "Y", "kbps");
	showgraph_set_range(showgraph, 100, 10);
	showgraph_set_scale(showgraph, 10);
	showgraph_set_samples(showgraph, samples, samples_len);
	showgraph_print(showgraph);
	showgraph_destroy(showgraph);
*/

#endif /* SHOWGRAPH_H */