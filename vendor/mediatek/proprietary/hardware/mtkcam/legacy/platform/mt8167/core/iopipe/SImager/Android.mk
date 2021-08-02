# Copyright Statement:
#
# This software/firmware and related documentation ("MediaTek Software") are
# protected under relevant copyright laws. The information contained herein
# is confidential and proprietary to MediaTek Inc. and/or its licensors.
# Without the prior written permission of MediaTek inc. and/or its licensors,
# any reproduction, modification, use or disclosure of MediaTek Software,
# and information contained herein, in whole or in part, shall be strictly prohibited.

# MediaTek Inc. (C) 2010. All rights reserved.
#
# BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
# THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
# RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
# AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
# NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
# SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
# SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
# THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
# THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
# CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
# SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
# STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
# CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
# AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
# OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
# MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
#
# The following software/firmware and/or related documentation ("MediaTek Software")
# have been modified by MediaTek Inc. All revisions are subject to any receiver's
# applicable license agreements with MediaTek Inc.


################################################################################
#
# libcam.iopipe.simager
#
################################################################################

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#-----------------------------------------------------------
sinclude $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/mtkcam.mk

MTKCAM_USE_LEGACY_JPEGHW ?= '1'

LOCAL_SRC_FILES := \
    ImageTransform/IImageTransformBridge.cpp \
    ImageTransform/ImageTransform.cpp \
    JpegCodec/IJpegCodecBridge.cpp \
    JpegCodec/JpegCodec.cpp \
    SImager.cpp \
    ISImagerBridge.cpp \

#-----------------------------------------------------------
LOCAL_C_INCLUDES += $(MTKCAM_C_INCLUDES)
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/gralloc_extra/include
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/ext/include

LOCAL_C_INCLUDES +=$(TOP)/$(MTK_MTKCAM_PLATFORM)/include
LOCAL_C_INCLUDES +=$(TOP)/$(MTKCAM_C_INCLUDES)
LOCAL_C_INCLUDES +=$(TOP)/$(MTKCAM_C_INCLUDES)/..

LOCAL_C_INCLUDES += $(TOP)/$(MTK_MTKCAM_PLATFORM)/include/
LOCAL_C_INCLUDES += $(MTK_MTKCAM_PLATFORM)/include/mtkcam/drv
LOCAL_C_INCLUDES += $(TOP)/system/core/libion/kernel-headers/
#
#
# camera Hardware
#LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/frameworks/base/include
#LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/camera/inc
#LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/camera/inc/common
#LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_PLATFORM)/hardware/camera/
#LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_PLATFORM)/hardware/camera/inc
#LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_PLATFORM)/hardware/camera/inc/common
#LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/kernel/include
#LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_PLATFORM)/kernel/core/include/mach

#
ifeq "'1'" "$(strip $(MTKCAM_USE_LEGACY_JPEGHW))"
# jpeg encoder
LOCAL_SHARED_LIBRARIES += libJpgEncPipe
## m4u
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/m4u/$(PLATFORM)
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/m4u/$(PLATFORM)/D2
LOCAL_CFLAGS += -DMTKCAM_USE_LEGACY_JPEGHW="1"
endif
#
LOCAL_HEADER_LIBRARIES += libmtkcam_headers
# image transform
LOCAL_SHARED_LIBRARIES += libdpframework
LOCAL_CFLAGS += -DUSING_D2
#-----------------------------------------------------------
LOCAL_CFLAGS += $(MTKCAM_CFLAGS)

#

#
LOCAL_STATIC_LIBRARIES := \


#
LOCAL_MODULE := libcam.iopipe.simager
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk

#

#
include $(MTK_STATIC_LIBRARY)

#
include $(call all-makefiles-under,$(LOCAL_PATH))
