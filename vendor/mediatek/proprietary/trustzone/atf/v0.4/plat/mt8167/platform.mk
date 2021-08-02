#
# Copyright (c) 2013-2014, ARM Limited and Contributors. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# Neither the name of ARM nor the names of its contributors may be used
# to endorse or promote products derived from this software without specific
# prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

# On MTK_platform, the TSP can execute either from Trusted SRAM or Trusted DRAM.
# Trusted SRAM is the default.
TSP_RAM_LOCATION	:=	tsram

ifeq (${TSP_RAM_LOCATION}, tsram)
  TSP_RAM_LOCATION_ID := TSP_IN_TZRAM
else ifeq (${TSP_RAM_LOCATION}, tdram)
  TSP_RAM_LOCATION_ID := TSP_IN_TZDRAM
else
  $(error "Unsupported TSP_RAM_LOCATION value")
endif

# Process TSP_RAM_LOCATION_ID flag
$(eval $(call add_define,TSP_RAM_LOCATION_ID))

PLAT_INCLUDES		:=	-Iplat/mt8167/include/ -Iplat/mt8167/ -Iplat/mt8167/drivers/log  -Iplat/mt8167/drivers/timer/ -Iplat/mt8167/drivers/l2c/ 


PLAT_BL_COMMON_SOURCES	:=	drivers/io/io_fip.c				\
				drivers/io/io_memmap.c				\
				drivers/io/io_semihosting.c			\
				lib/mmio.c					\
				lib/aarch64/sysreg_helpers.S			\
				lib/aarch64/xlat_tables.c			\
				lib/semihosting/semihosting.c			\
				lib/semihosting/aarch64/semihosting_call.S	\
				plat/common/aarch64/plat_common.c		\
				plat/mt8167/plat_io_storage.c			\
				plat/common/fiq_smp_call.c

BL1_SOURCES		+=	drivers/arm/cci400/cci400.c			\
				plat/common/aarch64/platform_up_stack.S		\
				plat/mt8167/bl1_plat_setup.c			\
				plat/mt8167/aarch64/platform_common.c			\
				plat/mt8167/aarch64/plat_helpers.S

BL2_SOURCES		+=	drivers/arm/tzc400/tzc400.c			\
				plat/common/aarch64/platform_up_stack.S		\
				plat/mt8167/bl2_plat_setup.c			\
				plat/mt8167/plat_security.c				\
				plat/mt8167/aarch64/platform_common.c

BL31_SOURCES		+=	drivers/arm/gic/gic_v2.c			\
				drivers/arm/gic/gic_v3.c			\
				drivers/arm/gic/aarch64/gic_v3_sysregs.S	\
				drivers/arm/cci400/cci400.c			\
				plat/common/aarch64/platform_mp_stack.S		\
				plat/mt8167/bl31_plat_setup.c			\
				plat/mt8167/plat_gic.c				\
				plat/mt8167/plat_pm.c				\
				plat/mt8167/plat_topology.c				\
				plat/mt8167/scu.c				\
				plat/mt8167/power_tracer.c				\
				plat/mt8167/mailbox.c				\
				plat/mt8167/aarch64/plat_helpers.S			\
				plat/mt8167/aarch64/platform_common.c			\
				plat/mt8167/drivers/pwrc/plat_pwrc.c			\
				plat/mt8167/drivers/uart/uart.c			\
				plat/mt8167/drivers/timer/mt_cpuxgpt.c	\
				plat/mt8167/drivers/l2c/l2c.c

BL31_SOURCES		+=	plat/mt8167/sip_svc/sip_svc_common.c		\
				plat/mt8167/sip_svc/sip_svc_setup.c		\
				plat/mt8167/drivers/log/log.c

ifeq (${RESET_TO_BL31}, 1)
BL31_SOURCES		+=	drivers/arm/tzc400/tzc400.c			\
				plat/mt8167/plat_security.c
endif

ifeq (${SPD}, tbase)
BL31_SOURCES		+=	plat/mt8167/plat_tbase.c
endif

# Flag used by the MTK_platform port to determine the version of ARM GIC architecture
# to use for interrupt management in EL3.
MT_GIC_ARCH		:=	2
$(eval $(call add_define,MT_GIC_ARCH))
