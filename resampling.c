#include "resampling.h"

// внутренние функции
static double resampling_find_max(double * data, uint32_t length)
{
	int i;
	double max_value = 0xfff0000000000000;
	for (i = 0; i < length; i++)
		if (data[i] > max_value)
			max_value = data[i];

	return max_value;
}

static double resampling_find_min(double * data, uint32_t length)
{
	int i;
	double min_value = 0x7ff0000000000000;
	for (i = 0; i < length; i++)
		if (data[i] < min_value)
			min_value = data[i];

	return min_value;
}

static void resampling_norm(double * data, uint32_t length, double norm_value)
{
	if (0 == norm_value) return;

	int i;
	for (i = 0; i < length; i++)
		data[i] = data[i]/norm_value;
}

static double resampling_find_scale(double * data, uint32_t length, uint32_t target)
{
	double scale = 1;
	double length_in = (double) length;
	if (length == target) return scale;
	if (length < target)
	{
		for (; scale * length_in < target; scale *= 2);
		return scale;
	}
	else
	{
		for (; scale * length_in > target; scale /= 2);
		return scale;
	}
}

// ресэмплирование вверх (дублированием)
int resample_up(double   * data_in,
                uint32_t   length_in,
                double  ** data_out,
                uint32_t * length_out,
                uint32_t   target)
{
	if (NULL == data_in)     return -1;
	if (0 == length_in)      return -1;
	if (NULL == data_out)    return -1;
	if (NULL == length_out)  return -1;
	if (length_in > target)  return -1;

	double scale = resampling_find_scale(data_in, length_in, target);

	double * data = (double *) calloc(length_in * scale, sizeof(double));
	if (NULL == data) return -1;

	int i, j;
	for (i = 0; i < length_in; i++)
		for (j = 0; j < scale; j++)
			data[i * (uint32_t) scale + j] = data_in[i];

	*data_out = data;
	*length_out = length_in * (uint32_t) scale;

	return 0;
}

// ресэмплирование вниз (интерполированием)
int resample_down(double   * data_in,
                  uint32_t   length_in,
                  double  ** data_out,
                  uint32_t * length_out,
                  uint32_t   target)
{
	if (NULL == data_in)     return -1;
	if (0 == length_in)      return -1;
	if (NULL == data_out)    return -1;
	if (0 == length_out)     return -1;
	if (length_in < target)  return -1;

	double scale = resampling_find_scale(data_in, length_in, target);

	double * data = (double *) calloc(length_in * scale, sizeof(double));
	if (NULL == data) return -1;

	int i, j;
	double sum;
	int inv_scale = (uint32_t) (1 / scale);
	printf("%lf\n", scale);
	for (i = 0; i < length_in * scale; i++)
	{
		sum = 0;
		for (j = 0; j < inv_scale; j++)
			sum += data_in[i * inv_scale + j];
		data[i] = sum / inv_scale;
	}

	*data_out = data;
	*length_out = (uint32_t) length_in * scale;

	return 0;
}

// нормирование массива данных
int resample_norm(double * data,
                  uint32_t length)
{
	if (NULL == data) return -1;
	if (0 == length)  return -1;

	double max = resampling_find_max(data, length);
	resampling_norm(data, length, max);

	return 0;
}