# Copyright Statement:
#
# This software/firmware and related documentation ("MediaTek Software") are
# protected under relevant copyright laws. The information contained herein
# is confidential and proprietary to MediaTek Inc. and/or its licensors.
# Without the prior written permission of MediaTek inc. and/or its licensors,
# any reproduction, modification, use or disclosure of MediaTek Software,
# and information contained herein, in whole or in part, shall be strictly prohibited.
#
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

LOCAL_PATH := $(call my-dir)

# Stub sound_trigger HAL module, used for tests
include $(CLEAR_VARS)

LOCAL_HEADER_LIBRARIES := \
    libaudioclient_headers \
    libbase_headers \
    libhardware_headers \
    libaudio_system_headers \
    libmedia_headers \
    liblog_headers \
    libsoundtrigger_headers \
    pcm_merge_common_includes \
    audio_hal_common_includes

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    liblog \
    libutils \
    libhardware_legacy \
    libhardware \
    libmedia_helper \
    libdl \
    libtinyalsa \
    libmsb \
    android.hardware.bluetooth.a2dp@1.0

ifeq ($(MTK_AIS_STHAL_PROCESS_SUPPORT),yes)
     LOCAL_SHARED_LIBRARIES += libais
     LOCAL_HEADER_LIBRARIES += capiv2_api_headers
     LOCAL_CFLAGS += -DAIS_STHAL_PROCESS_SUPPORT
endif

    LOCAL_SRC_FILES += sound_trigger_process.cpp
    LOCAL_SRC_FILES += sound_trigger_in_ctrl.cpp
    LOCAL_SRC_FILES += sound_trigger_out_ctrl.cpp
    LOCAL_SRC_FILES += sound_trigger_hw.cpp

# AEE
ifeq ($(HAVE_AEE_FEATURE),yes)
    LOCAL_SHARED_LIBRARIES += libaedv
    LOCAL_CFLAGS += -DHAVE_AEE_FEATURE
endif


LOCAL_MODULE := sound_trigger.primary.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_TAGS := optional
LOCAL_MULTILIB := both

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:= libsoundtrigger_headers
LOCAL_EXPORT_C_INCLUDES_DIRS := \
    $(MTK_PATH_SOURCE)/external/pcm_merge_proc/msb/inc \
    $(MTK_PATH_SOURCE)/hardware/audio/common/include \
    $(MTK_PATH_SOURCE)/hardware/audio/common/include/hardware_legacy
include $(BUILD_HEADER_LIBRARY)
