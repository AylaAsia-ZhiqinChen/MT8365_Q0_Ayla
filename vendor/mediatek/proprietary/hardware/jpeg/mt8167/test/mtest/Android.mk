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


#
# libmjpeg
#
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)


ifeq ($(MTK_PLATFORM),MT8167)

LOCAL_SRC_FILES:= \
   jpeg_dec_test_mhal.cpp \

LOCAL_SHARED_LIBRARIES := \
  libcutils \
  liblog \
  libm4u \
  libJpgEncPipe \
  libJpgDecPipe \
  libjpeg 

#LOCAL_SHARED_LIBRARIES := \
  libcutils \
  liblog \
  libskia \
  libm4u \
  libJpgDecPipe \
   libstlport \
   libcamdrv \
   libimageio \
   libvcodecdrv \
   libvcodec_utility \
   libJpgEncPipe \


LOCAL_C_INCLUDES:= \
  $(TOP)/system/core/include/cutils \
  $(TOP)/bionic \
  $(TOP)/external/jpeg \
  $(TOP)/external/skia/include/images \
  $(TOP)/external/skia/include/core \
  $(TOP)/external/stlport/stlport \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/m4u \
  $(TOP)/$(MTK_PATH_PLATFORM)/kernel/core/include/mach \
  $(MTK_PATH_SOURCE)external/mhal/src/core/drv/inc \
  $(TOP)/$(MTK_PATH_PLATFORM)/kernel/core/include/mach \
  $(MTK_PATH_SOURCE)kernel/include/linux \
  $(TOP)/$(MTK_PATH_PLATFORM)/kernel/core/include/mach \
  $(MTK_PATH_SOURCE)external/mhal/src/core/drv/inc \
  $(TOP)/$(MTK_PATH_PLATFORM)/kernel/drivers/jpeg \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/mt8127/inc \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/m4u/mt8127 \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/inc/platform/$(MTK_PLATFORM_DIR) \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/jpeg/utils \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/include/mhal \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/inc/mhal \
  $(TOP)/$(MTK_PATH_SOURCE)/platform/inc \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/vcodec/inc \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/dpframework/inc \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/dpframework/inc \
  $(TOP)/$(MTK_PATH_SOURCE)/protect-bsp/external/mhal/src/core/drv/inc \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/gdma/inc \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/gdma/hal \
  
LOCAL_C_INCLUDES += \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/include/mhal \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/include/enc \
  $(TOP)/bionic \

LOCAL_SHARED_LIBRARIES += libmhalImageCodec \
						  liblog \
						  libutils \
						  libcutils 
LOCAL_C_INCLUDES += $(TOP)/mediatek/external/mhal/inc
LOCAL_C_INCLUDES += $(TOP)/system/core/include/system 
LOCAL_CFLAGS += -Wno-multichar

LOCAL_MODULE:= mhalJpegTest
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk


LOCAL_MODULE_TAGS := tests


LOCAL_PRELINK_MODULE:=false

include $(MTK_EXECUTABLE)

endif




