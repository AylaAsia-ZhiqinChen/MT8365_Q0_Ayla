#
# Copyright (c) 2016, ARM Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

ifneq (${ARCH}, aarch32)
	$(error SP_MIN is only supported on AArch32 platforms)
endif

include lib/psci/psci_lib.mk

INCLUDES		+=	-Iinclude/bl32/sp_min

BL32_SOURCES		+=	bl32/sp_min/sp_min_main.c		\
				bl32/sp_min/aarch32/entrypoint.S	\
				common/runtime_svc.c			\
				services/std_svc/std_svc_setup.c	\
				${PSCI_LIB_SOURCES}

ifeq (${ENABLE_PMF}, 1)
BL32_SOURCES		+=	lib/pmf/pmf_main.c
endif

BL32_LINKERFILE	:=	bl32/sp_min/sp_min.ld.S

# Include the platform-specific SP_MIN Makefile
# If no platform-specific SP_MIN Makefile exists, it means SP_MIN is not supported
# on this platform.
SP_MIN_PLAT_MAKEFILE := $(wildcard ${PLAT_DIR}/sp_min/sp_min-${PLAT}.mk)
ifeq (,${SP_MIN_PLAT_MAKEFILE})
  $(error SP_MIN is not supported on platform ${PLAT})
else
  include ${SP_MIN_PLAT_MAKEFILE}
endif

RESET_TO_SP_MIN	:= 0
$(eval $(call add_define,RESET_TO_SP_MIN))
$(eval $(call assert_boolean,RESET_TO_SP_MIN))
