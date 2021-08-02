#
# MediaTek Inc. (C) 2019. All rights reserved.
#
# Copyright Statement:
#
# This software/firmware and related documentation ("MediaTek Software") are
# protected under relevant copyright laws. The information contained herein is
# confidential and proprietary to MediaTek Inc. and/or its licensors. Without
# the prior written permission of MediaTek inc. and/or its licensors, any
# reproduction, modification, use or disclosure of MediaTek Software, and
# information contained herein, in whole or in part, shall be strictly
# prohibited.
#
# BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
# THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
# RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
# ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
# WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
# NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
# RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
# INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
# TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
# RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
# OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
# SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
# RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
# STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
# ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
# RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
# MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
# CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
#
# The following software/firmware and/or related documentation ("MediaTek
# Software") have been modified by MediaTek Inc. All revisions are subject to
# any receiver's applicable license agreements with MediaTek Inc.
#

# Nandx DA Makefile
ifeq ($(CONFIG_NANDX_MK_DA), y)
NANDX_PATH := nandx
NANDX_OBJ := obj-y
NANDX_OS := da
NANDX_PREFIX :=
NANDX_POSTFIX := %.o
endif

# Nandx Preloader Makefile
ifeq ($(CONFIG_NANDX_MK_PRELOADER), y)
NANDX_PATH := nandx
NANDX_OBJ := MOD_SRC
NANDX_OS := preloader
NANDX_PREFIX :=
NANDX_POSTFIX := %.c
endif

# Nandx LK Makefile
ifeq ($(CONFIG_NANDX_MK_LK), y)
NANDX_OBJ := OBJS
NANDX_OS := lk
NANDX_PREFIX := $(LOCAL_DIR)/
NANDX_POSTFIX := %.o
NANDX_PATH := $(NANDX_PREFIX)nandx
endif

# Nandx Kernel Makefile
ifeq ($(CONFIG_NANDX_MK_KERNEL), y)
NANDX_PATH := $(srctree)/drivers/misc/mediatek/nand/nandx
ccflags-y += -I$(NANDX_PATH)/include/kernel
ccflags-y += -I$(NANDX_PATH)/include/internal
ccflags-y += -I$(NANDX_PATH)/driver/kernel
NANDX_OBJ := obj-y
NANDX_OS := kernel
NANDX_PREFIX :=
NANDX_POSTFIX := %.o
endif

NANDX_CORE := $(NANDX_PREFIX)nandx/core
NANDX_DRIVER := $(NANDX_PREFIX)nandx/driver
NANDX_CUSTOM := $(NANDX_PREFIX)nandx/driver/$(NANDX_OS)

NANDX_SRC :=
include $(NANDX_PATH)/core/Nandx.mk
$(NANDX_OBJ) += $(patsubst %.c, $(NANDX_CORE)/$(NANDX_POSTFIX), $(NANDX_SRC))

NANDX_SRC :=
include $(NANDX_PATH)/driver/Nandx.mk
$(NANDX_OBJ) += $(patsubst %.c, $(NANDX_DRIVER)/$(NANDX_POSTFIX), $(NANDX_SRC))

NANDX_SRC :=
include $(NANDX_PATH)/driver/$(NANDX_OS)/Nandx.mk
$(NANDX_OBJ) += $(patsubst %.c, $(NANDX_CUSTOM)/$(NANDX_POSTFIX), $(NANDX_SRC))
