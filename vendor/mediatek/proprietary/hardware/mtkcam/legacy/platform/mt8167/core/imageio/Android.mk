#
# libimageio
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ifeq ($(HAVE_AEE_FEATURE),yes)
    LOCAL_CFLAGS += -DHAVE_AEE_FEATURE
endif

#
LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libcamdrv \
    libm4u \
    libimageio_plat_drv \

LOCAL_SHARED_LIBRARIES += liblog


ifneq ($(BUILD_MTK_LDVT),yes)
ifeq ($(HAVE_AEE_FEATURE),yes)
    LOCAL_SHARED_LIBRARIES += libaedv
endif
endif

ifeq ($(MTK_ION_SUPPORT), yes)
  LOCAL_CFLAGS += -DUSING_MTK_ION
endif

#-----------------------------------------------------------
#
LOCAL_STATIC_LIBRARIES := \
#    libimageio_plat_pipe \
#    libimageio_plat_drv \
#    libimageio_plat_pipe_mgr \

LOCAL_WHOLE_STATIC_LIBRARIES := \
    libimageio_plat_pipe \
#    libimageio_plat_drv \

#
LOCAL_MODULE := libimageio
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
#LOCAL_MULTILIB := 32
#
LOCAL_PRELINK_MODULE := false

#

#

#
# Start of common part ------------------------------------
-include $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/mtkcam.mk

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
include $(MTK_SHARED_LIBRARY)


#
include $(call all-makefiles-under, $(LOCAL_PATH))
