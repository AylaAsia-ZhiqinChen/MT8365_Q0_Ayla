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
-include $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/mtkcam.mk
-include $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/drv/driver.mk
-include $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/aaa/aaa.mk

$(info MTKCAM_AAA_PLATFORM=$(MTKCAM_AAA_PLATFORM))

#MTKCAM_3A_PATH       := $(MTK_PATH_SOURCE)/hardware/mtkcam/aaa/source/$(TARGET_BOARD_PLATFORM)
MTKCAM_3A_PATH        := $(MTK_PATH_SOURCE)/hardware/mtkcam/aaa/source/$(MTKCAM_AAA_PLATFORM)
MTKCAM_3A_COMMON_PATH := $(MTK_PATH_SOURCE)/hardware/mtkcam/aaa/source/common
RELATIVE_PATH_MTKCAM_3A_COMMON  := ../common

$(info "========================ORIGINAL============================")
$(info "TOP=$(TOP)")
$(info "MTKCAM_INCLUDE=$(MTKCAM_INCLUDE)")
$(info "MTKCAM_3A_PATH=$(MTKCAM_3A_PATH)")
$(info "MTK_PATH_SOURCE=$(MTK_PATH_SOURCE)")
$(info "MTK_PATH_COMMON=$(MTK_PATH_COMMON)")
$(info "MTK_PATH_CUSTOM=$(MTK_PATH_CUSTOM)")
$(info "MTKCAM_ALGO_INCLUDE=$(MTKCAM_ALGO_INCLUDE)")
$(info "MTK_PATH_CUSTOM_PLATFORM=$(MTK_PATH_CUSTOM_PLATFORM)")
$(info "TARGET_BOARD_PLATFORM=$(TARGET_BOARD_PLATFORM)")
$(info "RELATIVE_PATH_MTKCAM_3A_COMMON=$(RELATIVE_PATH_MTKCAM_3A_COMMON)")
$(info "MTKCAM_INCLUDE=$(MTKCAM_DRV_INCLUDE)")
$(info "========================ORIGINAL============================")


#-----------------------------------------------------------
#LOCAL_SRC_FILES += Hal3AAdapter3.cpp
#LOCAL_SRC_FILES += Hal3AAdapter3Meta.cpp
#LOCAL_SRC_FILES += IHal3A.cpp
LOCAL_SRC_FILES += IspMgrIf.cpp
#LOCAL_SRC_FILES += aaa_hal_if.cpp
#LOCAL_SRC_FILES += aaa_hal_flowCtrl.cpp
#LOCAL_SRC_FILES += aaa_hal_flowCtrl.thread.cpp
#LOCAL_SRC_FILES += aaa_hal_raw.cpp
#LOCAL_SRC_FILES += aaa_hal_raw.thread.cpp
#LOCAL_SRC_FILES += Hal3AFlowCtrl.cpp
#LOCAL_SRC_FILES += Hal3AFlowCtrl_SMVR.cpp
#LOCAL_SRC_FILES += Thread3AImp.cpp
LOCAL_SRC_FILES += Hal3AMisc.cpp
LOCAL_SRC_FILES += Hal3ACbHub.cpp
LOCAL_SRC_FILES += ThreadRawImp.cpp
LOCAL_SRC_FILES += ThreadRawSMVRImp.cpp
#LOCAL_SRC_FILES += wrapper/Dft3AImp.cpp
LOCAL_SRC_FILES += wrapper/Hal3ARawImp.cpp
LOCAL_SRC_FILES += wrapper/Hal3ARawSMVRImp.cpp
LOCAL_SRC_FILES += wrapper/Hal3AYuvImp.cpp
#LOCAL_SRC_FILES += wrapper/I3AWrapper.cpp
LOCAL_SRC_FILES += aaa_hal_sttCtrl.cpp
LOCAL_SRC_FILES += sensor_mgr/aaa_sensor_mgr.cpp
LOCAL_SRC_FILES += awb_mgr/awb_mgr.cpp
LOCAL_SRC_FILES += awb_mgr/awb_cct_feature.cpp
LOCAL_SRC_FILES += awb_mgr/awb_state.cpp
LOCAL_SRC_FILES += awb_mgr/awb_mgr_if.cpp
LOCAL_SRC_FILES += awb_mgr/awb_cb.cpp
LOCAL_SRC_FILES += ae_mgr/AeSetting/IAeSetting.cpp
LOCAL_SRC_FILES += ae_mgr/AeSetting/AeSettingCCU.cpp
LOCAL_SRC_FILES += ae_mgr/AeSetting/AeSettingCPU.cpp
LOCAL_SRC_FILES += ae_mgr/AeSetting/AeSettingDefault.cpp
LOCAL_SRC_FILES += ae_mgr/AeSetting/AeSettingVHDR.cpp
LOCAL_SRC_FILES += ae_mgr/AeFlow/IAeFlow.cpp
LOCAL_SRC_FILES += ae_mgr/AeFlow/AeFlowCCU.cpp
LOCAL_SRC_FILES += ae_mgr/AeFlow/AeFlowCPU.cpp
LOCAL_SRC_FILES += ae_mgr/AeFlow/AeFlowDefault.cpp
LOCAL_SRC_FILES += ae_mgr/AeFlow/AeFlowVHDR.cpp
LOCAL_SRC_FILES += ae_mgr/AeTestImp.cpp
LOCAL_SRC_FILES += ae_mgr/ae_mgr.cpp
LOCAL_SRC_FILES += ae_mgr/ae_mgr_ctrl.cpp
LOCAL_SRC_FILES += ae_mgr/ae_mgr_pline.cpp
LOCAL_SRC_FILES += ae_mgr/ae_mgr_flow.cpp
LOCAL_SRC_FILES += ae_mgr/ae_mgr_setting.cpp
LOCAL_SRC_FILES += ae_mgr/ae_cct_feature.cpp
LOCAL_SRC_FILES += ae_mgr/ae_mgr_if.cpp
LOCAL_SRC_FILES += flash_mgr/flash_custom_adapter.cpp
LOCAL_SRC_FILES += flash_mgr/flash_platform_adapter.cpp
LOCAL_SRC_FILES += flash_mgr/flash_algo_adapter.cpp
LOCAL_SRC_FILES += flicker/flicker_platform_adapter.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_ctl.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_obc.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_dbs.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_bnr.cpp
#LOCAL_SRC_FILES += isp_mgr/isp_mgr_lsc.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_rpg.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_pgn.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_flc.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_udm.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_sl2.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_sl2_sensor.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_ccm.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_ggm.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_g2c.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_nbc.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_nbc2.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_pca.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_seee.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_nr3d.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_mfb.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_rmm.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_rmg.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_lce.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_rnr.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_hfg.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_ndg.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_ndg2.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_mixer3.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_awb_stat.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_ae_stat.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_af_stat.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_pdo.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_flk.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_helper.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_dcpn.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_cpn.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_adbs.cpp
LOCAL_SRC_FILES += isp_mgr/isp_debug.cpp
LOCAL_SRC_FILES += buf_mgr/AAOBufMgr.cpp
LOCAL_SRC_FILES += buf_mgr/PSOBufMgr.cpp
LOCAL_SRC_FILES += buf_mgr/AEOBufMgr.cpp
LOCAL_SRC_FILES += buf_mgr/AFOBufMgr.cpp
LOCAL_SRC_FILES += buf_mgr/FLKOBufMgr.cpp
LOCAL_SRC_FILES += buf_mgr/PDOBufMgr.cpp
LOCAL_SRC_FILES += buf_mgr/MVHDRBufMgr.cpp
LOCAL_SRC_FILES += buf_mgr/MVHDR3ExpoBufMgr.cpp
LOCAL_SRC_FILES += buf_mgr/AAOSeparation12.cpp
LOCAL_SRC_FILES += buf_mgr/AAOSeparation14.cpp
LOCAL_SRC_FILES += isp_tuning/isp_tuning_mgr.cpp
LOCAL_SRC_FILES += isp_tuning/isp_tuning_buf.cpp
LOCAL_SRC_FILES += isp_tuning/paramctrl/paramctrl_lifetime.cpp
LOCAL_SRC_FILES += isp_tuning/paramctrl/paramctrl_user.cpp
LOCAL_SRC_FILES += isp_tuning/paramctrl/paramctrl_attributes.cpp
LOCAL_SRC_FILES += isp_tuning/paramctrl/paramctrl_validate.cpp
LOCAL_SRC_FILES += isp_tuning/paramctrl/paramctrl_per_frame.cpp
LOCAL_SRC_FILES += isp_tuning/paramctrl/paramctrl_frameless.cpp
LOCAL_SRC_FILES += isp_tuning/paramctrl/paramctrl_exif.cpp
LOCAL_SRC_FILES += isp_tuning/paramctrl/pca_mgr/pca_mgr.cpp
LOCAL_SRC_FILES += isp_tuning/paramctrl/ccm_mgr/ccm_mgr.cpp
LOCAL_SRC_FILES += isp_tuning/paramctrl/ggm_mgr/ggm_mgr.cpp
LOCAL_SRC_FILES += isp_tuning/paramctrl/gma_mgr/gma_mgr.cpp
LOCAL_SRC_FILES += isp_tuning/paramctrl/lce_mgr/lce_mgr.cpp
LOCAL_SRC_FILES += isp_tuning/capturenr/capturenr.cpp
LOCAL_SRC_FILES += $(RELATIVE_PATH_MTKCAM_3A_COMMON)/iopipe/NormalPipeUtils.cpp
LOCAL_SRC_FILES += Thread/ThreadSensorGainImp.cpp
LOCAL_SRC_FILES += Thread/ThreadSensorI2CImp.cpp
LOCAL_SRC_FILES += Thread/ThreadStatisticBufImp.cpp
LOCAL_SRC_FILES += Thread/ThreadSensorBufferModeImp.cpp
LOCAL_SRC_FILES += Thread/ThreadSetCCUParaImp.cpp
LOCAL_SRC_FILES += lens/lens_drv.cpp
LOCAL_SRC_FILES += lens/mcu_drv.cpp
LOCAL_SRC_FILES += pd_mgr/pd_mgr.cpp
LOCAL_SRC_FILES += pd_mgr/pd_mgr_if.cpp
LOCAL_SRC_FILES += laser_mgr/laser_mgr.cpp
#LOCAL_SRC_FILES += debug/IDebugEntry.cpp
#LOCAL_SRC_FILES += debug/DebugUtil.cpp
#LOCAL_SRC_FILES += ResultBufCtrl/Hal3AResultBufImp.cpp
#LOCAL_SRC_FILES += ResultBufCtrl/IHal3AResultBuf.cpp
#LOCAL_SRC_FILES += ResultBufCtrl/IHal3AResultBufInfo.cpp
#LOCAL_SRC_FILES += ResultPool/ResultPool.cpp
#LOCAL_SRC_FILES += ResultPool/Entry.cpp
#LOCAL_SRC_FILES += ResultPool/ThreadResultPool.cpp
LOCAL_SRC_FILES += task/ITask.cpp
LOCAL_SRC_FILES += task/Task3APreview.cpp
LOCAL_SRC_FILES += task/Task3ATouch.cpp
LOCAL_SRC_FILES += task/Task3APrecapture.cpp
LOCAL_SRC_FILES += task/Task3AFlash.cpp
LOCAL_SRC_FILES += task/Task3AFlashFront.cpp
LOCAL_SRC_FILES += task/Task3AFlashBack.cpp
LOCAL_SRC_FILES += task/Task3ACapture.cpp
LOCAL_SRC_FILES += task/TaskAFNormal.cpp
LOCAL_SRC_FILES += task/TaskAFTrigger.cpp
LOCAL_SRC_FILES += task/TaskMgr.cpp

LOCAL_SRC_FILES += cct_svr/cctsvr_entry.cpp
#ACDK
MTKCAM_ACDK := $(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/acdk

#------------------ip base new include path-----------------------
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/include
#LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/include/mtkcam/drv/
#LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/drv/include/dip/$(MTKCAM_AAA_PLATFORM)
#LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/drv/include/dip/$(MTKCAM_AAA_PLATFORM)/drv
LOCAL_C_INCLUDES += $(MTKCAM_INCLUDE)
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/drv/include/$(MTKCAM_DRV_PLATFORM)/drv
LOCAL_C_INCLUDES += $(MTKCAM_DRV_INCLUDE)
#LOCAL_C_INCLUDES += $(MTKCAM_DRV_INCLUDE)/drv

LOCAL_C_INCLUDES += $(MTKCAM_ACDK)
LOCAL_C_INCLUDES += $(MTKCAM_ALGO_INCLUDE)
LOCAL_C_INCLUDES += $(MTKCAM_ALGO_INCLUDE)/libflicker
#LOCAL_C_INCLUDES += $(MTKCAM_ALGO_INCLUDE)/lib3a
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/libcamera_3a/libgma_lib/$(TARGET_BOARD_PLATFORM)/include
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/libcamera_3a/liblce_lib/$(TARGET_BOARD_PLATFORM)/include

#for AF
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/libcamera_3a/libaf_core_lib/$(TARGET_BOARD_PLATFORM)/include

#for CCU
LOCAL_STATIC_LIBRARIES += lib3a.ccu.headers
LOCAL_SHARED_LIBRARIES += lib3a.ccudrv
LOCAL_SHARED_LIBRARIES += lib3a.ccuif

#LOCAL_C_INCLUDES += $(MTKCAM_ALGO_INCLUDE)/libfdft
#LOCAL_C_INCLUDES += $(MTKCAM_ALGO_INCLUDE)/libsync3a

LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/aaa/include
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/EventIrq
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/awb_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/pd_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/laser_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/ae_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/flash_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/isp_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/buf_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/isp_tuning
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/isp_tuning/paramctrl/inc
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/isp_tuning/paramctrl/pca_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/isp_tuning/paramctrl/ccm_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/isp_tuning/paramctrl/ggm_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/isp_tuning/paramctrl/gma_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/isp_tuning/paramctrl/lce_mgr
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/aaa/source/common
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/aaa/source/common/lsc_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/sensor_mgr
#LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/ResultBufMgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/wrapper
#LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/ResultBufCtrl
#LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/ResultPool

#-----------------------------------------------------------
LOCAL_C_INCLUDES += $(MTKCAM_C_INCLUDES)

LOCAL_C_INCLUDES += $(MTKCAM_3A_COMMON_PATH)/hal3a/$(MTKCAM_COMMON_HAL3A_VERSION)
LOCAL_C_INCLUDES += $(MTKCAM_3A_COMMON_PATH)/hal3a/$(MTKCAM_COMMON_HAL3A_VERSION)/wrapper
LOCAL_C_INCLUDES += $(MTKCAM_3A_COMMON_PATH)/hal3a/$(MTKCAM_COMMON_HAL3A_VERSION)/buf_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_COMMON_PATH)/hal3a/$(MTKCAM_COMMON_HAL3A_VERSION)/ResultPool
LOCAL_C_INCLUDES += $(TOP)/system/media/camera/include/
LOCAL_C_INCLUDES += $(TOP)/device/mediatek/common/kernel-headers
LOCAL_C_INCLUDES += $(TOP)/vendor/mediatek/proprietary/external/libudf/libladder
LOCAL_C_INCLUDES += $(MTK_PATH_COMMON)/kernel/imgsensor/inc
LOCAL_C_INCLUDES += $(MTK_PATH_COMMON)/hal/inc
LOCAL_C_INCLUDES += $(MTK_PATH_COMMON)/hal/inc/camera_feature
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/isp_tuning
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/isp_tuning/$(MTK_CAM_SW_VERSION)
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM)/hal/camera
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM_PLATFORM)/hal/inc
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/aaa
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/pd_buf_mgr
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/mtkcam
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/debug_exif/aaa
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/debug_exif/cam
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM_PLATFORM)/hal/camera_3a
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM)/hal/camera/inc
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM_PLATFORM)/hal/camera/inc
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM)/hal/camera_3a/inc
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM_PLATFORM)/hal/camera_3a/inc
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/include/mtkcam
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/libcamera_3a/libaf_core_lib/$(MTK_PLATFORM_DIR)/include
#For AEPlineTable.h
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/custom
LOCAL_C_INCLUDES += $(MTK_PATH_COMMON)/hal/inc/custom/aaa
#For BuildInTypes.h
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/include/mtkcam/def
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/libcamera_feature/libflicker_lib/$(TARGET_BOARD_PLATFORM)/include

LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/external/aee/binary/inc
#capturenr
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/perfservice/perfservicenative
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/libcamera_feature/libvpunr_lib/$(TARGET_BOARD_PLATFORM)/include
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/include

LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/cct_svr
LOCAL_C_INCLUDES += $(MTKCAM_3A_COMMON_PATH)/utils/aaa_utils

# flash manager---------------------------------------------
#MTKCAM_3A_FLASH_PATH := ./flash_mgr
MTKCAM_3A_FLASH_PATH := $(RELATIVE_PATH_MTKCAM_3A_COMMON)/flash_mgr
LOCAL_SRC_FILES += $(MTKCAM_3A_FLASH_PATH)/flash_mgr.cpp
LOCAL_SRC_FILES += $(MTKCAM_3A_FLASH_PATH)/flash_mgr_m.cpp
LOCAL_SRC_FILES += $(MTKCAM_3A_FLASH_PATH)/flash_utils.cpp
LOCAL_SRC_FILES += $(MTKCAM_3A_FLASH_PATH)/flash_pline.cpp
LOCAL_SRC_FILES += $(MTKCAM_3A_FLASH_PATH)/flash_cct_quick2.cpp
LOCAL_SRC_FILES += $(MTKCAM_3A_FLASH_PATH)/flash_cct.cpp
LOCAL_SRC_FILES += $(MTKCAM_3A_FLASH_PATH)/flash_hal.cpp
LOCAL_SRC_FILES += $(MTKCAM_3A_FLASH_PATH)/flash_duty.cpp
LOCAL_SRC_FILES += $(MTKCAM_3A_FLASH_PATH)/flash_nvram.cpp
LOCAL_SRC_FILES += $(MTKCAM_3A_FLASH_PATH)/flash_refine.cpp
LOCAL_SRC_FILES += $(MTKCAM_3A_FLASH_PATH)/tools/flash_custom_utils.cpp
LOCAL_SRC_FILES += $(MTKCAM_3A_FLASH_PATH)/tools/flash_cali_utils.cpp
LOCAL_SRC_FILES += $(MTKCAM_3A_FLASH_PATH)/tools/flash_cali_xml.cpp
#LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/flash_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_COMMON_PATH)/flash_mgr

# strobe manager--------------------------------------------
#MTKCAM_3A_STROBE_PATH := ./strobe
MTKCAM_3A_STROBE_PATH := $(RELATIVE_PATH_MTKCAM_3A_COMMON)/strobe
LOCAL_SRC_FILES += $(MTKCAM_3A_STROBE_PATH)/strobe_drv.cpp
LOCAL_SRC_FILES += $(MTKCAM_3A_STROBE_PATH)/strobe_drv_flashlight.cpp
LOCAL_SRC_FILES += $(MTKCAM_3A_STROBE_PATH)/strobe_drv_flashlight_fops.cpp
LOCAL_SRC_FILES += $(MTKCAM_3A_STROBE_PATH)/strobe_drv_led.cpp
#LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/strobe
LOCAL_C_INCLUDES += $(MTKCAM_3A_COMMON_PATH)/strobe

# flicker manager--------------------------------------------
#MTKCAM_3A_FLICKER_PATH := ./flicker
MTKCAM_3A_FLICKER_PATH := $(RELATIVE_PATH_MTKCAM_3A_COMMON)/flicker
LOCAL_SRC_FILES += $(MTKCAM_3A_FLICKER_PATH)/flicker_hal_if.cpp
LOCAL_SRC_FILES += $(MTKCAM_3A_FLICKER_PATH)/flicker_hal.cpp
#LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/flicker
LOCAL_C_INCLUDES += $(MTKCAM_3A_COMMON_PATH)/flicker

# pd manager---------------------------------------------
ifeq ($(MTK_ION_SUPPORT), yes)
LOCAL_CFLAGS += -DUSING_MTK_ION
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/external
LOCAL_SHARED_LIBRARIES += libion libion_mtk
endif

# af manager------------------------------------------------
#### Select Code Folder : Common or Platform
MTKCAM_3A_AFMGR_PATH := ./af_mgr
#MTKCAM_3A_AFMGR_PATH  := $(RELATIVE_PATH_MTKCAM_3A_COMMON)/af_mgr
#### Source
LOCAL_SRC_FILES += $(MTKCAM_3A_AFMGR_PATH)/af_mgr.cpp
LOCAL_SRC_FILES += $(MTKCAM_3A_AFMGR_PATH)/af_mgr_if.cpp
LOCAL_SRC_FILES += $(MTKCAM_3A_AFMGR_PATH)/af_mgr_SyncAF.cpp
LOCAL_SRC_FILES += $(MTKCAM_3A_AFMGR_PATH)/af_cct_feature.cpp
#### Include
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/af_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/lens
#LOCAL_C_INCLUDES += $(MTKCAM_3A_COMMON_PATH)/af_mgr

LOCAL_CFLAGS += -DMTKCAM_CCU_AF_SUPPORT
ifeq ($(TARGET_BOARD_PLATFORM), mt6771)
LOCAL_CFLAGS += -DCCU_AF_FEATURE_EN=0
else
LOCAL_CFLAGS += -DCCU_AF_FEATURE_EN=0
endif

#-----------------------------------------------------------
LOCAL_CFLAGS += $(MTKCAM_CFLAGS)
#

ifeq ($(HAVE_AEE_FEATURE),yes)
    LOCAL_CFLAGS += -DHAVE_AEE_FEATURE
endif

# for 3A UT ---------------------------------
# UT must push lib64, close perframe AE, TG AE
USE_CAM3_FAKE_SENSOR_DRV := no
LOCAL_CFLAGS += -DCAM3_DEFAULT_ISP=0
LOCAL_CFLAGS += -DCAM3_DISABLE_SHADING=0

ifeq ($(USE_CAM3_FAKE_SENSOR_DRV),yes)
    LOCAL_SRC_FILES += fake_sensor_drv/NormalPipe.cpp
    LOCAL_SRC_FILES += fake_sensor_drv/StatisticPipe.cpp
    LOCAL_SRC_FILES += fake_sensor_drv/CamsvStatisticPipe.cpp
    LOCAL_SRC_FILES += fake_sensor_drv/NormalPipeModule.cpp
    LOCAL_CFLAGS += -DCAM3_FAKE_SENSOR_DRV=1
    LOCAL_CFLAGS += -DCAM3_DISABLE_AF_GSENSOR=1
    LOCAL_CFLAGS += -DCAM3_DISABLE_AE_PER_FRAME=1
else
    LOCAL_CFLAGS += -DCAM3_FAKE_SENSOR_DRV=0
    LOCAL_CFLAGS += -DCAM3_DISABLE_AF_GSENSOR=0
    LOCAL_CFLAGS += -DCAM3_DISABLE_AE_PER_FRAME=0
endif
#--------------------------------------------

#ISP 5.0
$(info "MTKCAM_IS_ISP_30_PLATFORM=$(MTKCAM_IS_ISP_30_PLATFORM)")
$(info "MTKCAM_IS_ISP_40_PLATFORM=$(MTKCAM_IS_ISP_40_PLATFORM)")
$(info "MTKCAM_IS_ISP_50_PLATFORM=$(MTKCAM_IS_ISP_50_PLATFORM)")
$(info "MTKCAM_IS_IP_BASE=$(MTKCAM_IS_IP_BASE)")
LOCAL_CFLAGS += -DCAM3_3A_ISP_30_EN=$(MTKCAM_IS_ISP_30_PLATFORM)
LOCAL_CFLAGS += -DCAM3_3A_ISP_40_EN=$(MTKCAM_IS_ISP_40_PLATFORM)
LOCAL_CFLAGS += -DCAM3_3A_ISP_50_EN=$(MTKCAM_IS_ISP_50_PLATFORM)
LOCAL_CFLAGS += -DCAM3_3A_IP_BASE=$(MTKCAM_IS_IP_BASE)

LOCAL_CFLAGS += -DUSE_AE_THD=1
LOCAL_CFLAGS += -DCAM3_3ATESTLVL=1

LOCAL_CFLAGS += -DCAM3_3AUT=0
LOCAL_CFLAGS += -DCAM3_3ASTTUT=1
LOCAL_CFLAGS += -DCAM3_3AIT=2
LOCAL_CFLAGS += -DCAM3_3AON=3

LOCAL_CFLAGS += -DCAM3_AF_FEATURE_EN=1
LOCAL_CFLAGS += -DCAM3_LSC_FEATURE_EN=1
LOCAL_CFLAGS += -DCAM3_FLASH_FEATURE_EN=1
LOCAL_CFLAGS += -DCAM3_FLICKER_FEATURE_EN=1
LOCAL_CFLAGS += -DCAM3_LSC2_USE_GAIN=0
LOCAL_CFLAGS += -DCAM3_AE_INTERRUPT_EN=1
LOCAL_CFLAGS += -DCAM3_AAO_NEON_EN=1
LOCAL_CFLAGS += -DCAM3_PSO_NEON_EN=1

#LOCAL_CFLAGS += -DMTKCAM_CMDQ_SUPPORT
#LOCAL_CFLAGS += -DMTKCAM_CMDQ_AA_SUPPORT

ifeq ($(TARGET_BUILD_VARIANT), user)
    LOCAL_CFLAGS += -DAE_PERFRAME_EN=1
else ifeq ($(TARGET_BUILD_VARIANT), userdebug)
    LOCAL_CFLAGS += -DAE_PERFRAME_EN=1
else
    LOCAL_CFLAGS += -DAE_PERFRAME_EN=0
endif

ifeq ($(strip $(MTK_NATIVE_3D_SUPPORT)),yes)
    LOCAL_CFLAGS += -DMTK_NATIVE_3D_SUPPORT
endif

# for Stereo Feature
ifeq ($(MTK_CAM_STEREO_CAMERA_SUPPORT),yes)
    LOCAL_SRC_FILES += Sync3A.cpp
    LOCAL_SRC_FILES += task/TaskSync2A.cpp
    LOCAL_SRC_FILES += task/TaskSyncAF.cpp
    LOCAL_SHARED_LIBRARIES += lib3a.n3d3a
    LOCAL_CFLAGS += -DCAM3_STEREO_FEATURE_EN=1
endif


ifeq ($(MTK_CAM_STEREO_CAMERA_SUPPORT), yes)
ifeq ($(MTK_CAM_STEREO_DENOISE_SUPPORT), yes)
MTKCAM_STEREO_DENOISE_SUPPORT   := '1'
else
MTKCAM_STEREO_DENOISE_SUPPORT   := '0'
endif
LOCAL_CFLAGS += -DMTKCAM_STEREO_DENOISE_SUPPORT="$(MTKCAM_STEREO_DENOISE_SUPPORT)"
endif


#-----------------------------------------------------------
LOCAL_SHARED_LIBRARIES += libfeatureiodrv_mem
LOCAL_STATIC_LIBRARIES += libcam.hal3a_utils

#libispfeature
LOCAL_STATIC_LIBRARIES += libispfeature_mtkcam
LOCAL_SHARED_LIBRARIES += libcamalgo.ispfeature
LOCAL_SHARED_LIBRARIES += libcamalgo.utility
LOCAL_WHOLE_STATIC_LIBRARIES += libcam.hal3a

#-----------------------------------------------------------
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libmtkcam_ulog
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libladder
LOCAL_SHARED_LIBRARIES += lib3a.awb.core
#
LOCAL_SHARED_LIBRARIES += libmtkcam_modulehelper
LOCAL_SHARED_LIBRARIES += libmtkcam_stdutils libmtkcam_imgbuf
LOCAL_SHARED_LIBRARIES += libmtkcam_hwutils
LOCAL_SHARED_LIBRARIES += lib3a.gma lib3a.lce
LOCAL_SHARED_LIBRARIES += lib3a.flash
LOCAL_SHARED_LIBRARIES += libcamalgo.lsc
LOCAL_SHARED_LIBRARIES += libcamalgo.flicker
LOCAL_SHARED_LIBRARIES += libcamalgo.fdft
LOCAL_SHARED_LIBRARIES += libcam3a_imem
LOCAL_SHARED_LIBRARIES += libmtkcam_metadata
LOCAL_SHARED_LIBRARIES += libmtkcam_metastore
LOCAL_SHARED_LIBRARIES += libmtkcam_sysutils
LOCAL_SHARED_LIBRARIES += libcam.pdtblgen
LOCAL_SHARED_LIBRARIES += libmtkcam_ulog

ifeq ($(USE_CAM3_FAKE_SENSOR_DRV),no)
LOCAL_SHARED_LIBRARIES += libcam.iopipe
endif

LOCAL_SHARED_LIBRARIES += libcam.hal3a.v3.nvram.50
LOCAL_SHARED_LIBRARIES += libcam.hal3a.v3.lsctbl.50
LOCAL_SHARED_LIBRARIES += libcam.hal3a.v3.lscMgr

LOCAL_SHARED_LIBRARIES += libdl
LOCAL_SHARED_LIBRARIES += libcamdrv_tuning_mgr
#LOCAL_SHARED_LIBRARIES += libcamdrv_cq_tuning_mgr
LOCAL_SHARED_LIBRARIES += libcam.utils.sensorprovider
LOCAL_SHARED_LIBRARIES += libui
#capturenr
LOCAL_SHARED_LIBRARIES += libhidlbase
LOCAL_SHARED_LIBRARIES += libhidltransport
LOCAL_SHARED_LIBRARIES += libhwbinder
LOCAL_SHARED_LIBRARIES += android.hardware.power@1.0
LOCAL_SHARED_LIBRARIES += vendor.mediatek.hardware.power@2.0
LOCAL_SHARED_LIBRARIES += libcamalgo.vpunr
#For AE
LOCAL_SHARED_LIBRARIES += lib3a.ae.core
ifeq ($(HAVE_AEE_FEATURE),yes)
    LOCAL_SHARED_LIBRARIES += libaedv
endif
LOCAL_SHARED_LIBRARIES += libmtkcam_tuning_utils
LOCAL_SHARED_LIBRARIES += libmtkcam_mapping_mgr
#
ifeq ($(BUILD_MTK_LDVT),yes)
LOCAL_CFLAGS += -DUSING_MTK_LDVT
endif

#ifneq ($(BUILD_MTK_LDVT),yes)
   #LOCAL_SHARED_LIBRARIES += lib3a
   #LOCAL_SHARED_LIBRARIES += lib3a_core1
   #LOCAL_SHARED_LIBRARIES += lib3a_sample
    LOCAL_SHARED_LIBRARIES += libcameracustom
    LOCAL_SHARED_LIBRARIES += lib3a.af.core
    LOCAL_SHARED_LIBRARIES += lib3a.af
    LOCAL_SHARED_LIBRARIES += lib3a.awb
    LOCAL_SHARED_LIBRARIES += lib3a.awb.core
    LOCAL_SHARED_LIBRARIES += lib3a.flash
    LOCAL_SHARED_LIBRARIES += lib3a.ae
    LOCAL_SHARED_LIBRARIES += lib3a.ae.core
#endif

ifeq ($(MTK_CAM_NEW_NVRAM_SUPPORT),1)
    LOCAL_SHARED_LIBRARIES += libmtkcam_mapping_mgr
endif
LOCAL_HEADER_LIBRARIES += libhardware_headers libandroid_sensor_headers
LOCAL_HEADER_LIBRARIES += libcutils_headers libutils_headers libsystem_headers
LOCAL_HEADER_LIBRARIES += libmtkcam_flickerhal_headers
#-----------------------------------------------------------
#
LOCAL_MODULE := libcam.hal3a.v3
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk

ifeq ($(BUILD_MTK_LDVT),yes)
    LOCAL_CFLAGS += -DUSING_MTK_LDVT
    LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/ldvt/$(TARGET_BOARD_PLATFORM)/include
    LOCAL_C_INCLUDES += $(TOP)/$(MTKCAM_INCLUDE)
    LOCAL_WHOLE_STATIC_LIBRARIES += libuvvf
endif

#-----------------------------------------------------------
LOCAL_INTERMEDIATES         := $(call local-intermediates-dir)
LOCAL_GEN_TOOL              := $(MTKCAM_3A_PATH)/isp_tuning/enum2str
ifeq ($(MTK_CAM_NEW_NVRAM_SUPPORT),1)
    LOCAL_ISP_PROFILE_HEADER    := $(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/tuning_mapping/cam_idx_struct_ext.h
else
    LOCAL_ISP_PROFILE_HEADER    := $(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/isp_tuning/isp_tuning.h
endif
LOCAL_ISP_PROFILE_STR_ARRAY := $(LOCAL_INTERMEDIATES)/EIspProfile_string.h

$(LOCAL_ISP_PROFILE_STR_ARRAY): $(LOCAL_ISP_PROFILE_HEADER) $(LOCAL_GEN_TOOL)
	$(LOCAL_GEN_TOOL) $(LOCAL_ISP_PROFILE_HEADER) EIspProfile_ strEIspProfile > $@ || { rm $@; exit -1; }

LOCAL_GENERATED_SOURCES += $(LOCAL_ISP_PROFILE_STR_ARRAY)
LOCAL_C_INCLUDES += $(LOCAL_INTERMEDIATES)

#-----------------------------------------------------------
ifeq ($(MTK_CAM_NEW_NVRAM_SUPPORT),1)
#LOCAL_INTERMEDIATES         := $(call local-intermediates-dir)
#LOCAL_GEN_TOOL              := $(LOCAL_PATH)/enum2str
LOCAL_MODULE_STR_ARRAY := $(LOCAL_INTERMEDIATES)/EModule_string.h
LOCAL_APP_STR_ARRAY := $(LOCAL_INTERMEDIATES)/EApp_string.h
#LOCAL_ISP_PROFILE_STR_ARRAY := $(LOCAL_INTERMEDIATES)/EIspProfile_string.h
LOCAL_SENSOR_MODE_STR_ARRAY := $(LOCAL_INTERMEDIATES)/ESensorMode_string.h

LOCAL_MODULE_HEADER    := $(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/tuning_mapping/cam_idx_struct_ext.h

$(LOCAL_MODULE_STR_ARRAY): $(LOCAL_MODULE_HEADER) $(LOCAL_GEN_TOOL)
	$(LOCAL_GEN_TOOL) $(LOCAL_MODULE_HEADER) EModule_ strEModule > $@ || { rm $@; exit -1; }
$(LOCAL_APP_STR_ARRAY): $(LOCAL_MODULE_HEADER) $(LOCAL_GEN_TOOL)
	$(LOCAL_GEN_TOOL) $(LOCAL_MODULE_HEADER) EApp_ strEApp > $@ || { rm $@; exit -1; }
#$(LOCAL_ISP_PROFILE_STR_ARRAY): $(LOCAL_MODULE_HEADER) $(LOCAL_GEN_TOOL)
#	$(LOCAL_GEN_TOOL) $(LOCAL_MODULE_HEADER) EIspProfile_ strEIspProfile > $@ || { rm $@; exit -1; }
$(LOCAL_SENSOR_MODE_STR_ARRAY): $(LOCAL_MODULE_HEADER) $(LOCAL_GEN_TOOL)
	$(LOCAL_GEN_TOOL) $(LOCAL_MODULE_HEADER) ESensorMode_ strESensorMode > $@ || { rm $@; exit -1; }

LOCAL_GENERATED_SOURCES += $(LOCAL_MODULE_STR_ARRAY)
LOCAL_GENERATED_SOURCES += $(LOCAL_APP_STR_ARRAY)
#LOCAL_GENERATED_SOURCES += $(LOCAL_ISP_PROFILE_STR_ARRAY)
LOCAL_GENERATED_SOURCES += $(LOCAL_SENSOR_MODE_STR_ARRAY)
#LOCAL_C_INCLUDES += $(LOCAL_INTERMEDIATES)
endif

#------------- Exif Check Sum ----------------------------------------------
LOCAL_MODULE_CLASS    := SHARED_LIBRARIES
INTERMEDIATES         := $(call local-intermediates-dir)
CHKSUM                := $(MTK_PATH_SOURCE)/hardware/mtkcam/aaa/source/common/utils/gen_exif_tag_chksum
CHKSUMEXE             := $(MTK_PATH_SOURCE)/hardware/mtkcam/aaa/source/common/utils/AdlerCheck
MTK_AAA_EXIF_TAG_PATH := $(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/debug_exif/aaa

$(INTERMEDIATES)/aaa_exif_tag_chksum.h: \
 $(CHKSUM) \
 $(CHKSUMEXE) \
 $(MTK_AAA_EXIF_TAG_PATH)/dbg_ae_param.h \
 $(MTK_AAA_EXIF_TAG_PATH)/dbg_af_param.h \
 $(MTK_AAA_EXIF_TAG_PATH)/dbg_awb_param.h \
 $(MTK_AAA_EXIF_TAG_PATH)/dbg_flash_param.h \
 $(MTK_AAA_EXIF_TAG_PATH)/dbg_flicker_param.h \
 $(MTK_AAA_EXIF_TAG_PATH)/dbg_isp_param.h \
 $(MTK_AAA_EXIF_TAG_PATH)/dbg_shading_param.h
	echo "generate $@ from $^"
	rm -rf $@
	$(CHKSUM) $(MTK_AAA_EXIF_TAG_PATH)/dbg_ae_param.h $(CHKSUMEXE)      && \
	$(CHKSUM) $(MTK_AAA_EXIF_TAG_PATH)/dbg_af_param.h $(CHKSUMEXE)      && \
	$(CHKSUM) $(MTK_AAA_EXIF_TAG_PATH)/dbg_awb_param.h $(CHKSUMEXE)     && \
	$(CHKSUM) $(MTK_AAA_EXIF_TAG_PATH)/dbg_flash_param.h $(CHKSUMEXE)   && \
	$(CHKSUM) $(MTK_AAA_EXIF_TAG_PATH)/dbg_flicker_param.h $(CHKSUMEXE) && \
	$(CHKSUM) $(MTK_AAA_EXIF_TAG_PATH)/dbg_isp_param.h $(CHKSUMEXE)     && \
	$(CHKSUM) $(MTK_AAA_EXIF_TAG_PATH)/dbg_shading_param.h $(CHKSUMEXE) || \
	{ >2 echo "generate $@ fail"; exit -1; }
	echo "#define CHKSUM_DBG_AE_PARAM      $(shell $(CHKSUM) $(MTK_AAA_EXIF_TAG_PATH)/dbg_ae_param.h $(CHKSUMEXE))" >> $@
	echo "#define CHKSUM_DBG_AF_PARAM      $(shell $(CHKSUM) $(MTK_AAA_EXIF_TAG_PATH)/dbg_af_param.h $(CHKSUMEXE))" >> $@
	echo "#define CHKSUM_DBG_AWB_PARAM     $(shell $(CHKSUM) $(MTK_AAA_EXIF_TAG_PATH)/dbg_awb_param.h $(CHKSUMEXE))" >> $@
	echo "#define CHKSUM_DBG_FLASH_PARAM   $(shell $(CHKSUM) $(MTK_AAA_EXIF_TAG_PATH)/dbg_flash_param.h $(CHKSUMEXE))" >> $@
	echo "#define CHKSUM_DBG_FLICKER_PARAM $(shell $(CHKSUM) $(MTK_AAA_EXIF_TAG_PATH)/dbg_flicker_param.h $(CHKSUMEXE))" >> $@
	echo "#define CHKSUM_DBG_ISP_PARAM     $(shell $(CHKSUM) $(MTK_AAA_EXIF_TAG_PATH)/dbg_isp_param.h $(CHKSUMEXE))" >> $@
	echo "#define CHKSUM_DBG_SHADING_PARAM $(shell $(CHKSUM) $(MTK_AAA_EXIF_TAG_PATH)/dbg_shading_param.h $(CHKSUMEXE))" >> $@

LOCAL_GENERATED_SOURCES += $(INTERMEDIATES)/aaa_exif_tag_chksum.h
LOCAL_C_INCLUDES += $(INTERMEDIATES)

#--------------------------------------------------------------------------
include $(MTK_SHARED_LIBRARY)
#include $(MTK_STATIC_LIBRARY)

################################################################################
#Build Fake ISP HAL
################################################################################
include $(CLEAR_VARS)

LOCAL_CFLAGS += $(MTKCAM_CFLAGS)

#-----------------------------------------------------------
LOCAL_C_INCLUDES += $(MTKCAM_INCLUDE)
LOCAL_C_INCLUDES += $(MTKCAM_DRV_INCLUDE)
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/include
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/drv/include/$(MTKCAM_DRV_PLATFORM)/drv
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/include/mtkcam
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/aaa/include
LOCAL_C_INCLUDES += $(MTKCAM_3A_COMMON_PATH)/fake_isp_hal

#-----------------------------------------------------------
LOCAL_SRC_FILES += $(RELATIVE_PATH_MTKCAM_3A_COMMON)/fake_isp_hal/IHalISP.cpp
LOCAL_SRC_FILES += $(RELATIVE_PATH_MTKCAM_3A_COMMON)/fake_isp_hal/HalIspImp.cpp

#-----------------------------------------------------------
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libmtkcam_stdutils libmtkcam_imgbuf
LOCAL_SHARED_LIBRARIES += libmtkcam_sysutils
LOCAL_SHARED_LIBRARIES += libmtkcam_hwutils
LOCAL_SHARED_LIBRARIES += libmtkcam_metadata
LOCAL_SHARED_LIBRARIES += libmtkcam_metastore
LOCAL_SHARED_LIBRARIES += libmtkcam_modulehelper
LOCAL_SHARED_LIBRARIES += libmtkcam_ulog

LOCAL_HEADER_LIBRARIES += libhardware_headers
#-----------------------------------------------------------
LOCAL_MODULE := libcam.halisp
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk

include $(MTK_SHARED_LIBRARY)

################################################################################
#
################################################################################
#include $(CLEAR_VARS)
include $(call all-makefiles-under,$(LOCAL_PATH))

