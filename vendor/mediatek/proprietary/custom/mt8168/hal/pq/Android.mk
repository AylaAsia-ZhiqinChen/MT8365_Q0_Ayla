LOCAL_PATH:= $(call my-dir)

ifneq ($(MTK_BASIC_PACKAGE), yes)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    cust_pq_main.cpp \
    cust_tdshp.cpp \
    cust_pqds.cpp \
    cust_pqdc.cpp \
    cust_color.cpp \
    cust_gamma.cpp

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libdpframework \
    libpq_prot

LC_MTK_PLATFORM := $(shell echo $(MTK_PLATFORM) | tr A-Z a-z )

LOCAL_C_INCLUDES += \
    $(TOP)/$(MTK_PATH_SOURCE)/hardware/pq/v2.0/include \
    $(TOP)/$(MTK_PATH_SOURCE)/platform/$(LC_MTK_PLATFORM)/kernel/drivers/dispsys \

ifneq (,$(filter $(strip $(TARGET_BOARD_PLATFORM)), mt6797 mt6799 mt6763 mt6758))
    # Dynamic Sharpenss After 6797
    LOCAL_CFLAGS += -DDYN_SHARP_VERSION=1
else
    # Dynamic Sharpenss Before 6755
    LOCAL_CFLAGS += -DDYN_SHARP_VERSION=0
endif

LOCAL_MODULE:= libpq_cust_base
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_MULTILIB := both

include $(MTK_SHARED_LIBRARY)

#################################

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    cust_pq_main.cpp \
    cust_tdshp.cpp \
    cust_pqds.cpp \
    cust_pqdc.cpp \
    cust_color.cpp \
    cust_gamma.cpp

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libdpframework_mtk \
    libpq_prot_mtk

LC_MTK_PLATFORM := $(shell echo $(MTK_PLATFORM) | tr A-Z a-z )

LOCAL_C_INCLUDES += \
    $(TOP)/$(MTK_PATH_SOURCE)/hardware/pq/v2.0/include \
    $(TOP)/$(MTK_PATH_SOURCE)/platform/$(LC_MTK_PLATFORM)/kernel/drivers/dispsys \

LOCAL_HEADER_LIBRARIES += liblog_headers

ifneq (,$(filter $(strip $(TARGET_BOARD_PLATFORM)), mt6797 mt6799 mt6763 mt6758))
    # Dynamic Sharpenss After 6797
    LOCAL_CFLAGS += -DDYN_SHARP_VERSION=1
else
    # Dynamic Sharpenss Before 6755
    LOCAL_CFLAGS += -DDYN_SHARP_VERSION=0
endif

LOCAL_MODULE:= libpq_cust_mtk_base
LOCAL_MODULE_OWNER := mtk
LOCAL_MULTILIB := both

include $(MTK_SHARED_LIBRARY)

endif
