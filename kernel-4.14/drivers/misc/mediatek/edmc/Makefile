#
# Copyright (C) 2018 MediaTek Inc.
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

#obj-y += $(subst ",,$(CONFIG_MTK_PLATFORM))/

ccflags-y += -I$(srctree)/drivers/misc/mediatek/edmc/$(MTK_PLATFORM)
ccflags-y += -I$(srctree)/drivers/misc/mediatek/edmc
ccflags-y += -I$(srctree)/drivers/misc/mediatek/vpu/vp6/mt8168
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include

obj-$(CONFIG_MTK_EDMC_SUPPORT) := edmc_main.o edmc_debug.o

subdir-ccflags-y += -DMTK_EDMC_CORE=$(MTK_EDMC_CORE)
subdir-ccflags-y += -DMTK_EDMC_DVT

# FPGA environment
ifeq ($(CONFIG_MTK_FPGA),y)
subdir-ccflags-y += -DMTK_EDMC_FPGA_PORTING
else ifeq ($(CONFIG_FPGA_EARLY_PORTING),y)
subdir-ccflags-y += -DMTK_EDMC_FPGA_PORTING
endif
