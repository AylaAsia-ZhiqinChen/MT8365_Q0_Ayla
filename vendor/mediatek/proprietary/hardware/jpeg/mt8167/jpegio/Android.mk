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






ifeq ($(MTK_PLATFORM),$(filter $(MTK_PLATFORM),MT6752 MT8167))

LOCAL_PATH:= $(call my-dir)


################################################################################
# libJpgEnc_pipe
################################################################################

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
  jpeg_enc_pipe.cpp \
  ../utils/img_mmu.cpp \

LOCAL_C_INCLUDES:= \
  $(TOP)/bionic \
  $(TOP)/$(MTK_PATH_SOURCE)/external/libjpeg-alpha/include \
  external/skia/include/core     \
  external/skia/include/effects  \
  external/skia/include/images   \
  external/skia/src/ports        \
  external/skia/include/utils    \
  $(TOP)/$(MTK_PATH_SOURCE)/platform/inc \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/m4u/mt8167 \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/gdma/inc \
  $(TOP)/$(MTK_PATH_PLATFORM)/kernel/core/include/mach \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/include/enc \
  $(MTK_PATH_SOURCE)/external/mhal/src/core/drv/inc \
  $(MTK_PATH_SOURCE)/kernel/include/linux \
  $(MTK_PATH_SOURCE)/external/include \
  $(TOP)/$(MTK_PATH_SOURCE)/kernel/include/linux/vcodec \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/mt8167/hal \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/mt8167/utils \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/mt8167/inc \
  $(TOP)/$(MTK_PATH_SOURCE)/external \

#  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/jpeg/hal \
#  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/jpeg/utils \

#  $(TOP)/$(MTK_PATH_SOURCE)/external/mhal/src/core/drv/common/inc \
#  $(TOP)/$(MTK_PATH_SOURCE)/external/mhal/src/core/pipe/common/inc \
#  $(TOP)/$(MTK_PATH_SOURCE)/external/mhal/src/core/drv/6752/inc \
#  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/inc/mhal \
#  $(TOP)/$(MTK_PATH_SOURCE)/external/mhal/src/core/drv/6752/jpeg \
#  $(TOP)/$(MTK_PATH_SOURCE)/external/mhal/src/core/pipe/6752/inc \



LOCAL_SHARED_LIBRARIES := \
     liblog \
     libutils \
     libcutils \
     libion \
     libion_mtk \
     libm4u \
     libjpeg-alpha_vendor \
#     libvcodec_utility \

#LOCAL_STATIC_LIBRARIES := \


ifeq ($(strip $(MTK_HW_ENHANCE)), yes)
    LOCAL_CFLAGS += -DMTK_HW_ENHANCE
endif

LOCAL_WHOLE_STATIC_LIBRARIES := \
   libJpgEncDrv_plat \

#LOCAL_CFLAGS += -DJENC_LOCK_VIDEO
LOCAL_CFLAGS += -DJENC_LOCK_HWENC


LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := libJpgEncPipe
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_MULTILIB := both

LOCAL_EXPORT_C_INCLUDE_DIRS := \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/include
include $(MTK_SHARED_LIBRARY)

#LOCAL_MODULE := libJpgEnc_pipe
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
#
#include $(MTK_STATIC_LIBRARY)





################################################################################
# libmhaljpeg_pipe.a
################################################################################


#include $(CLEAR_VARS)
#
#LOCAL_SHARED_LIBRARIES := \
#     libutils \
#     libcutils \
#     liblog \
#     libmhaldrv \
#
#
#LOCAL_SHARED_LIBRARIES += \
#
#
#LOCAL_WHOLE_STATIC_LIBRARIES := \
#   libmhaldrv_plat \
#   libmhaljpeg_pipe \
#
#
##LOCAL_MODULE := libmhaljpeg_pipe
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
#LOCAL_MODULE := libJpgDecPipe
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
#
#LOCAL_PRELINK_MODULE := false
#
#include $(MTK_SHARED_LIBRARY)

################################################################################
#
################################################################################
include $(CLEAR_VARS)
include $(call all-makefiles-under,$(LOCAL_PATH))


#===============================================================================

endif
