#ifndef PCM_BITS_CONVERT_H
#define PCM_BITS_CONVERT_H

typedef int (*pcm_bits_convert_func)(void *in, void *out, int chnum, int frames);

pcm_bits_convert_func get_pcm_bits_convert_func(int bits_in, int bits_out);

#endif