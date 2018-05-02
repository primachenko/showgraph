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

#define ERROR(...) printf("ERROR: "__VA_ARGS__)

typedef struct
{
	char   * name;
	char   * dim;
	uint32_t max;
	uint32_t min;
	uint32_t step;
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
int showgraph_set_axis(showgraph_axis_t * axis, char * name, char * dim, uint32_t max, uint32_t min, uint32_t step);
int showgraph_set_samples(showgraph_t * showgraph, double * samples, uint32_t samples_len);
int showgraph_print(showgraph_t * showgraph);
int showgraph_destroy(showgraph_t * showgraph);

#endif /* SHOWGRAPH_H */