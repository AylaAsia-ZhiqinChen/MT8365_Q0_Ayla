# Copyright Statement:
#
# This software/firmware and related documentation ("MediaTek Software") are
# protected under relevant copyright laws. The information contained herein
# is confidential and proprietary to MediaTek Inc. and/or its licensors.
# Without the prior written permission of MediaTek inc. and/or its licensors,
# any reproduction, modification, use or disclosure of MediaTek Software,
# and information contained herein, in whole or in part, shall be strictly prohibited.
#
# MediaTek Inc. (C) 2017. All rights reserved.
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

## This is OP08 operator config file

# Set for carrier express features in this File for OP08

ifeq ($(strip $(MTK_CIP_SUPPORT)), yes)
USP_PATH_ROOT := /custom
else
USP_PATH_ROOT := /system
endif

USP_OPERATOR_APK_PATH :=

USP_OPERATOR_PACKAGES := com.mediatek.op08.settingsProvider \
                com.mediatek.op08.systemui \
                com.mediatek.op08.cellbroadcastreceiver \
                com.mediatek.op08.settings \
                com.mediatek.op08.phone \
                com.mediatek.op08.dialer \
                com.mediatek.op08.settings.mulitine \
                com.mediatek.op08.telephony \
                com.mediatek.digits


USP_OPERATOR_FEATURES := MTK_EPDG_CIP_SUPPORT=yes \
                MTK_WFC_SUPPORT=yes \
                MTK_VOLTE_SUPPORT=yes \
                MSSI_MTK_VILTE_SUPPORT=no \
                MSSI_MTK_VIWIFI_SUPPORT=no \
                MSSI_MTK_UCE_SUPPORT=no \
                MTK_RCS_SUPPORT=no \
                MTK_RCS_UA_SUPPORT=no \
                MTK_RTT_SUPPORT=yes \
                MSSI_MTK_VZW_DEVICE_TYPE=0 \
                MTK_DIGITS_SUPPORT=no \
