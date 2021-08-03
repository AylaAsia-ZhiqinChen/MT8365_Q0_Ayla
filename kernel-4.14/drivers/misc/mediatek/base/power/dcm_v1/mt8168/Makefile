# SPDX-License-Identifier: GPL-2.0
#
# Copyright (c) 2019 MediaTek Inc.
#

ccflags-y += -I$(srctree)/drivers/misc/mediatek/base/power/include/dcm_v1/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/base/power/include/dcm_v1/$(MTK_PLATFORM)/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/base/power/$(MTK_PLATFORM)/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/dramc/$(MTK_PLATFORM)/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/base/power/include/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(CONFIG_MTK_PLATFORM)/include/mach

obj-y += mtk_dcm_autogen.o
obj-y += mtk_dcm_internal.o

