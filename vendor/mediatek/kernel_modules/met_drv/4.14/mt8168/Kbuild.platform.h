# SPDX-License-Identifier: GPL-2.0
# Copyright (C) 2020 MediaTek Inc.
################################################################################
# Copyright (C) 2020 MediaTek Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
################################################################################

################################################################################
# Include Path
################################################################################
MET_VCOREDVFS_INC := $(srctree)/drivers/misc/mediatek/base/power/include/vcorefs_v3
MET_PTPOD_INC := $(srctree)/drivers/misc/mediatek/base/power/cpufreq_v1/src/mach/$(MTK_PLATFORM)/

################################################################################
# Feature Spec
# CPUPMU_VERSION: V8_0/V8_2
# EMI_SEDA_VERSION: SEDA2/SEDA3/SEDA3_5
# SPMTWAM_VERSION: ap/sspm
# SPMTWAM_IDLE_SIGNAL_SUPPORT: single/mutilple
################################################################################
CPUPMU_VERSION := V8_0
EMI_SEDA_VERSION := SEDA3
EMI_DRAMC_VERSION := V2

################################################################################
# Feature On/Off
################################################################################
FEATURE_WALLTIME = y
FEATURE_GPU := y
FEATURE_EMI := y
FEATURE_THERMAL := y

FEATURE_VCOREDVFS := n
FEATURE_ONDIEMET := n
FEATURE_SPMTWAM := n
FEATURE_BACKLIGHT := n
FEATURE_SSPM_EMI := n
FEATURE_PTPOD := n
FEATURE_ONDIEMET_WALLTIME := n
FEATURE_CPUDSU := n
FEATURE_SMI = n
