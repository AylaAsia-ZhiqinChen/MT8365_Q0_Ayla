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

#-----------------------------------------------------------
MTKCAM_HAVE_GRALLOC_EXTRA           := '1'  # built-in if '1' ; otherwise not built-in
MTKCAM_HAVE_RECORD_CLIENT           ?= '1'  # built-in if '1' ; otherwise not built-in
MTKCAM_HAVE_FD_CLIENT               ?= '1'  # built-in if '1' ; otherwise not built-in
#
# OT CLient
ifeq ($(MTK_CAM_OT_SUPPORT),yes)
MTKCAM_HAVE_OT_CLIENT ?= '1'
else
MTKCAM_HAVE_OT_CLIENT ?= '0'
endif
#

#-----------------------------------------------------------

#-----------------------------------------------------------
LOCAL_C_INCLUDES += $(LOCAL_PATH)/inc
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/ext/include
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/inc
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/inc/common
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/client/CamClient/inc
LOCAL_C_INCLUDES += $(TOP)/$(MTK_MTKCAM_PLATFORM)/inc
LOCAL_C_INCLUDES += $(TOP)/$(MTK_MTKCAM_PLATFORM)/inc/common

#-----------------------------------------------------------
LOCAL_CFLAGS += -DMTKCAM_HAVE_RECORD_CLIENT="$(MTKCAM_HAVE_RECORD_CLIENT)"
LOCAL_CFLAGS += -DMTKCAM_HAVE_FD_CLIENT="$(MTKCAM_HAVE_FD_CLIENT)"
LOCAL_CFLAGS += -DMTKCAM_HAVE_OT_CLIENT="$(MTKCAM_HAVE_OT_CLIENT)"

#-----------------------------------------------------------
LOCAL_WHOLE_STATIC_LIBRARIES += libcam.client.camclient.common
#
ifeq "'1'" "$(strip $(MTKCAM_HAVE_RECORD_CLIENT))"
    LOCAL_WHOLE_STATIC_LIBRARIES += libcam.client.camclient.record.common
    LOCAL_WHOLE_STATIC_LIBRARIES += libcam.client.camclient.record.platform
endif
#
ifeq "'1'" "$(strip $(MTKCAM_HAVE_FD_CLIENT))"
    LOCAL_WHOLE_STATIC_LIBRARIES += libcam.client.camclient.fd
endif
#
#

# OT Client
ifeq "'1'" "$(strip $(MTKCAM_HAVE_OT_CLIENT))"
    LOCAL_WHOLE_STATIC_LIBRARIES += libcam.client.camclient.ot
endif
#

#-----------------------------------------------------------
LOCAL_MODULE := libcam.client.camclient
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk

#-----------------------------------------------------------

#
# Start of common part ------------------------------------


#-----------------------------------------------------------
LOCAL_CFLAGS += $(MTKCAM_CFLAGS)

#-----------------------------------------------------------
LOCAL_C_INCLUDES += $(MTKCAM_C_INCLUDES)
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/gralloc_extra/include
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/ext/include


#-----------------------------------------------------------
LOCAL_C_INCLUDES +=$(TOP)/$(MTK_MTKCAM_PLATFORM)/include
LOCAL_C_INCLUDES +=$(TOP)/$(MTKCAM_C_INCLUDES)
LOCAL_C_INCLUDES +=$(TOP)/$(MTKCAM_C_INCLUDES)/..

LOCAL_C_INCLUDES += $(TOP)/$(MTK_MTKCAM_PLATFORM)/include/

# End of common part ---------------------------------------
#
include $(MTK_STATIC_LIBRARY)




################################################################################
#
################################################################################
include $(CLEAR_VARS)
include $(call all-makefiles-under,$(LOCAL_PATH))

