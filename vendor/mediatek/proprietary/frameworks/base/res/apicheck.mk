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

ifneq ($(strip $(MTK_BSP_PACKAGE)),yes)
ifneq ($(strip $(MTK_BASIC_PACKAGE)),yes)

LOCAL_PATH := $(call my-dir)
SRC_MTK_API_DIR := vendor/mediatek/proprietary/frameworks/api


.PHONY: checkframeworkresidapi

# eval this to define a rule that runs apicheck.
#
# Args:
#    $(1)  target
#    $(2)  stable api xml file
#    $(3)  api xml file to be tested
#    $(4)  stable removed api file
#    $(5)  removed api file to be tested
#    $(6)  arguments for apicheck
#    $(7)  command to run if apicheck failed

define check-framework-res-id-api
$(TARGET_OUT_COMMON_INTERMEDIATES)/PACKAGING/$(strip $(1))-timestamp: $(2) $(3) $(4) $(APICHECK)
	@echo "Checking Framework Resource ID API:" $(1)
	$(hide) ( $(APICHECK_COMMAND) $(6) $(2) $(3) $(4) $(5) || ( $(7) ; exit 38 ) )
	$(hide) mkdir -p $$(dir $$@)
	$(hide) touch $$@
checkframeworkresidapi: $(TARGET_OUT_COMMON_INTERMEDIATES)/PACKAGING/$(strip $(1))-timestamp
endef

checkapi: checkframeworkresidapi

# Get Framework Resource ID API TXT of newest level.
last_framework_res_id_version := $(lastword $(call numerically_sort,\
	$(patsubst $(SRC_MTK_API_DIR)/framework_res_id/%.txt,%, \
	$(wildcard $(SRC_MTK_API_DIR)/framework_res_id/*.txt))))

FRAMEWORK_RES_ID_API_FILE := $(TARGET_OUT_COMMON_INTERMEDIATES)/PACKAGING/framework_res_id_api.txt

# Check that the API we're building hasn't broken the last-framework res id version.
# When fails, build will breaks with message from "apicheck_msg_framework_res_id_last" text file shown.
ifneq ($(strip $(PARTIAL_BUILD)),true)
$(eval $(call check-framework-res-id-api, \
	checkframeworkresidapi-last, \
	$(SRC_MTK_API_DIR)/framework_res_id/$(last_framework_res_id_version).txt, \
	$(FRAMEWORK_RES_ID_API_FILE), \
	$(SRC_MTK_API_DIR)/internal/removed.txt, \
	$(SRC_MTK_API_DIR)/internal/removed.txt, \
	-error 2 -error 3 -error 4 -hide 5 -error 6 -error 7 -error 8 -error 9 -error 10 \
	-error 11 -error 12 -error 13 -error 14 -error 15 -error 16 -error 17 -error 18 \
	-error 19 -error 20 -error 21 -error 23 -error 24 , \
	cat $(LOCAL_PATH)/apicheck_msg_framework_res_id_last.txt \
	))
endif
endif
endif
