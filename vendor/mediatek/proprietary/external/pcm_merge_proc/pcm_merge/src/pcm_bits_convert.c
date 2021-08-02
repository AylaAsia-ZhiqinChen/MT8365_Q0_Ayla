#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include "pcm_bits_convert.h"

static int pcm_bits_convert_16_32(void *in, void *out, int chnum, int frames)
{
	int16_t *pcm_in = (int16_t *)in;
	int32_t *pcm_out = (int32_t *)out;
	int total, index;

	if (pcm_in == NULL || pcm_out == NULL)
		return -EINVAL;

	total = chnum * frames;
	for (index = 0; index < total; index++)
		*(pcm_out + index) = (int32_t)(*(pcm_in + index)) << 16;

	return 0;
}

static int pcm_bits_convert_16_24(void *in, void *out, int chnum, int frames)
{
	int8_t *pcm_in = (int8_t *)in;
	int8_t *pcm_out = (int8_t *)out;
	int total, index;

	if (pcm_in == NULL || pcm_out == NULL)
		return -EINVAL;

	total = chnum * frames;
	for (index = 0; index < total; index++) {
		*(pcm_out + index * 3 + 0) = 0;
		*(pcm_out + index * 3 + 1) = *(pcm_in + index * 2 + 1);
		*(pcm_out + index * 3 + 2) = *(pcm_in + index * 2 + 2);
	}

	return 0;
}

static int pcm_bits_convert_32_16(void *in, void *out, int chnum, int frames)
{
	int32_t *pcm_in = (int32_t *)in;
	int16_t *pcm_out = (int16_t *)out;
	int total, index;

	if (pcm_in == NULL || pcm_out == NULL)
		return -EINVAL;

	total = chnum * frames;
	for (index = 0; index < total; index++)
		*(pcm_out + index) = (int16_t)(*(pcm_in + index) >> 16);

	return 0;
}

static int pcm_bits_convert_32_24(void *in, void *out, int chnum, int frames)
{
	int8_t *pcm_in = (int8_t *)in;
	int8_t *pcm_out = (int8_t *)out;
	int total, index;

	if (pcm_in == NULL || pcm_out == NULL)
		return -EINVAL;

	total = chnum * frames;
	for (index = 0; index < total; index++) {
		*(pcm_out + index * 3 + 0) = *(pcm_in + index * 4 + 1);
		*(pcm_out + index * 3 + 1) = *(pcm_in + index * 4 + 2);
		*(pcm_out + index * 3 + 2) = *(pcm_in + index * 4 + 3);
	}

	return 0;
}

static int pcm_bits_convert_24_16(void *in, void *out, int chnum, int frames)
{
	int8_t *pcm_in = (int8_t *)in;
	int8_t *pcm_out = (int8_t *)out;
	int total, index;

	if (pcm_in == NULL || pcm_out == NULL)
		return -EINVAL;

	total = chnum * frames;
	for (index = 0; index < total; index++) {
		*(pcm_out + index * 2 + 0) = *(pcm_in + index * 3 + 1);
		*(pcm_out + index * 2 + 1) = *(pcm_in + index * 3 + 2);
	}

	return 0;
}

static int pcm_bits_convert_24_32(void *in, void *out, int chnum, int frames)
{
	int8_t *pcm_in = (int8_t *)in;
	int8_t *pcm_out = (int8_t *)out;
	int total, index;

	if (pcm_in == NULL || pcm_out == NULL)
		return -EINVAL;

	total = chnum * frames;
	for (index = 0; index < total; index++) {
		*(pcm_out + index * 4 + 0) = 0;
		*(pcm_out + index * 4 + 1) = *(pcm_in + index * 3 + 0);
		*(pcm_out + index * 4 + 2) = *(pcm_in + index * 3 + 1);
		*(pcm_out + index * 4 + 3) = *(pcm_in + index * 3 + 2);
	}

	return 0;
}

pcm_bits_convert_func get_pcm_bits_convert_func(int bits_in, int bits_out)
{
	if (bits_in == 16 && bits_out == 24)
		return pcm_bits_convert_16_24;
	if (bits_in == 16 && bits_out == 32)
		return pcm_bits_convert_16_32;
	if (bits_in == 24 && bits_out == 16)
		return pcm_bits_convert_24_16;
	if (bits_in == 24 && bits_out == 32)
		return pcm_bits_convert_24_32;
	if (bits_in == 32 && bits_out == 16)
		return pcm_bits_convert_32_16;
	if (bits_in == 32 && bits_out == 24)
		return pcm_bits_convert_32_24;

	return NULL;
}
