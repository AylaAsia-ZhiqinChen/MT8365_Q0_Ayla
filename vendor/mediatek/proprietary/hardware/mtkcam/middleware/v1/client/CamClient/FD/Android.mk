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
################################################################################

LOCAL_PATH := $(call my-dir)

################################################################################
#
################################################################################
include $(CLEAR_VARS)

-include $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/mtkcam.mk
-include $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/feature/common/faceeffect/facefeature.mk

#-----------------------------------------------------------
MTKCAM_HAVE_FD_CLIENT       ?= '1'  # built-in if '1' ; otherwise not built-in
MTKCAM_HAVE_3A_HAL          := '1'  # built-in if '1' ; otherwise not built-in

#-----------------------------------------------------------
LOCAL_SRC_FILES += FDBufMgr.cpp
LOCAL_SRC_FILES += FDClient.BufOps.cpp
LOCAL_SRC_FILES += FDClient.Scenario.cpp
LOCAL_SRC_FILES += FDClient.Thread.cpp
LOCAL_SRC_FILES += FDClient.cpp
LOCAL_SRC_FILES += FDClient.CBThread.cpp

#-----------------------------------------------------------
LOCAL_C_INCLUDES += $(MTKCAM_C_INCLUDES)
LOCAL_C_INCLUDES += $(MY_CLIENT_C_INCLUDES)
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/gralloc_extra/include
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/include
#
LOCAL_C_INCLUDES += $(TOP)/system/media/camera/include
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/frameworks/av/include
LOCAL_C_INCLUDES += $(MTKCAM_ALGO_INCLUDE)/libhrd
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mmsdk/include
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/libcamera_feature/libfdft_lib/include

#-----------------------------------------------------------
ifeq ($(MTK_CAM_ASD_SUPPORT),yes)
LOCAL_CFLAGS += -DAUTO_SCENE_DETECT_SUPPORT=1
else
LOCAL_CFLAGS += -DAUTO_SCENE_DETECT_SUPPORT=0
endif
LOCAL_CFLAGS += $(MTKCAM_CFLAGS)
LOCAL_CFLAGS += -DMTKCAM_HAVE_3A_HAL="$(MTKCAM_HAVE_3A_HAL)"
# HR Monitor
ifeq ($(strip $(MTK_HEART_RATE_MONITOR_SUPPORT)),yes)
MTKCAM_HR_MONITOR_SUPPORT                 := '1'
else
MTKCAM_HR_MONITOR_SUPPORT                 := '0'
endif
LOCAL_CFLAGS += -DMTKCAM_HR_MONITOR_SUPPORT="$(MTKCAM_HR_MONITOR_SUPPORT)"

# HDR detection
MTK_CAM_HDR_DETECTION_MODE                ?= 0
LOCAL_CFLAGS += -DMTKCAM_HDR_DETECTION_MODE="$(MTK_CAM_HDR_DETECTION_MODE)"

# Stereo FD support
ifeq ($(MTK_CAM_STEREO_CAMERA_SUPPORT), yes)
	BUILD_STEREO_HAL=yes
endif
ifeq ($(MTK_CAM_DUAL_ZOOM_SUPPORT), yes)
	BUILD_STEREO_HAL=yes
endif

ifeq ($(BUILD_STEREO_HAL), yes)
LOCAL_CFLAGS += -DSTEREO_SUPPORTED=1
else
LOCAL_CFLAGS += -DSTEREO_SUPPORTED=0
endif

ifeq ($(FDVER),50)
LOCAL_CFLAGS += -DPADDING_SUPPORTED=0
else
LOCAL_CFLAGS += -DPADDING_SUPPORTED=1
endif

#-----------------------------------------------------------
LOCAL_WHOLE_STATIC_LIBRARIES += libcam.client.camclient.asd
#

LOCAL_SHARED_LIBRARIES += libui
#-----------------------------------------------------------
LOCAL_ARM_MODE := arm
LOCAL_MODULE := libcam.client.camclient.fd
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk


#-----------------------------------------------------------
ifeq "'1'" "$(strip $(MTKCAM_HAVE_FD_CLIENT))"
    include $(MTK_STATIC_LIBRARY)
else
    $(warning "Not Build FD Client")
endif


################################################################################
#
################################################################################
include $(CLEAR_VARS)
include $(call all-makefiles-under,$(LOCAL_PATH))

