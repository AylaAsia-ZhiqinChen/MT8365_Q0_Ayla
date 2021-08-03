# SPDX-License-Identifier: GPL-2.0
#
# Copyright (c) 2019 MediaTek Inc.
#

THERMAL_CHIP_DRIVER_DIR := $(srctree)/drivers/misc/mediatek/thermal/$(MTK_PLATFORM)
ccflags-y  += -I$(THERMAL_CHIP_DRIVER_DIR)/inc
ccflags-y  += -I$(srctree)/drivers/misc/mediatek/base/power/$(MTK_PLATFORM)
ccflags-y  += -I$(srctree)/drivers/misc/mediatek/gpu/hal/
ccflags-y  += -I$(srctree)/drivers/misc/mediatek/thermal/fakeHeader/
ccflags-y  += -I$(srctree)/drivers/misc/mediatek/pmic/include/
ccflags-y  += -I$(srctree)/drivers/misc/mediatek/thermal/common/inc/


obj-$(CONFIG_THERMAL) += mtk_tc.o
obj-$(CONFIG_THERMAL) += mtk_ts_bts.o
obj-$(CONFIG_THERMAL) += mtk_pmic_efuse.o
