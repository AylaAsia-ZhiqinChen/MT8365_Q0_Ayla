#
# SecureTime
#
ifeq ($(strip $(MTK_IN_HOUSE_TEE_SUPPORT)),yes)
ifneq ($(filter-out no, $(MTK_DRM_PLAYREADY_SUPPORT) $(MTK_WVDRM_L1_SUPPORT)), )

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS +=                 \
    -Wall -Wextra -Werror       \
    -Wno-error=unused-parameter \
    -DDebugSecureTime=0         \

LOCAL_SRC_FILES +=              \
    main_securetime.cpp         \
    TEESecureClock.cpp          \

LOCAL_C_INCLUDES +=                                                  \
    $(MTK_PATH_SOURCE)/external/trustzone/mtee/include               \

LOCAL_SHARED_LIBRARIES +=       \
    liblog                      \
    libutils                    \
    libtz_uree                  \

LOCAL_MODULE := securetime
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_MULTILIB := 32
LOCAL_MODULE_TAGS := optional

LOCAL_INIT_RC := securetime.rc

include $(MTK_EXECUTABLE)

endif
endif
