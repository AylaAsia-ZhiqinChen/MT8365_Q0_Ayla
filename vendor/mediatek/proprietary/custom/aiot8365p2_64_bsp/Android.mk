LOCAL_PATH:= $(call my-dir)
ifeq ($(MTK_PROJECT), aiot8365p2_64_bsp)
include $(call all-makefiles-under,$(LOCAL_PATH))
endif
