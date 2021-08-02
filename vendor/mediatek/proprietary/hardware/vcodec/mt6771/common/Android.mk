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

LOCAL_PATH:= $(call my-dir)

define vcodec-common-prebuilt32
$(foreach t,$(1), \
  $(eval include $(CLEAR_VARS)) \
  $(eval LOCAL_MODULE_CLASS := SHARED_LIBRARIES) \
  $(eval LOCAL_MODULE_TAGS := optional) \
  $(eval LOCAL_MODULE := $(basename $(notdir $(t)))) \
  $(eval LOCAL_PROPRIETARY_MODULE := true) \
  $(eval LOCAL_MODULE_OWNER := mtk) \
  $(eval LOCAL_SRC_FILES_arm := $(t)) \
  $(eval LOCAL_MODULE_SUFFIX := .so) \
  $(eval LOCAL_MULTILIB := 32) \
  $(eval include $(BUILD_PREBUILT)))
endef

$(call vcodec-common-prebuilt32, \
	lib/libvcodec_oal.so \
        lib/libh264dec_customize.so \
	lib/libh264dec_sa.ca7.so \
	lib/libh264dec_sd.ca7.so \
	lib/libh264dec_se.ca7.so \
	lib/libh264enc_sa.ca7.so \
	lib/libHEVCdec_sa.ca7.android.so \
	lib/libhevce_sb.ca7.android.so	\
	lib/libmp4enc_sa.ca7.so \
	lib/libmp4enc_xa.ca7.so \
 	lib/libvc1dec_sa.ca7.so \
	lib/libvp8dec_sa.ca7.so \
	lib/libvp8enc_sa.ca7.so \
	lib/libvp9dec_sa.ca7.so)
	
define vcodec-common-prebuilt64
$(foreach t,$(1), \
  $(eval include $(CLEAR_VARS)) \
  $(eval LOCAL_MODULE_CLASS := SHARED_LIBRARIES) \
  $(eval LOCAL_MODULE_TAGS := optional) \
  $(eval LOCAL_MODULE := $(basename $(notdir $(t)))) \
  $(eval LOCAL_PROPRIETARY_MODULE := true) \
  $(eval LOCAL_MODULE_OWNER := mtk) \
  $(eval LOCAL_SRC_FILES_arm64 := $(t)) \
  $(eval LOCAL_MODULE_SUFFIX := .so) \
  $(eval LOCAL_MULTILIB := 64) \
  $(eval include $(BUILD_PREBUILT)))
endef

$(call vcodec-common-prebuilt64, \
	lib64/libvcodec_oal.so \
	lib64/libhevce_sb.ca7.android.so)
