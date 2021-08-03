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

obj-y += adsp_mem.o
obj-y += adsp_plat.o
obj-y += adsp_clk.o
obj-y += adsp_debug.o
obj-y += adsp_sema.o
obj-y += adsp_ipi.o
obj-y += adsp_ipi_platform.o
obj-y += adsp_wdt.o

ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(CONFIG_MTK_PLATFORM)/include/mach
ccflags-y += -I$(srctree)/drivers/misc/mediatek/hifi_dsp/$(CONFIG_MTK_PLATFORM)
