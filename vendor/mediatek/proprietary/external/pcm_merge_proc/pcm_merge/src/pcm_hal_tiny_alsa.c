#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pcm_hal_tiny_alsa.h"
#include "pcm_hal.h"
#if (PCM_HAL_TINY_ALSA_SUPPORT == 1)
#include <tinyalsa/asoundlib.h>

struct tiny_alsa_priv {
	char name[32];
	struct pcm *pcm;
	int chnum;
	int rate;
	int bitdepth;
	int period_size;
	int buffer_size;
	int ns_per_frame;
};

struct tiny_alsa_priv info;

static int tiny_alsa_pcm_open(struct pcm_hal_mgr *mgr, struct pcm_hal_config config)
{
	int ret;
	struct tiny_alsa_priv *hal_priv = (struct tiny_alsa_priv *)mgr->priv;
	struct pcm_config pcm_config;
	int card_idx, pcm_idx;
	struct pcm *pcm = NULL;

	memset(&pcm_config, 0, sizeof(struct pcm_config));
	if (config.bitdepth == 32)
		pcm_config.format = PCM_FORMAT_S32_LE;
	else if(config.bitdepth == 24)
		pcm_config.format = PCM_FORMAT_S24_3LE;
	else
		pcm_config.format = PCM_FORMAT_S16_LE;
	pcm_config.rate = config.rate;
	pcm_config.channels = config.chnum;
	pcm_config.period_size = config.period_size;
	pcm_config.period_count = config.buffer_size / config.period_size;
	pcm_config.start_threshold = 0;
	pcm_config.stop_threshold = 0;
	pcm_config.silence_threshold = 0;

	ret = merge_string_to_alsa_id(&card_idx, &pcm_idx, config.name);
	if (ret < 0) {
		print_err("pcm device name is not correct(%d)\n", ret);
		return ret;
	}
	pcm = pcm_open(card_idx, pcm_idx, PCM_IN | PCM_MONOTONIC, &pcm_config);
	if (pcm == NULL || (pcm != NULL && !pcm_is_ready(pcm))) {
		print_err("card %d device %d open fail\n", card_idx, pcm_idx);
		return -EINVAL;
	}
	strcpy(hal_priv->name, config.name);
	hal_priv->rate = config.rate;
	hal_priv->chnum = config.chnum;
	hal_priv->bitdepth = config.bitdepth;
	hal_priv->period_size = (int)config.period_size;
	hal_priv->buffer_size = (int)config.buffer_size;
	hal_priv->ns_per_frame = (int)(TSTAMP_PER_SECOND / hal_priv->rate);
	hal_priv->pcm = pcm;

	return 0;
}

static int tiny_alsa_pcm_close(struct pcm_hal_mgr *mgr)
{
	struct tiny_alsa_priv *hal_priv = (struct tiny_alsa_priv *)mgr->priv;
	struct pcm *pcm = hal_priv->pcm;

	pcm_close(pcm);

	return 0;
}

static int tiny_alsa_pcm_start(struct pcm_hal_mgr *mgr)
{
	int ret;
	struct tiny_alsa_priv *hal_priv = (struct tiny_alsa_priv *)mgr->priv;
	struct pcm *pcm = hal_priv->pcm;

	ret = pcm_start(pcm);
	if (ret != 0)
		print_err("device %s start fail(%d)\n", hal_priv->name, ret);

	return ret;
}

static int tiny_alsa_pcm_stop(struct pcm_hal_mgr *mgr)
{
	int ret;
	struct tiny_alsa_priv *hal_priv = (struct tiny_alsa_priv *)mgr->priv;
	struct pcm *pcm = hal_priv->pcm;

	ret = pcm_stop(pcm);
	if (ret != 0)
		print_err("device %s stop fail(%d)\n", hal_priv->name, ret);

	return ret;
}

static int tiny_alsa_pcm_read(struct pcm_hal_mgr *mgr, void *buffer, int frames, int64_t *tstamp)
{
	int ret;
	struct tiny_alsa_priv *hal_priv = (struct tiny_alsa_priv *)mgr->priv;
	struct pcm *pcm = hal_priv->pcm;
	struct timespec timeval;
	unsigned int avail;
	int read_size;

	read_size = frames * hal_priv->chnum * hal_priv->bitdepth / 8;

	if (tstamp != NULL) {
		ret = pcm_get_htimestamp(pcm, &avail, &timeval);
		if (ret != 0 && errno == EPIPE) {
			pcm_stop(pcm);
			pcm_start(pcm);
			print_err("%s(), read error(%d, %d), restart pcm!", __FUNCTION__, ret, errno);
			return -EPIPE;
		} else if (ret != 0 && errno != EPIPE) {
			print_err("%s(), read error(%d, %d), break pcm read!", __FUNCTION__, ret, errno);
			return ret;
		}
		*tstamp = (int64_t)(timeval.tv_sec) * TSTAMP_PER_SECOND + (int64_t)(timeval.tv_nsec) - (int64_t)avail * (int64_t)(hal_priv->ns_per_frame);
	}
	ret = pcm_read(pcm, buffer, read_size);
	if (ret != 0)
		return ret;

	return 0;
}

int pcm_hal_tiny_alsa_create(struct pcm_hal_mgr *mgr)
{
	struct tiny_alsa_priv *hal_priv;

	hal_priv = (struct tiny_alsa_priv *)calloc(1, sizeof(struct tiny_alsa_priv));
	if (hal_priv == NULL)
		return -ENOMEM;

	mgr->type = PCM_HW_TINY_ALSA;
	mgr->pcm_open = tiny_alsa_pcm_open;
	mgr->pcm_start = tiny_alsa_pcm_start;
	mgr->pcm_stop = tiny_alsa_pcm_stop;
	mgr->pcm_close = tiny_alsa_pcm_close;
	mgr->pcm_read = tiny_alsa_pcm_read;
	hal_priv->pcm = NULL;
	hal_priv->chnum = 0;
	hal_priv->bitdepth = 0;
	hal_priv->rate = 0;
	hal_priv->period_size = 0;
	hal_priv->buffer_size = 0;

	mgr->priv = (void *)(hal_priv);

	return 0;
}

int pcm_hal_tiny_alsa_destroy(struct pcm_hal_mgr *mgr)
{
	struct tiny_alsa_priv *hal_priv = (struct tiny_alsa_priv *)(mgr->priv);

	mgr->pcm_open = NULL;
	mgr->pcm_start = NULL;
	mgr->pcm_stop = NULL;
	mgr->pcm_close = NULL;
	mgr->pcm_read = NULL;
	mgr->priv = NULL;
	free(hal_priv);

	return 0;
}
#else
int pcm_hal_tiny_alsa_create(struct pcm_hal_mgr *mgr)
{
	return -EPERM;
}

int pcm_hal_tiny_alsa_destroy(struct pcm_hal_mgr *mgr)
{
	return -EPERM;
}
#endif
