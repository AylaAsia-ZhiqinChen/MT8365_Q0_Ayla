LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE:=pcm_merge_common_includes

LOCAL_EXPORT_C_INCLUDE_DIRS:=$(LOCAL_PATH)\
        $(LOCAL_PATH)/msb/inc \
        $(LOCAL_PATH)/msrc/inc \

include $(BUILD_HEADER_LIBRARY)

include $(CLEAR_VARS)

LOCAL_CFLAGS := -Wall -g -fPIC
LOCAL_LDFLAGS :=  -Wall -g -fPIC
LOCAL_SRC_FILES := \
          msb/core/msb_mgr.c
LOCAL_MODULE := libmsb
LOCAL_SHARED_LIBRARIES :=
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_MULTILIB := both

include $(MTK_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := \
          $(LOCAL_PATH)/msrc/inc \
          $(MTK_PATH_SOURCE)/external/blisrc/blisrc32

LOCAL_CFLAGS := -Wall -g -fPIC
LOCAL_LDFLAGS :=  -Wall -g -fPIC  -llog 
LOCAL_SRC_FILES := \
          msrc/src/msrc_mgr.c \
          msrc/src/msrc_simple.c \
          msrc/src/msrc_blisrc.c

LOCAL_MODULE := libmsrc
LOCAL_SHARED_LIBRARIES := libblisrc32_vendor
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_MULTILIB := both

include $(MTK_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := \
          external/tinyalsa/include \

LOCAL_CFLAGS := -Wall -g -fPIC
LOCAL_LDFLAGS :=  -Wall -g -fPIC -llog
LOCAL_SRC_FILES := \
          pcm_merge/src/pcm_bits_convert.c \
          pcm_merge/src/pcm_hal.c \
          pcm_merge/src/pcm_hal_alsa_lib.c \
          pcm_merge/src/pcm_hal_tiny_alsa.c \
          pcm_merge/src/pcm_share_buffer.c \
          pcm_merge/src/pcm_merge.c

LOCAL_MODULE := libpcm_merge
LOCAL_SHARED_LIBRARIES := libmsb libmsrc libtinyalsa libutils
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_MULTILIB := both

include $(MTK_SHARED_LIBRARY)
