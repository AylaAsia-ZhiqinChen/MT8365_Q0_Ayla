#include "msrc_mgr.h"

#define LOW_QUALITY_INDEX_Q_NUM  24
#define LOW_QUALITY_INDEX_1_VAL  0x1000000

typedef struct {
	int32_t state;
	int32_t last_in_index; /* Q24 */
	int32_t *last_val;
	int16_t *last_val_16;
	int32_t *store_val;
	int16_t *store_val_16;
}simple_private_t;

static int msrc_simple_process_bit32(struct msrc_mgr *src_mgr, msrc_data_t *src_data)
{
	int32_t in_index;
	int32_t i;
	int32_t *in;
	int32_t *out;
	int32_t in_size;
	int32_t out_size;
	int32_t in_frame_pos;
	int32_t out_frame_pos;
	int32_t in_pos;
	int32_t out_pos;
	int32_t src_ratio;
	int32_t chnum;
	simple_private_t *priv;
	bool ratio_change = false;
	int32_t ratio_inc = 0;
	int32_t inc_delta;
	int32_t mod_index;
	int32_t mod_threhold;
	int32_t mod_period;
	static int out_idx = 0;

	/* check the in / out buffer is available */
	if (src_data == NULL)
		return MSRC_EINVAL;

	if (src_data->in_size <= 0 || src_data->out_size <= 0)
		return MSRC_EINVAL;

	in = (int32_t *)src_data->in_buf;
	out = (int32_t *)src_data->out_buf;
	in_size = src_data->in_size;
	out_size = src_data->out_size;

	in_frame_pos = out_frame_pos = 0;
	in_pos = out_pos = 0;

	src_ratio = src_mgr->ratio;
	mod_index = src_mgr->ratio_mod_idx;
	mod_threhold = src_mgr->ratio_mod;
	mod_period = src_mgr->ratio_mod_period;
	chnum = src_mgr->chnum;
	priv = (simple_private_t *)src_mgr->private_data;

	if (src_mgr->ratio_update != 0) {
		ratio_inc = src_mgr->ratio_update - src_mgr->ratio;
		src_mgr->ratio = src_mgr->ratio_update;
		src_mgr->ratio_update = 0;
		if (ratio_inc != 0)
			ratio_change = true;
		ratio_inc /= out_size;
	}
	/* first process, need to process init flag */
	if (priv->state == 1) {
		priv->last_val = in;
		priv->state = 0;
	} else {
		priv->last_val = priv->store_val;
	}

	in_index = priv->last_in_index;

	if (in_index >= LOW_QUALITY_INDEX_1_VAL) {
		inc_delta = (in_index/LOW_QUALITY_INDEX_1_VAL);
		in_frame_pos += inc_delta;
		in_pos += inc_delta * chnum;
		in_index %= LOW_QUALITY_INDEX_1_VAL;
		priv->last_val = in + (in_frame_pos - 1)* chnum;
	}
	while (in_frame_pos < in_size && out_frame_pos < out_size) {
		/* process ratio change smoothly */
		if (ratio_change)
			src_ratio += ratio_inc;
		for (i = 0; i < chnum; i++) {
			out[out_pos + i] = priv->last_val[i] +
				(int32_t)(((int64_t)in_index*((int64_t)in[in_pos + i] - (int64_t)priv->last_val[i]))>>LOW_QUALITY_INDEX_Q_NUM);
		}
		out_frame_pos++;
		out_pos += chnum;

		in_index += (src_ratio + (int32_t)(mod_index < mod_threhold));
		if (mod_threhold != 0 && ++mod_index >= mod_period)
			mod_index = 0;
		if (in_index >= LOW_QUALITY_INDEX_1_VAL) {
			inc_delta = (in_index/LOW_QUALITY_INDEX_1_VAL);
			in_frame_pos += inc_delta;
			in_pos += inc_delta * chnum;
			in_index %= LOW_QUALITY_INDEX_1_VAL;
			priv->last_val = in + (in_frame_pos - 1)* chnum;
		}
	}
	
	/* convert index to next buffer offset */
	if (in_frame_pos >= in_size) {
		in_index += (in_frame_pos - src_data->in_size) * LOW_QUALITY_INDEX_1_VAL;
		in_frame_pos = src_data->in_size;
		in_pos = in_frame_pos * chnum;
	}

	if (in_frame_pos - 1 >= 0) {
		for (i = 0; i < chnum; i++)
			priv->store_val[i] = in[(in_frame_pos - 1)* chnum + i];
	}
	priv->last_in_index = in_index;
	src_data->in_used = in_frame_pos;
	src_data->out_used = out_frame_pos;

	return out_frame_pos;
}

static int msrc_simple_process_bit16(struct msrc_mgr *src_mgr, msrc_data_t *src_data)
{
	int32_t in_index;
	int32_t i;
	int16_t *in;
	int16_t *out;
	int32_t in_size;
	int32_t out_size;
	int32_t in_frame_pos;
	int32_t out_frame_pos;
	int32_t in_pos;
	int32_t out_pos;
	int32_t src_ratio;
	int32_t chnum;
	simple_private_t *priv;
	bool ratio_change = false;
	int32_t ratio_inc = 0;
	int32_t inc_delta;
	int32_t mod_index;
	int32_t mod_threhold;
	int32_t mod_period;
	static int out_idx = 0;

	/* check the in / out buffer is available */
	if (src_data == NULL)
		return MSRC_EINVAL;

	if (src_data->in_size <= 0 || src_data->out_size <= 0)
		return MSRC_EINVAL;

	in = (int16_t *)src_data->in_buf;
	out = (int16_t *)src_data->out_buf;
	in_size = src_data->in_size;
	out_size = src_data->out_size;

	in_frame_pos = out_frame_pos = 0;
	in_pos = out_pos = 0;

	src_ratio = src_mgr->ratio;
	mod_index = src_mgr->ratio_mod_idx;
	mod_threhold = src_mgr->ratio_mod;
	mod_period = src_mgr->ratio_mod_period;
	chnum = src_mgr->chnum;
	priv = (simple_private_t *)src_mgr->private_data;

	if (src_mgr->ratio_update != 0) {
		ratio_inc = src_mgr->ratio_update - src_mgr->ratio;
		src_mgr->ratio = src_mgr->ratio_update;
		src_mgr->ratio_update = 0;
		if (ratio_inc != 0)
			ratio_change = true;
		ratio_inc /= out_size;
	}
	/* first process, need to process init flag */
	if (priv->state == 1) {
		priv->last_val_16 = in;
		priv->state = 0;
	} else {
		priv->last_val_16 = priv->store_val_16;
	}

	in_index = priv->last_in_index;

	if (in_index >= LOW_QUALITY_INDEX_1_VAL) {
		inc_delta = (in_index/LOW_QUALITY_INDEX_1_VAL);
		in_frame_pos += inc_delta;
		in_pos += inc_delta * chnum;
		in_index %= LOW_QUALITY_INDEX_1_VAL;
		priv->last_val_16 = in + (in_frame_pos - 1)* chnum;
	}
	while (in_frame_pos < in_size && out_frame_pos < out_size) {
		/* process ratio change smoothly */
		if (ratio_change)
			src_ratio += ratio_inc;
		for (i = 0; i < chnum; i++) {
			out[out_pos + i] = priv->last_val_16[i] +
				(int16_t)(((int64_t)in_index*((int64_t)in[in_pos + i] - (int64_t)priv->last_val_16[i]))>>LOW_QUALITY_INDEX_Q_NUM);
		}
		out_frame_pos++;
		out_pos += chnum;

		in_index += (src_ratio + (int32_t)(mod_index < mod_threhold));
		if (mod_threhold != 0 && ++mod_index >= mod_period)
			mod_index = 0;
		if (in_index >= LOW_QUALITY_INDEX_1_VAL) {
			inc_delta = (in_index/LOW_QUALITY_INDEX_1_VAL);
			in_frame_pos += inc_delta;
			in_pos += inc_delta * chnum;
			in_index %= LOW_QUALITY_INDEX_1_VAL;
			priv->last_val_16 = in + (in_frame_pos - 1)* chnum;
		}
	}
	
	/* convert index to next buffer offset */
	if (in_frame_pos >= in_size) {
		in_index += (in_frame_pos - src_data->in_size) * LOW_QUALITY_INDEX_1_VAL;
		in_frame_pos = src_data->in_size;
		in_pos = in_frame_pos * chnum;
	}

	if (in_frame_pos - 1 >= 0) {
		for (i = 0; i < chnum; i++)
			priv->store_val_16[i] = in[(in_frame_pos - 1)* chnum + i];
	}
	priv->last_in_index = in_index;
	src_data->in_used = in_frame_pos;
	src_data->out_used = out_frame_pos;

	return out_frame_pos;
}

static int msrc_simple_reset(struct msrc_mgr *src_mgr)
{
	simple_private_t *priv = (simple_private_t *)(src_mgr->private_data);
	if(src_mgr == NULL || priv == NULL)
		return MSRC_EPERM;

	priv->state = 1;
	priv->last_in_index = 0;
	priv->last_val = NULL;
	priv->last_val_16 = NULL;
	if (src_mgr->bitdepth == 32)
		memset(priv->store_val, 0, src_mgr->chnum * sizeof(int32_t));
	else if (src_mgr->bitdepth == 16)
		memset(priv->store_val_16, 0, src_mgr->chnum * sizeof(int16_t));

	return MSRC_OK;
}

int msrc_simple_create(msrc_mgr_t *src_mgr)
{
	simple_private_t *priv;
	if (src_mgr->private_data != NULL)
		return MSRC_EPERM;

	priv = (simple_private_t *)calloc(1, sizeof(simple_private_t));
	if (priv == NULL)
		return MSRC_ENOMEM;

	if (src_mgr->bitdepth == 32) {
		priv->store_val = (int32_t *)calloc(1, src_mgr->chnum * sizeof(int32_t));
		if (priv->store_val == 0) {
			free(priv);
			return MSRC_ENOMEM;
		}
		src_mgr->process = msrc_simple_process_bit32;
	} else {
		priv->store_val_16 = (int16_t *)calloc(1, src_mgr->chnum * sizeof(int16_t));
		if (priv->store_val_16 == 0) {
			free(priv);
			return MSRC_ENOMEM;
		}
		src_mgr->process = msrc_simple_process_bit16;
	}
	src_mgr->private_data = (void *)priv;
	src_mgr->reset = msrc_simple_reset;
	msrc_simple_reset((void *)src_mgr);
	return MSRC_OK;
}

int msrc_simple_destroy(msrc_mgr_t *src_mgr)
{
	simple_private_t *priv = (simple_private_t *)src_mgr->private_data;
	if (priv != NULL) {
		if (priv->store_val != NULL)
			free(priv->store_val);
		if (priv->store_val_16 != NULL)
			free(priv->store_val_16);
		free(priv);
	}

	src_mgr->private_data = NULL;
	src_mgr->process = NULL;
	src_mgr->reset = NULL;

	return MSRC_OK;
}