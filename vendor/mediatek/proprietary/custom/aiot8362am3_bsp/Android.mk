LOCAL_PATH:= $(call my-dir)
ifeq ($(MTK_PROJECT), aiot8362am3_bsp)
include $(call all-makefiles-under,$(LOCAL_PATH))
endif
