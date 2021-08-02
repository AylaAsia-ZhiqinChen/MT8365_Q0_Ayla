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


define all-c-cpp-under
$(patsubst ./%,%, \
$(shell cd $(LOCAL_PATH) ; \
find -L $(1) -maxdepth 1 \( -name "*.c" -or -name "*.cpp" \) -and -not -name ".*") \
)
endef

define all-cpp-files-under-custom
$(patsubst ./%,%, \
$(shell if [ -d $(1) ] ; then find -L $(1) -maxdepth 1 \( -name "*.c" -or -name "*.cpp" \) -and -not -name ".*" ; fi) \
)
endef

################################################################################
#
################################################################################
include $(CLEAR_VARS)

#-----------------------------------------------------------
LOCAL_EXPORT_C_INCLUDE_DIRS += $(TOP)/$(MTK_PATH_COMMON)/kernel/imgsensor/inc
LOCAL_EXPORT_C_INCLUDE_DIRS += $(TOP)/$(MTK_PATH_SOURCE)/external/mhal/inc
LOCAL_EXPORT_C_INCLUDE_DIRS += $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc
LOCAL_EXPORT_C_INCLUDE_DIRS += $(TOP)/$(MTK_PATH_CUSTOM)/hal/inc
LOCAL_EXPORT_C_INCLUDE_DIRS += $(TOP)/$(MTK_PATH_CUSTOM)/cgen/cfgfileinc

LOCAL_MODULE := libcustom_hal_for_lens_src_headers
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk

include $(BUILD_HEADER_LIBRARY)



include $(CLEAR_VARS)
LOCAL_HEADER_LIBRARIES := libutils_headers
LOCAL_HEADER_LIBRARIES := libcustom_hal_for_lens_src_headers

#-----------------------------------------------------------
LOCAL_SRC_FILES += lenslist.cpp
LOCAL_SRC_FILES += lens_para_Dummy.cpp
LOCAL_SRC_FILES += lens_para_OV5645AF.cpp
LOCAL_SRC_FILES += lens_para_SensorDrive.cpp
LOCAL_SRC_FILES += $(foreach f, $(CUSTOM_HAL_LENS), $(patsubst $(LOCAL_PATH)/%,%,$(call all-cpp-files-under-custom, $(LOCAL_PATH)/$(strip $(f)))))

#-----------------------------------------------------------
LOCAL_MODULE := libcameracustom.lens
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk

LOCAL_HEADER_LIBRARIES += libutils_headers
LOCAL_HEADER_LIBRARIES += libcustom_hal_for_lens_src_headers

#-----------------------------------------------------------
include $(MTK_STATIC_LIBRARY)




