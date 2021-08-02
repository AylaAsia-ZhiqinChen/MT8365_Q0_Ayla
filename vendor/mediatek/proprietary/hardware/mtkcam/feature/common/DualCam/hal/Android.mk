# Copyright Statement:
#
# This software/firmware and related documentation ("MediaTek Software") are
# protected under relevant copyright laws. The information contained herein
# is confidential and proprietary to MediaTek Inc. and/or its licensors.
# Without the prior written permission of MediaTek inc. and/or its licensors,
# any reproduction, modification, use or disclosure of MediaTek Software,
# and information contained herein, in whole or in part, shall be strictly prohibited.

# MediaTek Inc. (C) 2016. All rights reserved.
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
ifneq ($(strip $(MTK_EMULATOR_SUPPORT)),yes)
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
-include $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/mtkcam.mk


################################################################################
LOCAL_SRC_FILES += FOVHal.cpp

################################################################################

LOCAL_CFLAGS += $(MTKCAM_CFLAGS)
LOCAL_CFLAGS += -Wno-unused-value
LOCAL_CFLAGS += -Wno-unused-variable
LOCAL_CFLAGS += -Wno-unused-parameter
LOCAL_CFLAGS += -Wno-reorder
LOCAL_CFLAGS += -O3

################################################################################
#LOCAL_C_INCLUDES += $(call include-path-for, camera)
################################################################################
LOCAL_C_INCLUDES += $(MTKCAM_C_INCLUDES)
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/include
# need these?
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/gralloc_extra/include
#LOCAL_C_INCLUDES += $(MTKCAM_ALGO_INCLUDE)/libfov
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/feature/include
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/feature/common/DualCam/p2sync
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/feature/common/DualCam/policy
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/feature/common/DualCam/Frame3AControl
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/include/mtkcam/drv
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/include/mtkcam/aaa
ifeq ($(strip $(MTK_CAM_STEREO_DENOISE_SUPPORT)),yes)
LOCAL_C_INCLUDES += $(TOP)/system/media/camera/include
endif
# end
################################################################################

################################################################################
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/isp_tuning
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/cgen/cfgfileinc
# need these?
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/$(MTK_CAM_SW_VERSION)
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/custom/common/hal/inc
# end
################################################################################
LOCAL_SHARED_LIBRARIES += libc
#LOCAL_SHARED_LIBRARIES += libcamalgo
LOCAL_SHARED_LIBRARIES += libcam.iopipe
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libmtkcam_stdutils
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libgralloc_extra
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libstdc++
LOCAL_SHARED_LIBRARIES += libui
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libmtkcam_metadata
LOCAL_SHARED_LIBRARIES += libcameracustom
LOCAL_SHARED_LIBRARIES += libmtkcam_modulehelper
LOCAL_SHARED_LIBRARIES += libmtkcam_metastore
LOCAL_SHARED_LIBRARIES += libcam.hal3a.v3

ifeq ($(strip $(MTK_CAM_DUAL_ZOOM_SUPPORT)),yes)
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/libcamera_feature/libfov_lib/include
-include $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/feature/common/vsdof/vsdof_common.mk
ifeq ($(INDEP_STEREO_PROVIDER),true)
	LOCAL_SHARED_LIBRARIES += libfeature.stereo.provider
else
	LOCAL_SHARED_LIBRARIES += libfeature.vsdof.hal
endif
ifeq ($(strip $(MTK_CAM_DUAL_ZOOM_MTK_FOV_SUPPORT)),yes)
LOCAL_SHARED_LIBRARIES += libcamalgo.fov
endif
endif
################################################################################
# LOCAL_STATIC_LIBRARIES += libcameracustom.camera
################################################################################
################################################################################
LOCAL_MODULE := libmtkcam.DualCam.hal
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
################################################################################
ifneq ($(MTK_BASIC_PACKAGE),yes)
#include $(BUILD_STATIC_LIBRARY)
include $(BUILD_SHARED_LIBRARY)
endif
endif
