LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE:=msrc_includes

LOCAL_EXPORT_C_INCLUDE_DIRS:=$(LOCAL_PATH)

include $(BUILD_HEADER_LIBRARY)

