#include "showgraph.h"

int main(int argc, char **argv)
{
	showgraph_t * showgraph;

	int rc = showgraph_create(&showgraph);
	if (rc != 0)
	{
		ERROR("showgraph_create() failed\n");
		return -1;
	}

	rc = showgraph_set_title(showgraph, "CPU Utilization");
	if (rc != 0)
	{
		ERROR("showgraph_set_title() failed\n");
		return -1;
	}

	srand(getpid());
	uint32_t samples_len = 27;
	double samples[samples_len];
	for (int i = 0; i <= samples_len; i++)
		samples[i] = rand()%100;

	rc = showgraph_set_axis_text(&showgraph->x, "X", "W");
	if (rc != 0)
	{
		ERROR("showgraph_set_axis_text() failed\n");
		return -1;
	}

	rc = showgraph_set_axis_text(&showgraph->y, "Y", "m/s");
	if (rc != 0)
	{
		ERROR("showgraph_set_axis_text() failed\n");
		return -1;
	}

	rc = showgraph_set_range(showgraph, 100, 0);
	if (rc != 0)
	{
		ERROR("showgraph_set_range() failed\n");
		return -1;
	}

	rc = showgraph_set_scale(showgraph, 10);
	if (rc != 0)
	{
		ERROR("showgraph_set_scale() failed\n");
		return -1;
	}

	rc = showgraph_set_samples(showgraph, samples, samples_len);
	if (rc != 0)
	{
		ERROR("showgraph_set_samples() failed\n");
		return -1;
	}

	rc = showgraph_print(showgraph);
	if (rc != 0)
	{
		ERROR("showgraph_print() failed\n");
		return -1;
	}

	rc = showgraph_destroy(showgraph);
	if (rc != 0)
	{
		ERROR("showgraph_destroy() failed\n");
		return -1;
	}

	return 0;
}