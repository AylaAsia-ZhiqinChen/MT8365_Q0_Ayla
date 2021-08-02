#include "msrc.h"
#include "msrc_mgr.h"
#include "msrc_simple.h"
#include "msrc_blisrc.h"
#include <stdint.h>

static inline bool is_valid_bits(int32_t bits);
static inline bool is_valid_src_ratio(int32_t ratio);
static inline bool is_valid_src_samplerate(int32_t samplerate_in, int32_t samplerate_out);
static inline bool is_valid_src_chnum(int32_t chnum);
static int msrc_factory_create(msrc_mgr_t *src_mgr);
static int msrc_factory_destroy(msrc_mgr_t *src_mgr);

static inline bool is_valid_bits(int32_t bits)
{
	if (bits != 32 && bits != 16)
		return false;
	return true;
}

static inline bool is_valid_src_ratio(int32_t ratio)
{
	/*Now only support 1/64 ~ 64 ratio*/
	if (ratio < MSRC_RATIO_MIN_VAL || ratio > MSRC_RATIO_MAX_VAL)
		return false;
	return true;
}

static inline bool is_valid_src_samplerate(int32_t samplerate_in, int32_t samplerate_out)
{
	int32_t ratio;

	ratio = (int32_t)((int64_t)MSRC_RATIO_1_VAL * (int64_t)samplerate_out / (int64_t)samplerate_in);
	if (ratio < MSRC_RATIO_MIN_VAL || ratio > MSRC_RATIO_MAX_VAL)
		return false;
	return true;
}

static inline bool is_valid_src_chnum(int32_t chnum)
{
	/* now only support 1ch ~ 16ch */
	if (chnum <= 0 || chnum >16)
		return false;
	return true;
}

static int msrc_gcm(int a, int b)
{
	int c = 1;

	while(c != 0) {
		c = a % b;
		a = b;
		b = c;
	}
	return a;
}

static int msrc_factory_create(msrc_mgr_t *src_mgr)
{
	int ret = 0;
	switch(src_mgr->src_type)
	{
	case MSRC_SIMPLE:
		ret = msrc_simple_create(src_mgr);
		break;
	case MSRC_BLISRC:
		ret = msrc_blisrc_create(src_mgr);
		break;
	default:
		/* not support src type */
		ret = -1;
		break;
	}

	return ret;
}

static int msrc_factory_destroy(msrc_mgr_t *src_mgr)
{
	int ret = 0;
	switch(src_mgr->src_type)
	{
		case MSRC_SIMPLE:
			ret = msrc_simple_destroy(src_mgr);
			break;
		case MSRC_BLISRC:
			ret = msrc_blisrc_destroy(src_mgr);
			break;
		default:
			/* not support src type */
			ret = -1;
			break;
	}

	return ret;
}

int msrc_create(msrc_handle_t **handle, msrc_config_t config)
{
	msrc_mgr_t *src_mgr;
	int ret;

	if (!is_valid_bits((int32_t)config.bitdepth)) {
		*handle = NULL;
		return MSRC_EINVAL;
	}

	if (!is_valid_src_samplerate((int32_t)config.samplerate_in, (int32_t)config.samplerate_out)) {
		*handle = NULL;
		return MSRC_EINVAL;
	}

	if (!is_valid_src_chnum((int32_t)config.chnum)) {
		*handle = NULL;
		return MSRC_EINVAL;
	}

	src_mgr = (msrc_mgr_t *)calloc (1, sizeof(msrc_mgr_t));
	if (src_mgr == NULL) {
		*handle = NULL;
		return MSRC_ENOMEM;
	}

	src_mgr->src_type = config.src_type;
	src_mgr->chnum = (int32_t)config.chnum;
	src_mgr->bitdepth = (int32_t)config.bitdepth;
	src_mgr->samplerate_in = (int32_t)config.samplerate_in;
	src_mgr->samplerate_out = (int32_t)config.samplerate_out;
	src_mgr->private_data = NULL;
	if (config.cb_obtain_in == NULL || config.cb_release_in == NULL) {
		src_mgr->proc_mode = MSRC_PROCESS_NORMAL;
		src_mgr->cb_obtain_in = NULL;
		src_mgr->cb_release_in = NULL;
		src_mgr->cb_data = NULL;
	} else {
		src_mgr->proc_mode = MSRC_PROCESS_CALLBACK;
		src_mgr->cb_obtain_in = config.cb_obtain_in;
		src_mgr->cb_release_in = config.cb_release_in;
		src_mgr->cb_data = config.cb_data;
	}
	src_mgr->ratio = (int32_t)((int64_t)MSRC_RATIO_1_VAL * (int64_t)src_mgr->samplerate_in / (int64_t)src_mgr->samplerate_out);
	src_mgr->ratio_mod = (int32_t)((int64_t)MSRC_RATIO_1_VAL * (int64_t)src_mgr->samplerate_in % (int64_t)src_mgr->samplerate_out);
	if (src_mgr->ratio_mod != 0) {
		int32_t gcm;

		src_mgr->ratio_mod_idx = 0;
		src_mgr->ratio_mod_period = src_mgr->samplerate_out;
		gcm = (int32_t)msrc_gcm((int)src_mgr->ratio_mod, (int)src_mgr->ratio_mod_period);
		src_mgr->ratio_mod /= gcm;
		src_mgr->ratio_mod_period /= gcm;
	}
	src_mgr->ratio_update = 0;
	src_mgr->ratio_mod_update = 0;
	src_mgr->ratio_mod_period_update = 0;
	ret = msrc_factory_create(src_mgr);
	if (ret != 0) {
		free(src_mgr);
		*handle = NULL;
		return ret;
	}

	*handle = (msrc_handle_t *)src_mgr;
	return MSRC_OK;
}

int msrc_destroy(msrc_handle_t *handle)
{
	msrc_mgr_t* src_mgr = (msrc_mgr_t*)handle;

	if (handle == NULL)
		return MSRC_EINVAL;
	msrc_factory_destroy(src_mgr);
	free(src_mgr);

	return MSRC_OK;
}

int msrc_reset(msrc_handle_t* handle)
{
	msrc_mgr_t* src_mgr = (msrc_mgr_t*)handle;

	if (handle == NULL)
		return MSRC_EINVAL;

	if (src_mgr->reset == NULL)
		return MSRC_EPERM;
	if (src_mgr->ratio_update != 0) {
		src_mgr->ratio = src_mgr->ratio_update;
		src_mgr->ratio_update = 0;
		src_mgr->ratio_mod = src_mgr->ratio_mod_update;
		src_mgr->ratio_mod_update = 0;
		src_mgr->ratio_mod_period = src_mgr->ratio_mod_period_update;
		src_mgr->ratio_mod_period_update = 0;
	}
	src_mgr->ratio_mod_idx = 0;
	src_mgr->reset(src_mgr);

	return MSRC_OK;
}

int msrc_samplerate_reconfig(msrc_handle_t *handle, int samplerate_in, int samplerate_out)
{
	msrc_mgr_t *src_mgr = (msrc_mgr_t *)handle;
	int32_t mod = 0;
	int32_t period = 0;

	if (handle == NULL)
		return MSRC_EINVAL;

	if (src_mgr->samplerate_in == (int32_t)samplerate_in && src_mgr->samplerate_out == (int32_t)samplerate_out)
		return MSRC_OK;

	src_mgr->samplerate_in = (int32_t)samplerate_in;
	src_mgr->samplerate_out = (int32_t)samplerate_out;
	src_mgr->ratio_update = (int32_t)((int64_t)MSRC_RATIO_1_VAL * (int64_t)src_mgr->samplerate_in / (int64_t)src_mgr->samplerate_out);
	mod = (int32_t)((int64_t)MSRC_RATIO_1_VAL * (int64_t)src_mgr->samplerate_in % (int64_t)src_mgr->samplerate_out);
	if (mod != 0) {
		int32_t gcm;

		period = src_mgr->samplerate_out;
		gcm = (int32_t)msrc_gcm((int)mod, (int)period);
		mod /= gcm;
		period /= gcm;
	}
	src_mgr->ratio_mod_update = mod;
	src_mgr->ratio_mod_period_update = period;

	return MSRC_OK;
}

int msrc_process(msrc_handle_t *handle, msrc_data_t *src_data)
{
	msrc_mgr_t* src_mgr = (msrc_mgr_t*)handle;
	int ret;

	if (src_data->in_buf == NULL || src_data->out_buf == NULL)
		return MSRC_EINVAL;

	if (src_data->in_size <= 0 || src_data->out_size <= 0)
		return MSRC_EINVAL;

	if (src_mgr->proc_mode != MSRC_PROCESS_NORMAL)
		return MSRC_EPERM;

	if (src_mgr->process == NULL)
		return MSRC_EPERM;

	ret = src_mgr->process(src_mgr, src_data);

	return ret;
}

int msrc_cb_read(msrc_handle_t *handle, void *buf, int frames)
{
	msrc_mgr_t *src_mgr = (msrc_mgr_t *)handle;
	msrc_data_t src_data;
	int in_req_size;
	int ret;
	int rem_frames;
	int out_gen = 0;

	if (buf == NULL)
		return MSRC_EINVAL;

	if (frames == 0)
		return MSRC_EINVAL;

	if (src_mgr->proc_mode != MSRC_PROCESS_CALLBACK)
		return MSRC_EPERM;

	if (src_mgr->cb_obtain_in == NULL)
		return MSRC_EPERM;

	src_data.in_buf = NULL;
	src_data.in_size = 0;
	src_data.in_used = 0;

	src_data.out_size = frames;
	src_data.out_buf = buf;
	src_data.out_used = 0;

	while(out_gen < frames) {
		if (src_data.in_size <= 0 || src_data.in_buf == NULL) {
			in_req_size = (int)(((int64_t)(frames - out_gen) * (int64_t)src_mgr->ratio) >> MSRC_RATIO_Q_NUM);
			in_req_size = (in_req_size == 0) ? 1 : in_req_size;
			src_data.in_size = src_mgr->cb_obtain_in(src_mgr->cb_data, &(src_data.in_buf), in_req_size);
			src_data.in_used = 0;
		}

		if (src_data.in_size <= 0 || src_data.in_buf == NULL)
			break;

		src_mgr->proc_mode = MSRC_PROCESS_NORMAL;
		ret = src_mgr->process(src_mgr, &src_data);
		src_mgr->proc_mode = MSRC_PROCESS_CALLBACK;
		if (ret <= 0)
			break;
		if (src_mgr->cb_release_in != NULL)
			src_mgr->cb_release_in(src_mgr->cb_data, &(src_data.in_buf), src_data.in_used);
		out_gen += src_data.out_used;
		src_data.out_buf = (void*)((int32_t *)buf + out_gen * src_mgr->chnum);
		src_data.out_size = frames - out_gen;
		src_data.out_used = 0;
		src_data.in_buf = NULL;
		src_data.in_size = 0;
	}

	return out_gen;
}