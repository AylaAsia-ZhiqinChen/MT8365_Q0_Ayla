ifndef ARCH_arm64_TOOLCHAIN_INCLUDED
ARCH_arm64_TOOLCHAIN_INCLUDED := 1

ifndef ARCH_arm64_TOOLCHAIN_PREFIX
ARCH_arm64_TOOLCHAIN_PREFIX := aarch64-elf-
FOUNDTOOL=$(shell which $(ARCH_arm64_TOOLCHAIN_PREFIX)gcc)
ifeq ($(FOUNDTOOL),)
ARCH_arm64_TOOLCHAIN_PREFIX := aarch64-linux-android-
FOUNDTOOL=$(shell which $(ARCH_arm64_TOOLCHAIN_PREFIX)gcc)
ifeq ($(FOUNDTOOL),)
$(error cannot find toolchain, please set ARCH_arm64_TOOLCHAIN_PREFIX or add it to your path)
endif
endif
endif

ARCH_arm64_COMPILEFLAGS := -mgeneral-regs-only -DWITH_NO_FP=1

ifeq ($(call TOBOOL,$(CLANGBUILD)),true)

CLANG_ARM64_TARGET_SYS ?= linux
CLANG_ARM64_TARGET_ABI ?= gnu

CLANG_ARM64_AS_DIR := $(shell dirname $(shell dirname $(ARCH_arm64_TOOLCHAIN_PREFIX)))

AS_PATH := $(wildcard $(CLANG_ARM64_AS_DIR)/*/bin/as)
ifeq ($(AS_PATH),)
$(error Could not find $(CLANG_ARM64_AS_DIR)/*/bin/as, did the directory structure change?)
endif

ARCH_arm64_COMPILEFLAGS += -target aarch64-$(CLANG_ARM64_TARGET_SYS)-$(CLANG_ARM64_TARGET_ABI) \
			   --gcc-toolchain=$(CLANG_ARM64_AS_DIR)/

endif
endif
