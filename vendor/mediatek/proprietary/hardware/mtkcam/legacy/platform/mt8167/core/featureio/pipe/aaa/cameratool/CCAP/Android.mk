LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

#-----------------------------------------------------------

LOCAL_ARM_MODE := arm
LOCAL_PRELINK_MODULE := false

LOCAL_SRC_FILES:= Meta_CCAP_Para.cpp \
                  meta_ccap_interface.cpp

$(warning "----MTK_MTKCAM_PLATFORM:$(MTK_MTKCAM_PLATFORM)")
$(warning "----MTK_PATH_CUSTOM_PLATFORM:$(MTK_PATH_CUSTOM_PLATFORM)")
$(warning "----MTK_PATH_SOURCE:$(MTK_PATH_SOURCE)")

LOCAL_C_INCLUDES += \
    $(TOP)/$(MTK_PATH_SOURCE)/hardware/include \
    $(TOP)/$(MTK_PATH_SOURCE)/hardware/meta/common/inc \
    $(MTK_MTKCAM_PLATFORM)/inc/acdk \
    $(MTK_MTKCAM_PLATFORM)/acdk/inc/cct \
    $(MTK_MTKCAM_PLATFORM)/acdk/inc/acdk \
    $(MTK_MTKCAM_PLATFORM)/include \
    $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/legacy/include \
    $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/legacy/include/mtkcam \
    $(MTK_PATH_SOURCE)/external/mhal/src/custom/inc \
    $(MTK_PATH_SOURCE)/external/mhal/inc \
    $(MTK_PATH_COMMON)/kernel/imgsensor/inc \
    $(MTK_PATH_CUSTOM)/hal/inc \
    $(MTK_PATH_CUSTOM_PLATFORM)/hal/inc \
    $(TOP)/$(MTK_PATH_CUSTOM)/cgen/cfgfileinc \
    $(TOP)/device/mediatek/sprout/kernel-headers \

#-----------------------------------------------------------
LOCAL_WHOLE_STATIC_LIBRARIES += libacdk_entry_cctif
LOCAL_WHOLE_STATIC_LIBRARIES += libacdk_entry_mdk_cct

#-----------------------------------------------------------
LOCAL_SHARED_LIBRARIES := libcutils libc
#LOCAL_STATIC_LIBRARIES += libft

LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libccci_util

LOCAL_MODULE := libccap
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
#LOCAL_MULTILIB := 32

#-----------------------------------------------------------
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/include
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_PLATFORM)/hardware/include

include $(MTK_SHARED_LIBRARY)
#include $(MTK_STATIC_LIBRARY)

