/*
 * mt8168-ep.c  --  MT8168 machine driver
 *
 * Copyright (c) 2018 MediaTek Inc.
 * Author: Jia Zeng <jia.zeng@mediatek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/module.h>
#include <linux/of_gpio.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>
#include "mt8168-afe-common.h"
#ifdef CONFIG_MTK_HIFIXDSP_SUPPORT
#include "mt8168-adsp-utils.h"
#endif

#define PREFIX	"mediatek,"

#define TEST_BACKEND_WITH_ENDPOINT

enum PINCTRL_PIN_STATE {
	PIN_STATE_DEFAULT = 0,
	PIN_STATE_EXTAMP_ON,
	PIN_STATE_EXTAMP_OFF,
	PIN_STATE_MAX
};

static const char * const mt8168_evb_pin_str[PIN_STATE_MAX] = {
	"default",
	"extamp_on",
	"extamp_off",
};

struct mt8168_evb_dmic_ctrl_data {
	unsigned int fix_rate;
	unsigned int fix_channels;
	unsigned int fix_bit_width;
};

struct mt8168_evb_tdm_ctrl_data {
	unsigned int tdmin_fix_rate;
	unsigned int tdmin_fix_bit_width;
	unsigned int tdmin_fix_channels;
};

struct mt8168_evb_priv {
	struct pinctrl *pinctrl;
	struct pinctrl_state *pin_states[PIN_STATE_MAX];
	struct mt8168_evb_dmic_ctrl_data dmic_data;
	struct mt8168_evb_tdm_ctrl_data tdm_data;
};

struct mt8168_dai_link_prop {
	char *name;
	unsigned int link_id;
};

static const struct snd_soc_dapm_widget mt8168_evb_widgets[] = {
	SND_SOC_DAPM_OUTPUT("PCM1 Out"),
	SND_SOC_DAPM_INPUT("PCM1 In"),
	SND_SOC_DAPM_INPUT("DMIC In"),
	SND_SOC_DAPM_OUTPUT("GASRC1 Out"),
	SND_SOC_DAPM_INPUT("GASRC1 In"),
	SND_SOC_DAPM_OUTPUT("GASRC2 Out"),
	SND_SOC_DAPM_INPUT("GASRC2 In"),
	SND_SOC_DAPM_OUTPUT("TDM_ASRC Out"),
	SND_SOC_DAPM_INPUT("TDM_ASRC In"),
#ifdef TEST_BACKEND_WITH_ENDPOINT
	SND_SOC_DAPM_OUTPUT("TDM_OUT Out"),
	SND_SOC_DAPM_INPUT("TDM_IN In"),
#endif
};

static const struct snd_soc_dapm_route mt8168_evb_routes[] = {
	{"DMIC Capture", NULL, "DMIC In"},
	{"PCM1 Out", NULL, "PCM1 Playback"},
	{"PCM1 Capture", NULL, "PCM1 In"},
	{"GASRC1 Out", NULL, "GASRC1 Playback"},
	{"GASRC1 Capture", NULL, "GASRC1 In"},
	{"GASRC2 Out", NULL, "GASRC2 Playback"},
	{"GASRC2 Capture", NULL, "GASRC2 In"},
	{"TDM_ASRC Out", NULL, "TDM_ASRC Playback"},
	{"TDM_ASRC Capture", NULL, "TDM_ASRC In"},
#ifdef TEST_BACKEND_WITH_ENDPOINT
	{"TDM_OUT Out", NULL, "TDM_OUT Playback"},
	{"TDM_IN Capture", NULL, "TDM_IN In"},
#endif

};

enum {
	/* FE */
	DAI_LINK_DL1_PLAYBACK = 0,
	DAI_LINK_DL2_PLAYBACK,
	DAI_LINK_AWB_CAPTURE,
	DAI_LINK_VUL_CAPTURE,
	DAI_LINK_VUL2_CAPTURE,
	DAI_LINK_VUL3_CAPTURE,
	DAI_LINK_TDM_OUT,
	DAI_LINK_TDM_IN,
#ifdef CONFIG_SND_SOC_MTK_BTCVSD
	DAI_LINK_BTCVSD,
#endif
#ifdef CONFIG_MTK_HIFIXDSP_SUPPORT
	DAI_LINK_FE_HOSTLESS_VA,
	DAI_LINK_FE_VA,
	DAI_LINK_FE_MIC_RECORD,
#endif
	/* BE */
	DAI_LINK_I2S_INTF,
	DAI_LINK_2ND_I2S_INTF,
	DAI_LINK_PCM1_INTF,
	DAI_LINK_VIRTUAL_DL_SOURCE,
	DAI_LINK_VIRTUAL_TDM_OUT_SOURCE,
	DAI_LINK_DMIC,
	DAI_LINK_INT_ADDA,
	DAI_LINK_TDM_OUT_IO,
	DAI_LINK_TDM_IN_IO,
	DAI_LINK_GASRC1,
	DAI_LINK_GASRC2,
	DAI_LINK_TDM_ASRC,
#ifdef CONFIG_MTK_HIFIXDSP_SUPPORT
	DAI_LINK_BE_VUL2_IN,
	DAI_LINK_BE_TDM_IN,
#endif
	DAI_LINK_NUM
};

#ifdef CONFIG_MTK_HIFIXDSP_SUPPORT
static const unsigned int adsp_dai_links[] = {
	DAI_LINK_FE_HOSTLESS_VA,
	DAI_LINK_FE_VA,
	DAI_LINK_FE_MIC_RECORD,
	DAI_LINK_BE_VUL2_IN,
	DAI_LINK_BE_TDM_IN
};

static bool is_adsp_dai_link(unsigned int dai)
{
	size_t i;

	for (i = 0; i < ARRAY_SIZE(adsp_dai_links); i++)
		if (dai == adsp_dai_links[i])
			return true;

	return false;
}
#endif

static int mt8168_be_hw_params_fixup(struct snd_soc_pcm_runtime *rtd,
	struct snd_pcm_hw_params *params)
{
	struct mt8168_evb_priv *priv = snd_soc_card_get_drvdata(rtd->card);
	int id = rtd->dai_link->id;
	struct mt8168_evb_tdm_ctrl_data *tdm;
	struct mt8168_evb_dmic_ctrl_data *dmic;
	unsigned int fix_rate = 0;
	unsigned int fix_bit_width = 0;
	unsigned int fix_channels = 0;

	if (id == DAI_LINK_DMIC) {
		dmic = &priv->dmic_data;
		fix_rate = dmic->fix_rate;
		fix_bit_width = dmic->fix_bit_width;
		fix_channels = dmic->fix_channels;
	} else if (id == DAI_LINK_TDM_IN_IO) {
		tdm = &priv->tdm_data;
		fix_rate = tdm->tdmin_fix_rate;
		fix_bit_width = tdm->tdmin_fix_bit_width;
		fix_channels = tdm->tdmin_fix_channels;
	}

	if (fix_rate > 0) {
		struct snd_interval *rate =
			hw_param_interval(params, SNDRV_PCM_HW_PARAM_RATE);

		rate->max = rate->min = fix_rate;
	}

	if (fix_bit_width > 0) {
		struct snd_mask *mask =
			hw_param_mask(params, SNDRV_PCM_HW_PARAM_FORMAT);

		if (fix_bit_width == 32) {
			snd_mask_none(mask);
			snd_mask_set(mask, SNDRV_PCM_FORMAT_S32_LE);
		} else if (fix_bit_width == 16) {
			snd_mask_none(mask);
			snd_mask_set(mask, SNDRV_PCM_FORMAT_S16_LE);
		}
	}

	if (fix_channels > 0) {
		struct snd_interval *channels = hw_param_interval(params,
			SNDRV_PCM_HW_PARAM_CHANNELS);

		channels->min = channels->max = fix_channels;
	}

	return 0;
}

/* Digital audio interface glue - connects codec <---> CPU */
static struct snd_soc_dai_link mt8168_evb_dais[] = {
	/* Front End DAI links */
	[DAI_LINK_DL1_PLAYBACK] = {
		.name = "DL1_FE",
		.stream_name = "MultiMedia1_PLayback",
		.cpu_dai_name = "DL1",
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.id = DAI_LINK_DL1_PLAYBACK,
		.trigger = {
			SND_SOC_DPCM_TRIGGER_POST,
			SND_SOC_DPCM_TRIGGER_POST
		},
		.dynamic = 1,
		.dpcm_playback = 1,
	},
	[DAI_LINK_DL2_PLAYBACK] = {
		.name = "DL2_FE",
		.stream_name = "MultiMedia2_PLayback",
		.cpu_dai_name = "DL2",
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.id = DAI_LINK_DL2_PLAYBACK,
		.trigger = {
			SND_SOC_DPCM_TRIGGER_POST,
			SND_SOC_DPCM_TRIGGER_POST
		},
		.dynamic = 1,
		.dpcm_playback = 1,
	},
	[DAI_LINK_AWB_CAPTURE] = {
		.name = "AWB_FE",
		.stream_name = "DL1_AWB_Record",
		.cpu_dai_name = "AWB",
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.id = DAI_LINK_AWB_CAPTURE,
		.trigger = {
			SND_SOC_DPCM_TRIGGER_POST,
			SND_SOC_DPCM_TRIGGER_POST
		},
		.dynamic = 1,
		.dpcm_capture = 1,
	},
	[DAI_LINK_VUL_CAPTURE] = {
		.name = "VUL_FE",
		.stream_name = "MultiMedia1_Capture",
		.cpu_dai_name = "VUL",
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.id = DAI_LINK_VUL_CAPTURE,
		.trigger = {
			SND_SOC_DPCM_TRIGGER_POST,
			SND_SOC_DPCM_TRIGGER_POST
		},
		.dynamic = 1,
		.dpcm_capture = 1,
	},
	[DAI_LINK_VUL2_CAPTURE] = {
		.name = "VUL2_FE",
		.stream_name = "VUL2 Capture",
		.cpu_dai_name = "VUL2",
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.id = DAI_LINK_VUL2_CAPTURE,
		.trigger = {
			SND_SOC_DPCM_TRIGGER_POST,
			SND_SOC_DPCM_TRIGGER_POST
		},
		.dynamic = 1,
		.dpcm_capture = 1,
	},
	[DAI_LINK_VUL3_CAPTURE] = {
		.name = "VUL3_FE",
		.stream_name = "VUL3 Capture",
		.cpu_dai_name = "VUL3",
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.id = DAI_LINK_VUL3_CAPTURE,
		.trigger = {
			SND_SOC_DPCM_TRIGGER_POST,
			SND_SOC_DPCM_TRIGGER_POST
		},
		.dynamic = 1,
		.dpcm_capture = 1,
	},
	[DAI_LINK_TDM_OUT] = {
		.name = "TDM_OUT_FE",
		.stream_name = "TDM_Playback",
		.cpu_dai_name = "TDM_OUT",
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.id = DAI_LINK_TDM_OUT,
		.trigger = {
			SND_SOC_DPCM_TRIGGER_POST,
			SND_SOC_DPCM_TRIGGER_POST
		},
		.dynamic = 1,
		.dpcm_playback = 1,
	},
	[DAI_LINK_TDM_IN] = {
		.name = "TDM_IN_FE",
		.stream_name = "TDM_Capture",
		.cpu_dai_name = "TDM_IN",
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.id = DAI_LINK_TDM_IN,
		.trigger = {
			SND_SOC_DPCM_TRIGGER_POST,
			SND_SOC_DPCM_TRIGGER_POST
		},
		.dynamic = 1,
		.dpcm_capture = 1,
	},
#ifdef CONFIG_SND_SOC_MTK_BTCVSD
	[DAI_LINK_BTCVSD] = {
		.name = "BTCVSD",
		.stream_name = "BTCVSD",
		.cpu_dai_name = "snd-soc-dummy-dai",
		.platform_name = "18050000.mtk-btcvsd-snd",
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
	},
#endif
#ifdef CONFIG_MTK_HIFIXDSP_SUPPORT
	[DAI_LINK_FE_HOSTLESS_VA] = {
		.name = "ADSP HOSTLESS_VA",
		.stream_name = "HOSTLESS_VA",
		.cpu_dai_name = "FE_HOSTLESS_VA",
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.trigger = {
			SND_SOC_DPCM_TRIGGER_POST,
			SND_SOC_DPCM_TRIGGER_POST
		},
		.dynamic = 1,
		.dpcm_capture = 1,
	},
	[DAI_LINK_FE_VA] = {
		.name = "ADSP VA_FE",
		.stream_name = "VA Capture",
		.cpu_dai_name = "FE_VA",
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.trigger = {
			SND_SOC_DPCM_TRIGGER_POST,
			SND_SOC_DPCM_TRIGGER_POST
		},
		.dynamic = 1,
		.dpcm_capture = 1,
	},
	[DAI_LINK_FE_MIC_RECORD] = {
		.name = "ADSP MIC_RECORD",
		.stream_name = "MIC_RECORD",
		.cpu_dai_name = "FE_MICR",
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.trigger = {
			SND_SOC_DPCM_TRIGGER_POST,
			SND_SOC_DPCM_TRIGGER_POST
		},
		.dynamic = 1,
		.dpcm_capture = 1,
	},
#endif
	/* Back End DAI links */
	[DAI_LINK_I2S_INTF] = {
		.name = "I2S BE",
		.cpu_dai_name = "I2S",
		.no_pcm = 1,
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.id = DAI_LINK_I2S_INTF,
		.dai_fmt = SND_SOC_DAIFMT_I2S |
				SND_SOC_DAIFMT_NB_NF |
				SND_SOC_DAIFMT_CBS_CFS,
		.dpcm_playback = 1,
		.dpcm_capture = 1,
	},
	[DAI_LINK_2ND_I2S_INTF] = {
		.name = "2ND_I2S BE",
		.cpu_dai_name = "2ND I2S",
		.no_pcm = 1,
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.id = DAI_LINK_2ND_I2S_INTF,
		.dai_fmt = SND_SOC_DAIFMT_I2S |
				SND_SOC_DAIFMT_NB_NF |
				SND_SOC_DAIFMT_CBS_CFS,
		.dpcm_playback = 1,
		.dpcm_capture = 1,
	},
	[DAI_LINK_PCM1_INTF] = {
		.name = "PCM1 BE",
		.cpu_dai_name = "PCM1",
		.no_pcm = 1,
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.id = DAI_LINK_PCM1_INTF,
		.dai_fmt = SND_SOC_DAIFMT_I2S |
			   SND_SOC_DAIFMT_NB_NF |
			   SND_SOC_DAIFMT_CBS_CFS,
		.dpcm_playback = 1,
		.dpcm_capture = 1,
	},
	[DAI_LINK_VIRTUAL_DL_SOURCE] = {
		.name = "VIRTUAL_DL_SRC BE",
		.cpu_dai_name = "VIRTUAL_DL_SRC",
		.no_pcm = 1,
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.id = DAI_LINK_VIRTUAL_DL_SOURCE,
		.dpcm_capture = 1,
	},
	[DAI_LINK_VIRTUAL_TDM_OUT_SOURCE] = {
		.name = "VIRTUAL_TDM_OUT_SRC BE",
		.cpu_dai_name = "VIRTUAL_TDM_OUT_SRC",
		.no_pcm = 1,
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.id = DAI_LINK_VIRTUAL_TDM_OUT_SOURCE,
		.dpcm_capture = 1,
	},
	[DAI_LINK_DMIC] = {
		.name = "DMIC BE",
		.cpu_dai_name = "DMIC",
		.no_pcm = 1,
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.id = DAI_LINK_DMIC,
		.dpcm_capture = 1,
	},
	[DAI_LINK_INT_ADDA] = {
		.name = "MTK Codec",
		.cpu_dai_name = "INT ADDA",
		.no_pcm = 1,
		.codec_name = "mt-soc-codec",
		.codec_dai_name = "mt6357-codec-dai",
		.id = DAI_LINK_INT_ADDA,
		.dpcm_playback = 1,
		.dpcm_capture = 1,
	},
	[DAI_LINK_TDM_OUT_IO] = {
		.name = "TDM_OUT BE",
		.cpu_dai_name = "TDM_OUT_IO",
		.no_pcm = 1,
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.id = DAI_LINK_TDM_OUT_IO,
		.dai_fmt = SND_SOC_DAIFMT_I2S |
				SND_SOC_DAIFMT_NB_NF |
				SND_SOC_DAIFMT_CBS_CFS,
		.dpcm_playback = 1,
	},
	[DAI_LINK_TDM_IN_IO] = {
		.name = "TDM_IN BE",
		.cpu_dai_name = "TDM_IN_IO",
		.no_pcm = 1,
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.id = DAI_LINK_TDM_IN_IO,
		.dai_fmt = SND_SOC_DAIFMT_I2S |
				SND_SOC_DAIFMT_IB_IF |
				SND_SOC_DAIFMT_CBS_CFS,
		.dpcm_capture = 1,
	},
	[DAI_LINK_GASRC1] = {
		.name = "GASRC1 BE",
		.cpu_dai_name = "GASRC1",
		.no_pcm = 1,
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.id = DAI_LINK_GASRC1,
		.dpcm_playback = 1,
		.dpcm_capture = 1,
	},
	[DAI_LINK_GASRC2] = {
		.name = "GASRC2 BE",
		.cpu_dai_name = "GASRC2",
		.no_pcm = 1,
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.id = DAI_LINK_GASRC2,
		.dpcm_playback = 1,
		.dpcm_capture = 1,
	},
	[DAI_LINK_TDM_ASRC] = {
		.name = "TDM_ASRC BE",
		.cpu_dai_name = "TDM_ASRC",
		.no_pcm = 1,
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.id = DAI_LINK_TDM_ASRC,
		.dpcm_playback = 1,
		.dpcm_capture = 1,
	},
#ifdef CONFIG_MTK_HIFIXDSP_SUPPORT
	[DAI_LINK_BE_VUL2_IN] = {
		.name = "ADSP_VUL2_IN BE",
		.cpu_dai_name = "BE_VUL2_IN",
		.no_pcm = 1,
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.dpcm_playback = 0,
		.dpcm_capture = 1,
	},
	[DAI_LINK_BE_TDM_IN] = {
		.name = "ADSP_TDM_IN BE",
		.cpu_dai_name = "BE_TDM_IN",
		.no_pcm = 1,
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.dpcm_playback = 0,
		.dpcm_capture = 1,
	},
#endif
};

static int mt8168_evb_gpio_probe(struct snd_soc_card *card)
{
	struct mt8168_evb_priv *priv = snd_soc_card_get_drvdata(card);
	int ret = 0;
	int i;

	priv->pinctrl = devm_pinctrl_get(card->dev);
	if (IS_ERR(priv->pinctrl)) {
		ret = PTR_ERR(priv->pinctrl);
		dev_err(card->dev, "%s devm_pinctrl_get failed %d\n",
			__func__, ret);
		return ret;
	}

	for (i = 0 ; i < PIN_STATE_MAX ; i++) {
		priv->pin_states[i] = pinctrl_lookup_state(priv->pinctrl,
			mt8168_evb_pin_str[i]);
		if (IS_ERR(priv->pin_states[i])) {
			ret = PTR_ERR(priv->pin_states[i]);
			dev_info(card->dev, "%s Can't find pin state %s %d\n",
				 __func__, mt8168_evb_pin_str[i], ret);
		}
	}

	if (IS_ERR(priv->pin_states[PIN_STATE_DEFAULT]))
		return 0;

	/* default state */
	ret = pinctrl_select_state(priv->pinctrl,
				   priv->pin_states[PIN_STATE_DEFAULT]);
	if (ret)
		dev_err(card->dev, "%s failed to select state %d\n",
			__func__, ret);

	return ret;
}

static int link_to_dai(int link_id)
{
	switch (link_id) {
	case DAI_LINK_TDM_IN_IO:
		return MT8168_AFE_IO_TDM_IN;
	case DAI_LINK_DMIC:
		return MT8168_AFE_IO_DMIC;
	default:
		break;
	}
	return -1;
}

static void mt8168_evb_parse_of(struct snd_soc_card *card,
	struct device_node *np)
{
	struct mt8168_evb_priv *priv = snd_soc_card_get_drvdata(card);
	size_t i;
	int ret;
	char prop[128];
	unsigned int val;

	static const struct mt8168_dai_link_prop of_dai_links_io[] = {
		{ "dmic",	DAI_LINK_DMIC },
		{ "tdmin",	DAI_LINK_TDM_IN_IO },
	};

	for (i = 0; i < ARRAY_SIZE(of_dai_links_io); i++) {
		unsigned int link_id = of_dai_links_io[i].link_id;
		struct snd_soc_dai_link *dai_link = &mt8168_evb_dais[link_id];
		struct mt8168_evb_dmic_ctrl_data *dmic;
		struct mt8168_evb_tdm_ctrl_data *tdm;

		/* parse fix rate */
		snprintf(prop, sizeof(prop), PREFIX"%s-fix-rate",
			 of_dai_links_io[i].name);
		ret = of_property_read_u32(np, prop, &val);

		if (ret == 0 && mt8168_afe_rate_supported(val,
			link_to_dai(link_id))) {
			switch (link_id) {
			case DAI_LINK_DMIC:
				dmic = &priv->dmic_data;
				dmic->fix_rate = val;
				break;
			case DAI_LINK_TDM_IN_IO:
				tdm = &priv->tdm_data;
				tdm->tdmin_fix_rate = val;
				break;
			default:
				break;
			}

			dai_link->be_hw_params_fixup =
				mt8168_be_hw_params_fixup;
		}

		/* parse fix bit width */
		snprintf(prop, sizeof(prop), PREFIX"%s-fix-bit-width",
			 of_dai_links_io[i].name);
		ret = of_property_read_u32(np, prop, &val);
		if (ret == 0 && (val == 32 || val == 16)) {
			switch (link_id) {
			case DAI_LINK_DMIC:
				dmic = &priv->dmic_data;
				dmic->fix_bit_width = val;
				break;
			case DAI_LINK_TDM_IN_IO:
				tdm = &priv->tdm_data;
				tdm->tdmin_fix_bit_width = val;
				break;
			default:
				break;
			}

			dai_link->be_hw_params_fixup =
				mt8168_be_hw_params_fixup;
		}

		/* parse fix channels */
		snprintf(prop, sizeof(prop), PREFIX"%s-fix-channels",
			 of_dai_links_io[i].name);
		ret = of_property_read_u32(np, prop, &val);

		if (ret == 0 && mt8168_afe_channel_supported(val,
			link_to_dai(link_id))) {
			switch (link_id) {
			case DAI_LINK_DMIC:
				dmic = &priv->dmic_data;
				dmic->fix_channels = val;
				break;
			case DAI_LINK_TDM_IN_IO:
				tdm = &priv->tdm_data;
				tdm->tdmin_fix_channels = val;
				break;
			default:
				break;
			}

			dai_link->be_hw_params_fixup =
				mt8168_be_hw_params_fixup;
		}
	}

}

static struct snd_soc_card mt8168_evb_card = {
	.name = "mt-snd-card",
	.owner = THIS_MODULE,
	.dai_link = mt8168_evb_dais,
	.num_links = ARRAY_SIZE(mt8168_evb_dais),
	.dapm_widgets = mt8168_evb_widgets,
	.num_dapm_widgets = ARRAY_SIZE(mt8168_evb_widgets),
	.dapm_routes = mt8168_evb_routes,
	.num_dapm_routes = ARRAY_SIZE(mt8168_evb_routes),
};

static int mt8168_evb_dev_probe(struct platform_device *pdev)
{
	struct snd_soc_card *card = &mt8168_evb_card;
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct device_node *platform_node;
	struct mt8168_evb_priv *priv;
	int i, ret;

	platform_node = of_parse_phandle(dev->of_node, "mediatek,platform", 0);
	if (!platform_node) {
		dev_err(dev, "Property 'platform' missing or invalid\n");
		return -EINVAL;
	}

	for (i = 0; i < card->num_links; i++) {
#ifdef CONFIG_MTK_HIFIXDSP_SUPPORT
		if (is_adsp_dai_link(i))
			continue;
#endif
		if (mt8168_evb_dais[i].platform_name)
			continue;
		mt8168_evb_dais[i].platform_of_node = platform_node;
	}

#ifdef CONFIG_MTK_HIFIXDSP_SUPPORT
	platform_node = of_parse_phandle(dev->of_node,
					 "mediatek,adsp-platform", 0);
	if (!platform_node) {
		dev_err(dev, "Property 'adsp-platform' missing or invalid\n");
		return -EINVAL;
	}

	for (i = 0; i < ARRAY_SIZE(adsp_dai_links); i++) {
		unsigned int dai = adsp_dai_links[i];

		if (mt8168_evb_dais[dai].platform_name)
			continue;
		mt8168_evb_dais[dai].platform_of_node = platform_node;
	}
#endif

	card->dev = dev;

	priv = devm_kzalloc(dev, sizeof(struct mt8168_evb_priv),
			    GFP_KERNEL);
	if (!priv) {
		ret = -ENOMEM;
		dev_err(dev, "%s allocate card private data fail %d\n",
			__func__, ret);
		return ret;
	}

	snd_soc_card_set_drvdata(card, priv);

	mt8168_evb_gpio_probe(card);

	mt8168_evb_parse_of(card, np);

	ret = devm_snd_soc_register_card(dev, card);
	if (ret)
		dev_err(dev, "%s snd_soc_register_card fail %d\n",
			__func__, ret);

	return ret;
}

static const struct of_device_id mt8168_evb_dt_match[] = {
	{ .compatible = "mediatek,mt8168-ep", },
	{ }
};
MODULE_DEVICE_TABLE(of, mt8168_evb_dt_match);

static struct platform_driver mt8168_evb_driver = {
	.driver = {
		   .name = "mt8168-ep",
		   .of_match_table = mt8168_evb_dt_match,
#ifdef CONFIG_PM
		   .pm = &snd_soc_pm_ops,
#endif
	},
	.probe = mt8168_evb_dev_probe,
};

module_platform_driver(mt8168_evb_driver);

/* Module information */
MODULE_DESCRIPTION("MT8168 EP SoC machine driver");
MODULE_AUTHOR("Jia Zeng <jia.zeng@mediatek.com>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:mt8168-ep");

