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






ifeq ($(MTK_PLATFORM),$(filter $(MTK_PLATFORM),MT8168))

LOCAL_PATH:= $(call my-dir)


################################################################################
# libJpgDec_pipe.a
################################################################################


include $(CLEAR_VARS)

#LOCAL_SRC_FILES:= \
  jpegio/jpeg_dec_comp.cpp \


LOCAL_C_INCLUDES:= \
  $(TOP)/bionic \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/m4u/mt8168 \
  $(TOP)/$(MTK_PATH_PLATFORM)/kernel/core/include/mach \
  $(MTK_PATH_SOURCE)external/mhal/src/core/drv/inc \
  $(TOP)/$(MTK_PATH_PLATFORM)/exteranl/ldvt/include \
  $(MTK_PATH_SOURCE)kernel/include/linux \
  $(MTK_PATH_SOURCE)kernel/include/linux/vcodec \
  $(MTK_PATH_SOURCE)kernel/include/linux/vcodec/user \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/inc/platform/$(MTK_PLATFORM_DIR) \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/jpeg/hal \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/mt8168/inc \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/mt8168/hal \



LOCAL_SHARED_LIBRARIES += \
  liblog \
  libutils \
  libcutils \
  libm4u \


LOCAL_STATIC_LIBRARIES := \


LOCAL_WHOLE_STATIC_LIBRARIES := \

LOCAL_HEADER_LIBRARIES := liblog_headers

LOCAL_MODULE := libJpgDec_comp
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_MULTILIB := both

#include $(MTK_STATIC_LIBRARY)



################################################################################
# libJpgDecPipe.so
################################################################################


include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
  jpegio/jpeg_dec_pipe.cpp \
  imagecodec/mhal_jpeg_dec.cpp \
  utils/img_mmu.cpp \
  jpegio/jpeg_dec_comp.cpp \
#  imagecodec/mhal_imgScaler.cpp \
#  webp/webp_dec_comp.cpp \
#  ../../gdma/inc/img_cdp_pipe.h \

LOCAL_C_INCLUDES:= \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/m4u/mt8168 \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/vcodec/inc \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/include/mhal \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/libvcodec/mt8168/common \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/libvcodec/mt8168/common/vp8_dec \
  $(TOP)/$(MTK_PATH_SOURCE)/protect-bsp/external/mhal/src/core/drv/inc \
  $(TOP)/$(MTK_PATH_SOURCE)/protect-bsp/external/mhal/src/core/drv/6752/vcodec/include \
  $(TOP)/$(MTK_PATH_SOURCE)/protect-bsp/external/mhal/src/core/drv/6752/vcodec/common/ \
  $(TOP)/$(MTK_PATH_SOURCE)/protect-bsp/external/mhal/src/core/drv/6752/vcodec/common/vp8_dec \
  $(TOP)/$(MTK_PATH_PLATFORM)/kernel/core/include/mach \
  $(MTK_PATH_SOURCE)external/mhal/src/core/drv/inc \
  $(TOP)/$(MTK_PATH_PLATFORM)/kernel/core/include/mach \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/camera/ \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/camera/inc \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/camera/inc/imageio \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/camera/inc/campipe \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/camera/inc/drv \
  $(TOP)/$(MTK_PATH_PLATFORM)/exteranl/ldvt/include \
  $(TOP)/$(MTK_PATH_PLATFORM)/exteranl/ldvt/ts/camera/mhal/src/core/drv/common/imgsensor \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/camera/inc/scenario \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/camera/core/inc/scenario \
  $(MTK_PATH_SOURCE)/kernel/include/linux \
  $(MTK_PATH_SOURCE)/kernel/include/linux/vcodec \
  $(MTK_PATH_SOURCE)/kernel/include/linux/vcodec/user \
  $(MTK_PATH_SOURCE)/external/include \
  $(MTK_PATH_SOURCE)/external \
  $(TOP)/external/jpeg \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/mt8168/inc \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/mt8168/hal \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/mt8168/utils \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/mt8168/webp/inc \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/gdma/inc \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/inc/platform/mt8168 \
  $(TOP)/$(MTK_PATH_PLATFORM)/hardware/jpeg/webp/inc \
  $(TOP)/frameworks/native/include \
  $(TOP)/system/core/include/utils \
  #$(TOP)/$(MTK_PATH_PLATFORM)/hardware/gdma/hal \
  #$(TOP)/$(MTK_PATH_PLATFORM)/hardware/jpeg/utils \
  #$(TOP)/$(MTK_PATH_PLATFORM)/hardware/jpeg/hal \
  #$(MTK_PATH_SOURCE)hardware/jpeg/inc/mhal \


#  $(TOP)/$(MTK_PATH_SOURCE)/external/mhal/src/core/drv/inc \
#  $(TOP)/$(MTK_PATH_SOURCE)/external/mhal/src/core/drv/6752/vcodec/include \
#  $(TOP)/$(MTK_PATH_SOURCE)/external/mhal/src/core/drv/6752/vcodec/common/ \
#  $(TOP)/$(MTK_PATH_SOURCE)/external/mhal/src/core/drv/6752/vcodec/common/vp8_dec \



LOCAL_SHARED_LIBRARIES += \
  liblog \
  libutils \
  libcutils \
  libion \
  libion_mtk \
  libm4u \
  libSwJpgCodec \
  libGdmaScalerPipe \
  libjpeg-alpha_vendor \
  libdpframework \


LOCAL_STATIC_LIBRARIES := \

LOCAL_HEADER_LIBRARIES := liblog_headers

LOCAL_WHOLE_STATIC_LIBRARIES := \
  libJpgDecDrv_plat \
#   libGdmaFmtDrv_plat \


#LOCAL_CFLAGS += -DUSE_VDEC_SO
#LOCAL_CFLAGS += -DBACK_GROUND_LOW
#LOCAL_CFLAGS += -DCHECK_SW_SMALL
LOCAL_CFLAGS += -DJPEG_HW_DECODE_COMP

LOCAL_MODULE := libJpgDecPipe
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_MULTILIB := both

LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

LOCAL_EXPORT_C_INCLUDE_DIRS := \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/include
include $(MTK_SHARED_LIBRARY)





################################################################################
# libmHalImgCodec.so
################################################################################





include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
  imagecodec/mhal_image_codec.cpp \
  imagecodec/mhal_jpeg_enc.cpp \

#MediaHal.h
LOCAL_C_INCLUDES += \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/include/mhal \
# $(TOP)/$(MTK_ROOT)/hardware/jpeg/inc/mhal \

LOCAL_C_INCLUDES += \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/inc/platform/$(MTK_PLATFORM_DIR) \
  $(TOP)/system/core/include/cutils \
  $(TOP)/$(MTK_PATH_PLATFORM)/kernel/core/include/mach \
  $(TOP)/frameworks/base/include \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/mt8168/inc \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/mt8168/hal \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/mt8168/utils \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/m4u/mt8168 \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/inc/mhal \


LOCAL_STATIC_LIBRARIES := \

LOCAL_WHOLE_STATIC_LIBRARIES := \
#    libmhaldrvmdp_plat \
#    libmhalpipemdp_plat \
#    libmhaljpeg_pipe \
#    libmhaljpeg_scenario \

LOCAL_SHARED_LIBRARIES:= \
  liblog \
  libcutils \
  libutils \
  libJpgDecPipe \
  libJpgEncPipe \

LOCAL_HEADER_LIBRARIES := liblog_headers

ifeq ($(MTK_CAM_SECURITY_SUPPORT),yes)
LOCAL_CFLAGS += -DJPEG_SUPPORT_SECURE_PATH
endif

LOCAL_MODULE:= libmhalImageCodec
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_MULTILIB := both
LOCAL_MODULE_TAGS := optional

LOCAL_EXPORT_C_INCLUDE_DIRS := \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/jpeg/include
include $(MTK_SHARED_LIBRARY)





################################################################################
#
################################################################################
include $(CLEAR_VARS)
include $(call all-makefiles-under,$(LOCAL_PATH))


#===============================================================================

endif
