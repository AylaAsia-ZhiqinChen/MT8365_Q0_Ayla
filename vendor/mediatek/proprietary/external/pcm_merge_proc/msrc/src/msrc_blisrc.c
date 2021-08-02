#include <errno.h>
#include "msrc_blisrc.h"
#if (MSRC_BLISRC_SUPPORT == 1)
#include "Blisrc_exp.h"

typedef struct {
	unsigned int temp_buf_size; // in byte
	unsigned int internal_buf_size; // in byte
	char *temp_buf;
	char *internal_buf;
	Blisrc_Param src_params;
	Blisrc_Handle *src_handle;
	int state;
} blisrc_private_t;

static int msrc_blisrc_process(struct msrc_mgr *src_mgr, msrc_data_t *src_data)
{
	blisrc_private_t *priv = (blisrc_private_t *)(src_mgr->private_data);
	unsigned int size_in, size_out;
	unsigned int frame_size;

	if (priv == NULL)
		return -EINVAL;

	frame_size = (unsigned int)(src_mgr->bitdepth) * (unsigned int)(src_mgr->chnum) / 8;

	size_in = (unsigned int)(src_data->in_size) * frame_size;
	size_out = (unsigned int)(src_data->out_size) * frame_size;

	Blisrc_Process(priv->src_handle,
	               priv->temp_buf,
	               src_data->in_buf,
	               &size_in,
	               src_data->out_buf,
	               &size_out);
	src_data->in_used = src_data->in_size - (int)(size_in / frame_size);
	src_data->out_used = (int)(size_out / frame_size);

	return src_data->out_used;
}

static int msrc_blisrc_reset(struct msrc_mgr *src_mgr)
{
	blisrc_private_t *priv = (blisrc_private_t *)(src_mgr->private_data);

	if (priv == NULL)
		return -EINVAL;

	Blisrc_Reset(priv->src_handle);

	return MSRC_OK;
}

int msrc_blisrc_create(msrc_mgr_t *src_mgr)
{
	blisrc_private_t *priv;
	int ret;

	if (src_mgr->chnum > 2)
		return -EINVAL;

	if (src_mgr->bitdepth != 32 && src_mgr->bitdepth != 16)
		return -EINVAL;

	priv = (blisrc_private_t *)calloc(1, sizeof(blisrc_private_t));
	if (priv == NULL)
		return -ENOMEM;

	if (src_mgr->bitdepth == 32) {
		priv->src_params.PCM_Format = BLISRC_IN_Q1P31_OUT_Q1P31;
	} else if (src_mgr->bitdepth == 16) {
		priv->src_params.PCM_Format = BLISRC_IN_Q1P15_OUT_Q1P15;
	}

	priv->src_params.in_sampling_rate = (unsigned int)(src_mgr->samplerate_in);
	priv->src_params.ou_sampling_rate = (unsigned int)(src_mgr->samplerate_out);
	priv->src_params.in_channel = (unsigned int)(src_mgr->chnum);
	priv->src_params.ou_channel = (unsigned int)(src_mgr->chnum);

	ret = Blisrc_GetBufferSize(&(priv->internal_buf_size), &(priv->temp_buf_size), &(priv->src_params));
	if (ret < 0)
		goto exit;

	if (priv->internal_buf_size > 0) {
		priv->internal_buf = (char *)malloc(priv->internal_buf_size);
		if (priv->internal_buf == NULL) {
			ret = -ENOMEM;
			goto exit;
		}
	}

	if (priv->temp_buf_size > 0) {
		priv->temp_buf = (char *)malloc(priv->temp_buf_size);
		if (priv->temp_buf == NULL) {
			ret = -ENOMEM;
			goto exit;
		}
	}

	ret = Blisrc_Open(&(priv->src_handle), (void *)(priv->internal_buf), &(priv->src_params));
	if (ret < 0)
		goto exit;

	priv->state = 1;
	src_mgr->private_data = (void *)priv;
	src_mgr->reset = msrc_blisrc_reset;
	src_mgr->process = msrc_blisrc_process;

	return 0;

exit:
	if (priv->internal_buf != NULL)
		free(priv->internal_buf);

	if (priv->temp_buf != NULL)
		free(priv->temp_buf);

	free(priv);

	return ret;
}

int msrc_blisrc_destroy(msrc_mgr_t *src_mgr)
{
	blisrc_private_t *priv = (blisrc_private_t *)src_mgr->private_data;

	if (priv->internal_buf != NULL)
		free(priv->internal_buf);

	if (priv->temp_buf != NULL)
		free(priv->temp_buf);

	free(priv);

	src_mgr->private_data = NULL;
	src_mgr->process = NULL;
	src_mgr->reset = NULL;

	return 0;
}

#else
int msrc_blisrc_create(msrc_mgr_t *src_mgr)
{
	return -ENOENT;
}

int msrc_blisrc_destroy(msrc_mgr_t *src_mgr)
{
	return -ENOENT;
}
#endif