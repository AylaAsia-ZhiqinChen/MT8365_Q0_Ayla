#
# Copyright (c) 2015 MediaTek Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files
# (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge,
# publish, distribute, sublicense, and/or sell copies of the Software,
# and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

ifeq ($(strip $(MTK_GOOGLE_TRUSTY_SUPPORT)),yes)
export TARGET_BOARD_PLATFORM
export MTK_PLATFORM
ifdef TRUSTY_PROJECT
    LOCAL_PATH := $(call my-dir)
    TRUSTY_ROOT_DIR := $(PWD)
    TRUSTY_DIR := $(LOCAL_PATH)/../..

    # export path for clang binary and TRUSTY_TOP
    export CLANG_BINDIR=${TRUSTY_ROOT_DIR}/prebuilts/clang/host/linux-x86/clang-4691093/bin
    export TRUSTY_TOP=$(TRUSTY_ROOT_DIR)

  ifeq ($(wildcard $(TARGET_PREBUILT_TRUSTY)),)
    TARGET_TRUSTY_OUT ?= $(TARGET_OUT_INTERMEDIATES)/TRUSTY_OBJ
    BUILT_TRUSTY_TARGET := $(TARGET_TRUSTY_OUT)/build-$(TRUSTY_PROJECT)/lk.bin
    ifeq ($(TARGET_ARCH), arm64)
      TRUSTY_CROSS_COMPILE_64 ?= $(TRUSTY_ROOT_DIR)/prebuilts/gcc/$(HOST_PREBUILT_TAG)/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android-
      TRUSTY_CROSS_COMPILE_32 ?= $(TRUSTY_ROOT_DIR)/prebuilts/gcc/$(HOST_PREBUILT_TAG)/arm/arm-linux-androideabi-4.9/bin/arm-linux-androideabi-
    else
      TRUSTY_CROSS_COMPILE_32 ?= $(TRUSTY_ROOT_DIR)/prebuilts/gcc/$(HOST_PREBUILT_TAG)/arm/arm-linux-androideabi-4.9/bin/arm-linux-androideabi-
    endif
    TRUSTY_MAKE_OPTION := $(if $(SHOW_COMMANDS),NOECHO=) $(if $(TRUSTY_CROSS_COMPILE_32),ARCH_arm_TOOLCHAIN_PREFIX=$(TRUSTY_CROSS_COMPILE_32)) $(if $(TRUSTY_CROSS_COMPILE_64),ARCH_arm64_TOOLCHAIN_PREFIX=$(TRUSTY_CROSS_COMPILE_64)) LKMAKEROOT=$(abspath $(TRUSTY_DIR)) BUILDROOT=$(abspath $(TARGET_TRUSTY_OUT)) ROOTDIR=$(TRUSTY_ROOT_DIR) ANDROIDMAKEROOT=$(abspath $(TOP))

$(BUILT_TRUSTY_TARGET): FORCE
	@echo Trusty: $@
	$(hide) mkdir -p $(dir $@)
	$(MAKE) -C $(TRUSTY_DIR) -f external/lk/makefile -I vendor/mediatek/proprietary $(TRUSTY_MAKE_OPTION) $(TRUSTY_PROJECT)

  else
    BUILT_TRUSTY_TARGET := $(TARGET_PREBUILT_TRUSTY)
  endif

.PHONY: trusty clean-trusty
trusty: $(BUILT_TRUSTY_TARGET)

clean-trusty:
	$(hide) rm -rf $(BUILT_TRUSTY_TARGET) $(TARGET_TRUSTY_OUT)

# TODO: set dependency with trustzone.bin in vendor/mediatek/proprietary/trustzone/Android.mk
#$(BUILT_TRUSTZONE_TARGET): $(BUILT_TRUSTY_TARGET)
trustzone: $(BUILT_TRUSTY_TARGET)

else
  $(error TRUSTY_PROJECT is not defined when MTK_GOOGLE_TRUSTY_SUPPORT=$(MTK_GOOGLE_TRUSTY_SUPPORT))
endif
endif
