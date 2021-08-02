#include <errno.h>
#include "pcm_hal_alsa_lib.h"
#include "pcm_hal.h"
#if (PCM_HAL_ALSA_LIB_SUPPORT == 1)
#include <alsa/asoundlib.h>

#define ALSA_LIB_VERSION(major, minor, subminor) (((major)<<16)|((minor)<<8)|(subminor))

struct alsa_lib_priv {
	char name[32];
	snd_pcm_t *pcm;
	int chnum;
	int rate;
	int bitdepth;
	int period_size;
	int buffer_size;
	int ns_per_frame;
	int state;
};

static int alsa_lib_pcm_open(struct pcm_hal_mgr *mgr, struct pcm_hal_config config)
{
	int ret;
	struct alsa_lib_priv *hal_priv = (struct alsa_lib_priv *)mgr->priv;
	snd_pcm_hw_params_t *params = NULL;
	snd_pcm_sw_params_t *swparams = NULL;
	snd_pcm_uframes_t period_frames = 0;
	snd_pcm_uframes_t buffer_frames = 0;
	snd_pcm_format_t format;
	snd_pcm_t **pcm = &(hal_priv->pcm);

	/* fix capture block mode */
	ret = snd_pcm_open(pcm, config.name, SND_PCM_STREAM_CAPTURE, 0);
	if (ret != 0) {
		print_err("device %s audio open fail(%d)\n", config.name, ret);
		goto err_exit;
	}

	//hw/sw parameters setting
	snd_pcm_hw_params_alloca(&params);
	if (params == NULL)
		goto err_exit1;
	snd_pcm_sw_params_alloca(&swparams);
	if (swparams == NULL)
		goto err_exit1;

	ret = snd_pcm_hw_params_any(*pcm, params);
	if (ret < 0) {
		print_err("device %s Broken configuration for this PCM: no configurations available\n",  config.name);
		goto err_exit1;
	}
	ret = snd_pcm_hw_params_set_access(*pcm, params, SND_PCM_ACCESS_RW_INTERLEAVED);
	if (ret < 0) {
		print_err("device %s set access interleave fail(%d)\n", config.name, ret);
		goto err_exit1;
	}

	if (config.bitdepth == 32)
		format = SND_PCM_FORMAT_S32_LE;
	else if (config.bitdepth == 24)
		format = SND_PCM_FORMAT_S24_3LE;
	else
		format = SND_PCM_FORMAT_S16_LE;

	ret = snd_pcm_hw_params_set_format(*pcm, params, format);
	if (ret < 0) {
		print_err("device %s set access format fail(%d)\n", config.name, ret);
		goto err_exit1;
	}
	ret = snd_pcm_hw_params_set_channels(*pcm, params, config.chnum);
	if (ret < 0) {
		print_err("device %s set access channels fail(%d)\n", config.name, ret);
		goto err_exit1;
	}

	ret = snd_pcm_hw_params_set_rate(*pcm, params, (unsigned int)(config.rate), 0);
	if (ret < 0) {
		print_err("[%s] set access sample rate fail(%d)\n", config.name, ret);
		goto err_exit1;
	}

	period_frames = config.period_size;
	buffer_frames = config.buffer_size;
	ret = snd_pcm_hw_params_set_period_size_near(*pcm, params, &period_frames, 0);
	if (ret < 0) {
		print_err("device %s set period frames fail(%d)\n", config.name, ret);
		goto err_exit1;
	}
	ret = snd_pcm_hw_params_set_buffer_size_near(*pcm, params, &buffer_frames);
	if (ret < 0) {
		print_err("device %s set period buffer fail(%d)\n", config.name, ret);
		goto err_exit1;
	}
	ret = snd_pcm_hw_params(*pcm, params);
	if (ret < 0) {
		print_err("device %s Unable to install hw params(%d)\n", config.name, ret);
		goto err_exit1;
	}

	snd_pcm_hw_params_get_period_size(params, &period_frames, 0);
	snd_pcm_hw_params_get_buffer_size(params, &buffer_frames);
	if ((int)period_frames != config.period_size)
		print_warn("device %s period size change from %d to %d\n", config.name, config.period_size, (int)period_frames);
	if ((int)buffer_frames != config.buffer_size)
		print_warn("device %s period size change from %d to %d\n", config.name, config.buffer_size, (int)buffer_frames);

	snd_pcm_sw_params_current(*pcm, swparams);
	snd_pcm_sw_params_set_start_threshold(*pcm, swparams, config.start_threshold);
	snd_pcm_sw_params_set_tstamp_mode(*pcm, swparams, SND_PCM_TSTAMP_ENABLE);
#if (SND_LIB_VERSION >= ALSA_LIB_VERSION(1, 0, 29))
	snd_pcm_sw_params_set_tstamp_type(*pcm, swparams, SND_PCM_TSTAMP_TYPE_MONOTONIC);
#endif
	ret = snd_pcm_sw_params(*pcm, swparams);
	if ( ret < 0) {
		print_err("device %s unable to install sw params(%d)\n", config.name, ret);
		goto err_exit1;
	}

	strcpy(hal_priv->name, config.name);
	hal_priv->rate = config.rate;
	hal_priv->chnum = config.chnum;
	hal_priv->bitdepth = config.bitdepth;
	hal_priv->period_size = (int)period_frames;
	hal_priv->buffer_size = (int)buffer_frames;
	hal_priv->ns_per_frame = (int)(TSTAMP_PER_SECOND / hal_priv->rate);
	hal_priv->state = 1;

	return 0;

err_exit1:
	snd_pcm_close(*pcm);
	*pcm = NULL;
err_exit:
	return ret;
}

static int alsa_lib_pcm_close(struct pcm_hal_mgr *mgr)
{
	int ret;
	struct alsa_lib_priv *hal_priv = (struct alsa_lib_priv *)mgr->priv;
	snd_pcm_t *pcm = hal_priv->pcm;

	ret = snd_pcm_close(pcm);

	hal_priv->state = 0;
	return ret;
}

static int alsa_lib_pcm_start(struct pcm_hal_mgr *mgr)
{
	int ret;
	struct alsa_lib_priv *hal_priv = (struct alsa_lib_priv *)mgr->priv;
	snd_pcm_t *pcm = hal_priv->pcm;

	ret = snd_pcm_prepare(pcm);
	if (ret<0) {
		print_err("device %s prepare fail(%d)\n", hal_priv->name, ret);
		return ret;
	}

	ret = snd_pcm_start(pcm);
	if (ret != 0)
		print_err("device %s start fail(%d)\n", hal_priv->name, ret);

	return ret;
}

static int alsa_lib_pcm_stop(struct pcm_hal_mgr *mgr)
{
	int ret;
	struct alsa_lib_priv *hal_priv = (struct alsa_lib_priv *)mgr->priv;
	snd_pcm_t *pcm = hal_priv->pcm;

	ret = snd_pcm_drop(pcm);
	if (ret != 0)
		print_err("device %s stop fail(%d)\n", hal_priv->name, ret);

	return ret;
}

static int alsa_lib_pcm_read(struct pcm_hal_mgr *mgr, void *buffer, int frames, int64_t *tstamp)
{
	int ret;
	struct alsa_lib_priv *hal_priv = (struct alsa_lib_priv *)mgr->priv;
	snd_pcm_t *pcm = hal_priv->pcm;
	snd_pcm_status_t *status;
	snd_htimestamp_t timeval;
	snd_pcm_uframes_t avail;
	snd_pcm_state_t state;

	if (tstamp != NULL) {
		snd_pcm_status_alloca(&status);
		ret = snd_pcm_status(pcm, status);
		if (ret < 0)
			return ret;
		snd_pcm_status_get_htstamp(status, &timeval);
		state = snd_pcm_status_get_state(status);
		avail = snd_pcm_status_get_avail(status);
		if (state == SND_PCM_STATE_XRUN) {
			snd_pcm_recover(pcm, -EPIPE, 1);
			snd_pcm_start(pcm);
			return -EPIPE;
		} else if (state == SND_PCM_STATE_SUSPENDED) {
			snd_pcm_resume(pcm);
			snd_pcm_prepare(pcm);
			snd_pcm_start(pcm);
			return -ESTRPIPE;
		}
		*tstamp = (int64_t)(timeval.tv_sec) * TSTAMP_PER_SECOND + (int64_t)(timeval.tv_nsec) - (int64_t)avail * (int64_t)(hal_priv->ns_per_frame);
	}
	ret = snd_pcm_readi(pcm, buffer, frames);
	if (ret == -EPIPE) {
		snd_pcm_recover(pcm, -EPIPE, 1);
		snd_pcm_start(pcm);
		return ret;
	} else if (ret == -ESTRPIPE) {
		snd_pcm_resume(pcm);
		snd_pcm_prepare(pcm);
		snd_pcm_start(pcm);
		return ret;
	}

	if (ret < 0)
		return ret;

	if (ret != frames)
		return -EINTR;

	return 0;
}

int pcm_hal_alsa_lib_create(struct pcm_hal_mgr *mgr)
{
	struct alsa_lib_priv *hal_priv;

	hal_priv = (struct alsa_lib_priv *)calloc(1, sizeof(struct alsa_lib_priv));
	if (hal_priv == NULL)
		return -ENOMEM;

	mgr->type = PCM_HW_ALSA_LIB;
	mgr->pcm_open = alsa_lib_pcm_open;
	mgr->pcm_start = alsa_lib_pcm_start;
	mgr->pcm_stop = alsa_lib_pcm_stop;
	mgr->pcm_close = alsa_lib_pcm_close;
	mgr->pcm_read = alsa_lib_pcm_read;
	hal_priv->pcm = NULL;
	hal_priv->chnum = 0;
	hal_priv->bitdepth = 0;
	hal_priv->rate = 0;
	hal_priv->period_size = 0;
	hal_priv->buffer_size = 0;
	hal_priv->state = 0;

	mgr->priv = (void *)(hal_priv);

	return 0;
}

int pcm_hal_alsa_lib_destroy(struct pcm_hal_mgr *mgr)
{
	struct alsa_lib_priv *hal_priv = (struct alsa_lib_priv *)(mgr->priv);

	if (hal_priv->state == 1)
		mgr->pcm_close(mgr);

	mgr->pcm_open = NULL;
	mgr->pcm_start = NULL;
	mgr->pcm_stop = NULL;
	mgr->pcm_close = NULL;
	mgr->pcm_read = NULL;
	free(hal_priv);

	mgr->priv = NULL;

	return 0;
}
#else
int pcm_hal_alsa_lib_create(struct pcm_hal_mgr *mgr)
{
	return -EPERM;
}

int pcm_hal_alsa_lib_destroy(struct pcm_hal_mgr *mgr)
{
	return -EPERM;
}
#endif
