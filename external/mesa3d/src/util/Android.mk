# Mesa 3-D graphics library
#
# Copyright (C) 2014 Tomasz Figa <tomasz.figa@gmail.com>
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

include $(LOCAL_PATH)/Makefile.sources

# ---------------------------------------
# Build libmesa_util
# ---------------------------------------

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	$(MESA_UTIL_FILES) \
	$(XMLCONFIG_FILES)

LOCAL_C_INCLUDES := \
	external/zlib \
	$(MESA_TOP)/src/mesa \
	$(MESA_TOP)/src/mapi \
	$(MESA_TOP)/src/gallium/include \
	$(MESA_TOP)/src/gallium/auxiliary

LOCAL_SHARED_LIBRARIES := \
	libexpat

LOCAL_MODULE := libmesa_util

# Generated sources

LOCAL_MODULE_CLASS := STATIC_LIBRARIES

prebuilt_intermediates := $(MESA_TOP)/prebuilt-intermediates
intermediates := $(call local-generated-sources-dir)

LOCAL_EXPORT_C_INCLUDE_DIRS := $(intermediates)

UTIL_GENERATED_SOURCES := $(addprefix $(intermediates)/,$(MESA_UTIL_GENERATED_FILES))
LOCAL_GENERATED_SOURCES := $(UTIL_GENERATED_SOURCES)

$(intermediates)/xmlpool/options.h: $(prebuilt_intermediates)/xmlpool/options.h
	@mkdir -p $(dir $@)
	@cp -f $< $@

MESA_DRI_OPTIONS_H := $(intermediates)/xmlpool/options.h
LOCAL_GENERATED_SOURCES := $(MESA_DRI_OPTIONS_H)

$(intermediates)/format_srgb.c: $(prebuilt_intermediates)/util/format_srgb.c
	@mkdir -p $(dir $@)
	@cp -f $< $@

MESA_FORMAT_SRGB_C := $(intermediates)/format_srgb.c
LOCAL_GENERATED_SOURCES += $(MESA_FORMAT_SRGB_C)

include $(MESA_COMMON_MK)
include $(BUILD_STATIC_LIBRARY)
