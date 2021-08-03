#
# Copyright (C) 2015 MediaTek Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#

snd-soc-mt8168-pcm-objs := \
    mt8168-afe-pcm.o mt8168-afe-utils.o mt8168-afe-controls.o mt8168-afe-debug.o

ifeq ($(CONFIG_MTK_HIFIXDSP_SUPPORT),y)
snd-soc-mt8168-adsp-pcm-objs := \
    mt8168-adsp-pcm.o mt8168-adsp-utils.o
endif

# MTK Platform Support
obj-$(CONFIG_SND_SOC_MT8168) += snd-soc-mt8168-pcm.o
obj-$(CONFIG_MTK_HIFIXDSP_SUPPORT) += snd-soc-mt8168-adsp-pcm.o

# Machine support
obj-$(CONFIG_SND_SOC_MT8168_FPGA) += mt8168-ep.o
obj-$(CONFIG_SND_SOC_MT8168_EVB) += mt8168-evb.o
obj-$(CONFIG_SND_SOC_MT8168_P1) += mt8168-p1.o
obj-$(CONFIG_SND_SOC_MT8133_M1) += mt8133-m1.o

ifeq ($(CONFIG_MTK_HIFIXDSP_SUPPORT),y)
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(MTK_PLATFORM)/include/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/hifidsp_audio_ipi/common/framework
ccflags-y += -I$(srctree)/drivers/misc/mediatek/base/power/mt8168
ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(MTK_PLATFORM)/include/mach/

ifeq ($(CONFIG_MTK_QOS_SUPPORT),y)
ccflags-y += -I$(srctree)/drivers/devfreq/
endif
endif
