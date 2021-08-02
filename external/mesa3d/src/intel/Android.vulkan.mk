# Copyright © 2017 Intel Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
include $(LOCAL_PATH)/Makefile.sources

VULKAN_COMMON_INCLUDES := \
	$(MESA_TOP)/include \
	$(MESA_TOP)/src/mapi \
	$(MESA_TOP)/src/gallium/auxiliary \
	$(MESA_TOP)/src/gallium/include \
	$(MESA_TOP)/src/mesa \
	$(MESA_TOP)/src/vulkan/wsi \
	$(MESA_TOP)/src/vulkan/util \
	$(MESA_TOP)/src/intel \
	$(MESA_TOP)/include/drm-uapi \
	$(MESA_TOP)/src/intel/vulkan \
	frameworks/native/vulkan/include

# libmesa_anv_entrypoints with header and dummy.c
#
# This static library is built to pull entrypoints header
# for multiple gen specific build targets below. The c file
# is generated separately for libmesa_vulkan_common to avoid
# duplicate symbols when linking the anv libraries.

include $(CLEAR_VARS)
LOCAL_MODULE := libmesa_anv_entrypoints
LOCAL_MODULE_CLASS := STATIC_LIBRARIES

intermediates := $(call local-generated-sources-dir)
prebuilt_intermediates := $(MESA_TOP)/prebuilt-intermediates

LOCAL_C_INCLUDES := \
	$(VULKAN_COMMON_INCLUDES)

LOCAL_GENERATED_SOURCES += $(intermediates)/vulkan/anv_entrypoints.h
LOCAL_GENERATED_SOURCES += $(intermediates)/vulkan/dummy.c

$(intermediates)/vulkan/dummy.c:
	@mkdir -p $(dir $@)
	@echo "Gen Dummy: $(PRIVATE_MODULE) <= $(notdir $(@))"
	$(hide) touch $@

$(intermediates)/vulkan/anv_entrypoints.h: $(prebuilt_intermediates)/vulkan/anv_entrypoints.h
	@mkdir -p $(dir $@)
	@cp -f $< $@

LOCAL_EXPORT_C_INCLUDE_DIRS := \
        $(intermediates)

LOCAL_SHARED_LIBRARIES := libdrm

include $(MESA_COMMON_MK)
include $(BUILD_STATIC_LIBRARY)

ANV_INCLUDES := \
	$(VULKAN_COMMON_INCLUDES) \
	$(call generated-sources-dir-for,STATIC_LIBRARIES,libmesa_anv_entrypoints,,)/vulkan \
	$(call generated-sources-dir-for,STATIC_LIBRARIES,libmesa_nir,,)/nir \
	$(call generated-sources-dir-for,STATIC_LIBRARIES,libmesa_vulkan_common,,)/vulkan \
	$(call generated-sources-dir-for,STATIC_LIBRARIES,libmesa_vulkan_util,,)/util

ANV_SHARED_LIBRARIES := libdrm

ifeq ($(filter $(MESA_ANDROID_MAJOR_VERSION), 4 5 6 7),)
ANV_HEADER_LIBRARIES += libcutils_headers libnativebase_headers libsystem_headers
ANV_STATIC_LIBRARIES += libarect
ANV_SHARED_LIBRARIES += libnativewindow
endif

#
# libanv for gen7
#

include $(CLEAR_VARS)
LOCAL_MODULE := libmesa_anv_gen7
LOCAL_MODULE_CLASS := STATIC_LIBRARIES

LOCAL_SRC_FILES := $(VULKAN_GEN7_FILES)
LOCAL_CFLAGS := -DGEN_VERSIONx10=70

LOCAL_C_INCLUDES := $(ANV_INCLUDES)

LOCAL_WHOLE_STATIC_LIBRARIES := libmesa_anv_entrypoints libmesa_genxml

LOCAL_HEADER_LIBRARIES := $(ANV_HEADER_LIBRARIES)
LOCAL_STATIC_LIBRARIES := $(ANV_STATIC_LIBRARIES)
LOCAL_SHARED_LIBRARIES := $(ANV_SHARED_LIBRARIES)

include $(MESA_COMMON_MK)
include $(BUILD_STATIC_LIBRARY)

#
# libanv for gen75
#

include $(CLEAR_VARS)
LOCAL_MODULE := libmesa_anv_gen75
LOCAL_MODULE_CLASS := STATIC_LIBRARIES

LOCAL_SRC_FILES := $(VULKAN_GEN75_FILES)
LOCAL_CFLAGS := -DGEN_VERSIONx10=75

LOCAL_C_INCLUDES := $(ANV_INCLUDES)

LOCAL_WHOLE_STATIC_LIBRARIES := libmesa_anv_entrypoints libmesa_genxml

LOCAL_HEADER_LIBRARIES := $(ANV_HEADER_LIBRARIES)
LOCAL_STATIC_LIBRARIES := $(ANV_STATIC_LIBRARIES)
LOCAL_SHARED_LIBRARIES := $(ANV_SHARED_LIBRARIES)

include $(MESA_COMMON_MK)
include $(BUILD_STATIC_LIBRARY)

#
# libanv for gen8
#

include $(CLEAR_VARS)
LOCAL_MODULE := libmesa_anv_gen8
LOCAL_MODULE_CLASS := STATIC_LIBRARIES

LOCAL_SRC_FILES := $(VULKAN_GEN8_FILES)
LOCAL_CFLAGS := -DGEN_VERSIONx10=80

LOCAL_C_INCLUDES := $(ANV_INCLUDES)

LOCAL_WHOLE_STATIC_LIBRARIES := libmesa_anv_entrypoints libmesa_genxml

LOCAL_HEADER_LIBRARIES := $(ANV_HEADER_LIBRARIES)
LOCAL_STATIC_LIBRARIES := $(ANV_STATIC_LIBRARIES)
LOCAL_SHARED_LIBRARIES := $(ANV_SHARED_LIBRARIES)

include $(MESA_COMMON_MK)
include $(BUILD_STATIC_LIBRARY)

#
# libanv for gen9
#

include $(CLEAR_VARS)
LOCAL_MODULE := libmesa_anv_gen9
LOCAL_MODULE_CLASS := STATIC_LIBRARIES

LOCAL_SRC_FILES := $(VULKAN_GEN9_FILES)
LOCAL_CFLAGS := -DGEN_VERSIONx10=90

LOCAL_C_INCLUDES := $(ANV_INCLUDES)

LOCAL_WHOLE_STATIC_LIBRARIES := libmesa_anv_entrypoints libmesa_genxml

LOCAL_HEADER_LIBRARIES := $(ANV_HEADER_LIBRARIES)
LOCAL_STATIC_LIBRARIES := $(ANV_STATIC_LIBRARIES)
LOCAL_SHARED_LIBRARIES := $(ANV_SHARED_LIBRARIES)

include $(MESA_COMMON_MK)
include $(BUILD_STATIC_LIBRARY)

#
# libanv for gen10
#

include $(CLEAR_VARS)
LOCAL_MODULE := libmesa_anv_gen10
LOCAL_MODULE_CLASS := STATIC_LIBRARIES

LOCAL_SRC_FILES := $(VULKAN_GEN10_FILES)
LOCAL_CFLAGS := -DGEN_VERSIONx10=100

LOCAL_C_INCLUDES := $(ANV_INCLUDES)

LOCAL_WHOLE_STATIC_LIBRARIES := libmesa_anv_entrypoints libmesa_genxml

LOCAL_HEADER_LIBRARIES := $(ANV_HEADER_LIBRARIES)
LOCAL_STATIC_LIBRARIES := $(ANV_STATIC_LIBRARIES)
LOCAL_SHARED_LIBRARIES := $(ANV_SHARED_LIBRARIES)

include $(MESA_COMMON_MK)
include $(BUILD_STATIC_LIBRARY)

#
# libmesa_vulkan_common
#

include $(CLEAR_VARS)
LOCAL_MODULE := libmesa_vulkan_common
LOCAL_MODULE_CLASS := STATIC_LIBRARIES

intermediates := $(call local-generated-sources-dir)
prebuilt_intermediates := $(MESA_TOP)/prebuilt-intermediates

LOCAL_SRC_FILES := $(VULKAN_FILES)

LOCAL_C_INCLUDES := \
	$(ANV_INCLUDES) \
	$(MESA_TOP)/src/compiler

LOCAL_WHOLE_STATIC_LIBRARIES := \
	libmesa_anv_entrypoints \
	libmesa_genxml \
	libmesa_vulkan_util

# The rule generates both C and H files, but due to some strange
# reason generating the files once leads to link-time issues.
# Work around create them here as well - we're safe from race
# conditions since they are stored in another location.

LOCAL_GENERATED_SOURCES += $(intermediates)/vulkan/anv_entrypoints.c
LOCAL_GENERATED_SOURCES += $(intermediates)/vulkan/anv_extensions.c
LOCAL_GENERATED_SOURCES += $(intermediates)/vulkan/anv_extensions.h

$(intermediates)/vulkan/anv_entrypoints.c: $(prebuilt_intermediates)/vulkan/anv_entrypoints.c
	@mkdir -p $(dir $@)
	@cp -f $< $@

$(intermediates)/vulkan/anv_extensions.c: $(prebuilt_intermediates)/vulkan/anv_extensions.c
	@mkdir -p $(dir $@)
	@cp -f $< $@

$(intermediates)/vulkan/anv_extensions.h: $(prebuilt_intermediates)/vulkan/anv_extensions.h
	@mkdir -p $(dir $@)
	@cp -f $< $@

LOCAL_HEADER_LIBRARIES := $(ANV_HEADER_LIBRARIES)
LOCAL_STATIC_LIBRARIES := $(ANV_STATIC_LIBRARIES)
LOCAL_SHARED_LIBRARIES := $(ANV_SHARED_LIBRARIES)

include $(MESA_COMMON_MK)
include $(BUILD_STATIC_LIBRARY)

#
# FIXME: Defining a vulkan HAL for all TARGET_BOARD_PLATFORM, when it can
#        only work for Intel platforms, is just wrong. For now, just omit
#        module unless BOARD_GPU_DRIVERS contains i965. Even this is not
#        correct, but it's difficult to determine what the 'right' list of
#        TARGET_BOARD_PLATFORM to check really are..
#
ifneq ($(findstring i965,$(BOARD_GPU_DRIVERS)),)

#
# libvulkan_intel
#

include $(CLEAR_VARS)

LOCAL_MODULE := vulkan.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := hw

LOCAL_LDFLAGS += -Wl,--build-id=sha1

LOCAL_SRC_FILES := \
	$(VULKAN_GEM_FILES) \
	$(VULKAN_ANDROID_FILES)

LOCAL_C_INCLUDES := \
	$(VULKAN_COMMON_INCLUDES) \
	$(call generated-sources-dir-for,STATIC_LIBRARIES,libmesa_anv_entrypoints,,)/vulkan \
	$(call generated-sources-dir-for,STATIC_LIBRARIES,libmesa_vulkan_common,,)/vulkan

LOCAL_EXPORT_C_INCLUDE_DIRS := $(MESA_TOP)/src/intel/vulkan

LOCAL_WHOLE_STATIC_LIBRARIES := \
	libmesa_nir \
	libmesa_isl \
	libmesa_glsl \
	libmesa_util \
	libmesa_blorp \
	libmesa_compiler \
	libmesa_intel_common \
	libmesa_vulkan_common \
	libmesa_anv_gen7 \
	libmesa_anv_gen75 \
	libmesa_anv_gen8 \
	libmesa_anv_gen9 \
	libmesa_anv_gen10 \
	libmesa_intel_compiler \
	libmesa_anv_entrypoints

LOCAL_HEADER_LIBRARIES := $(ANV_HEADER_LIBRARIES) libhardware_headers
LOCAL_STATIC_LIBRARIES := $(ANV_STATIC_LIBRARIES)
LOCAL_SHARED_LIBRARIES := $(ANV_SHARED_LIBRARIES) libz libsync liblog

include $(MESA_COMMON_MK)
include $(BUILD_SHARED_LIBRARY)

endif # BOARD_GPU_DRIVERS contains 'i965'
