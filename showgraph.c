#include "showgraph.h"

static inline void safe_free(void * ptr)
{
	if (ptr != NULL) free(ptr);
}

static inline int safe_strlen(void * ptr)
{
	if (ptr != NULL) return strlen(ptr);
	else return 0;
}

int showgraph_create(showgraph_t ** showgraph)
{
	if (NULL == showgraph)
	{
		ERROR("Invalid showgraph pointer\n");
		return -1;
	}

	showgraph_t * showgraph_in = (showgraph_t *) malloc(sizeof(showgraph_t));
	if (NULL == showgraph_in)
	{
		ERROR("Memory allocation failed\n");
		return -1;
	}

	memset(showgraph_in, 0, sizeof(*showgraph_in));
	showgraph_in->x.scale = 1;

	*showgraph = showgraph_in;

	return 0;
}

int showgraph_set_title(showgraph_t * showgraph, char * title)
{
	if (NULL == showgraph)
	{
		ERROR("Invalid showgraph pointer\n");
		return -1;
	}

	if (NULL == title)
	{
		ERROR("Invalid title pointer\n");
		return -1;
	}

	showgraph->title = strdup(title);
	if (NULL == showgraph->title)
	{
		ERROR("strdup() failed\n");
		return -1;
	}

	return 0;
}


int showgraph_set_axis(showgraph_axis_t * axis, char * name, char * dim, uint32_t max, uint32_t min, uint32_t step)
{
	if (NULL == axis)
	{
		ERROR("Invalid axis pointer\n");
		return -1;
	}

	if (0 == max || max <= min || 0 == step || (max - min) % step != 0)
	{
		ERROR("Invalid max/min/step value\n");
		return -1;
	}

	if (NULL != name)
	{
		axis->name = strdup(name);
		if (NULL == axis->name)
		{
			ERROR("strdup() failed\n");
			return -1;
		}
	}

	if (NULL != dim)
	{
		axis->dim = strdup(dim);
		if (NULL == axis->dim)
		{
			ERROR("strdup() failed\n");
			safe_free(axis->name);
			return -1;
		}
	}

	axis->max  = max;
	axis->min  = min;
	axis->step = step;

	return 0;
}

static int showgraph_resampling(showgraph_t * showgraph, double ** samples, uint32_t * samples_len)
{
	if (NULL == showgraph)
	{
		ERROR("Invalid showgraph pointer\n");
		return -1;
	}

	if (NULL == samples || NULL == samples_len)
	{
		ERROR("Invalid samples pointers\n");
		return -1;
	}

	int scale = 1;

	if (*samples_len < MIN_ALLOWED_WIDTH)
	{
		for (scale = 1; *samples_len * scale < MIN_ALLOWED_WIDTH; scale = scale << 1);
		if (scale > MAX_ALLOWED_SCALE)
		{
			ERROR("Exceeded allowable scaling factor\n");
			return -1;
		}

		showgraph->samples = (double *) calloc(*samples_len * scale, sizeof(double));
		if (NULL == showgraph->samples)
		{
			ERROR("calloc() failed\n");
			return -1;
		}

		double * samples_in = *samples;

		for (int i = 0; i < *samples_len; i++)
			for (int j = 0; j < scale; j++)
				showgraph->samples[i * scale + j] = samples_in[i];

		showgraph->x.scale = scale;
	}

	return 0;
}

int showgraph_set_samples(showgraph_t * showgraph, double * samples, uint32_t samples_len)
{
	if (NULL == showgraph)
	{
		ERROR("Invalid showgraph pointer\n");
		return -1;
	}

	if (samples_len < MIN_ALLOWED_WIDTH)
	{
		return showgraph_resampling(showgraph, &samples, &samples_len);
	}

	if (samples_len > MAX_ALLOWED_WIDTH)
	{
		ERROR("The number of samples exceeds the maximum allowed\n");
		return -1;
	}

	showgraph->samples = (double *) calloc(samples_len, sizeof(double));
	if (NULL == showgraph->samples)
	{
		ERROR("calloc() failed\n");
		return -1;
	}

	memcpy(showgraph->samples, samples, samples_len * sizeof(double));
	showgraph->samples_len = samples_len;

	return 0;
}

int showgraph_destroy(showgraph_t * showgraph)
{
	if (NULL == showgraph)
	{
		ERROR("Invalid showgraph pointer\n");
		return -1;
	}

	safe_free(showgraph->x.name);
	safe_free(showgraph->x.dim);
	safe_free(showgraph->y.name);
	safe_free(showgraph->y.dim);
	safe_free(showgraph->line);
	safe_free(showgraph->samples);
	safe_free(showgraph->title);
	safe_free(showgraph);

	return 0;
}

static int showgraph_line_add(showgraph_t * showgraph, const char * format, ...)
{
	if (NULL == showgraph)
	{
		ERROR("Invalid showgraph pointer\n");
		return -1;
	}

	char * new_line        = NULL;
	char * old_line        = showgraph->line;
	char * realloc_protect = old_line;
	int    new_line_length = 0;
	int    old_line_length = safe_strlen(old_line);

	va_list args;
	va_start(args, format);

	new_line_length = vasprintf(&new_line, format, args);
	if (!new_line_length)
	{
		ERROR("asprintf() failed\n");
		return -1;
	}

	va_end(args);

	old_line = (char *) realloc(old_line, old_line_length + new_line_length + 1);
	if (NULL == old_line)
	{
		ERROR("realloc() failed\n");
		safe_free(realloc_protect);
		safe_free(new_line);
		return -1;
	}

	memcpy(old_line + old_line_length, new_line, new_line_length);
	old_line[old_line_length + new_line_length] = '\0';

	safe_free(new_line);
	showgraph->line = old_line;

	return 0;
}

static int showgraph_make(showgraph_t * showgraph)
{
	if (NULL == showgraph)
	{
		ERROR("Invalid showgraph pointer\n");
		return -1;
	}

	int rc = showgraph_line_add(showgraph, "%*s\n\n", (int)((showgraph->x.max * showgraph->x.scale - showgraph->x.min * showgraph->x.scale)/showgraph->x.step/2 + 5 + strlen(showgraph->title)/2), showgraph->title);
	if (rc != 0)
	{
		ERROR("showgraph_line_add() failed\n");
		return -1;
	}

	rc = showgraph_line_add(showgraph, "%s, %s\n\n", showgraph->y.name, showgraph->y.dim);
	if (rc != 0)
	{
		ERROR("showgraph_line_add() failed\n");
		return -1;
	}

	for (int y = showgraph->y.max; y >= showgraph->y.min; y-=showgraph->y.step)
	{
		rc = showgraph_line_add(showgraph, "%3d |", y);
		if (rc != 0)
		{
			ERROR("showgraph_line_add() failed\n");
			return -1;
		}

		if (y == showgraph->y.min) break;

		for (int x = showgraph->x.min * showgraph->x.scale; x < showgraph->x.max * showgraph->x.scale; x += showgraph->x.step)
		{
			rc = showgraph_line_add(showgraph, (showgraph->samples[x/showgraph->x.step] >= y) ? "#" : " ");
			if (rc != 0)
			{
				ERROR("showgraph_line_add() failed\n");
				return -1;
			}
		}

		rc = showgraph_line_add(showgraph, "\n");
		if (rc != 0)
		{
			ERROR("showgraph_line_add() failed\n");
			return -1;
		}
	}

	for (int x = showgraph->x.min * showgraph->x.scale + showgraph->x.step; x <= showgraph->x.max * showgraph->x.scale; x += showgraph->x.step)
	{
		rc = showgraph_line_add(showgraph, "%s", (x/showgraph->x.step%(5 * (int)showgraph->x.scale) == 0) ? "|" : ".");
		if (rc != 0)
		{
			ERROR("showgraph_line_add() failed\n");
			return -1;
		}
	}

	rc = showgraph_line_add(showgraph, "\n");
	if (rc != 0)
	{
		ERROR("showgraph_line_add() failed\n");
		return -1;
	}

	rc = showgraph_line_add(showgraph, "    %d", showgraph->x.min );
	if (rc != 0)
	{
		ERROR("showgraph_line_add() failed\n");
		return -1;
	}

	for (int x = showgraph->x.min * showgraph->x.scale + showgraph->x.step; x <= showgraph->x.max * showgraph->x.scale; x += showgraph->x.step)
	{
		if (x/showgraph->x.step%(5 * (int)showgraph->x.scale) == 0)
		{
			rc = showgraph_line_add(showgraph, "%*d", 5 * (int)showgraph->x.scale, x / (int)showgraph->x.scale);
			if (rc != 0)
			{
				ERROR("showgraph_line_add() failed\n");
				return -1;
			}
		}
	}

	rc = showgraph_line_add(showgraph, "\n");
	if (rc != 0)
	{
		ERROR("showgraph_line_add() failed\n");
		return -1;
	}

	rc = showgraph_line_add(showgraph, "    %*s, %s", showgraph->x.max * (int)showgraph->x.scale/showgraph->x.step, showgraph->x.name, showgraph->x.dim);
	if (rc != 0)
	{
		ERROR("showgraph_line_add() failed\n");
		return -1;
	}

	return 0;
}

int showgraph_print(showgraph_t * showgraph)
{
	if (NULL == showgraph)
	{
		ERROR("Invalid showgraph pointer\n");
		return -1;
	}

	int rc = showgraph_make(showgraph);
	if (rc != 0)
	{
		ERROR("showgraph_make() failed\n");
		return -1;
	}

	if (NULL == showgraph->line)
	{
		ERROR("showgraph line is empty\n");
		return -1;
	}

	printf("%s\n", showgraph->line);

	return 0;
}