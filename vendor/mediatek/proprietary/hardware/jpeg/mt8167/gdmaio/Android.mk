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
# libCdpPipe
################################################################################

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
   img_mdp_pipe.cpp \
#   ../../jpeg/utils/img_mmu.cpp \


#LOCAL_SRC_FILES:= \
#   ../../jpeg/utils/img_mmu.cpp \
#   img_cdp_pipe.cpp \

LOCAL_C_INCLUDES:= \
  $(TOP)/bionic \
  $(TOP)/system/core/include/utils \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/m4u/mt8167 \
  $(MTK_PATH_SOURCE)external/mhal/src/core/drv/inc \
  $(TOP)/$(MTK_PATH_PLATFORM)/kernel/core/include/mach \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/camera/ \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/camera/inc \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/camera/inc/imageio \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/camera/inc/campipe \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/camera/inc/drv \
  $(TOP)/$(MTK_PATH_PLATFORM)/exteranl/ldvt/include \
  $(TOP)/$(MTK_PATH_PLATFORM)/exteranl/ldvt/ts/camera/mhal/src/core/drv/common/imgsensor \
  $(TOP)/$(MTK_PATH_PLATFORM)/kernel/core/include/mach \
  $(MTK_PATH_SOURCE)kernel/include/linux \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/mt8167/inc \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/mt8167/utils \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/mt8167/inc \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/mt8167/hal \


LOCAL_SHARED_LIBRARIES += \
   liblog \
   libutils \
   libcutils \
   libdpframework \
#   libm4u \
#   libcamdrv \
#   libimageio \
#   libcam.campipe \

LOCAL_STATIC_LIBRARIES := \

LOCAL_WHOLE_STATIC_LIBRARIES := \
#   libGdmaCtrDrv_plat \

#LOCAL_MODULE := libGdmaCdp_pipe
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE := libGdmaScalerPipe
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_MULTILIB := both

LOCAL_MODULE_TAGS := optional


include $(MTK_SHARED_LIBRARY)




################################################################################
#
################################################################################
include $(CLEAR_VARS)
include $(call all-makefiles-under,$(LOCAL_PATH))


#===============================================================================

endif
