keys :=
ifdef SYS_TARGET_PROJECT_FOLDER
ifneq ($(wildcard $(SYS_TARGET_PROJECT_FOLDER)/SystemConfig.mk),)
sub_keys := $(strip $(subst =, ,$(shell grep -o "^\s*\w\+\s*=" $(SYS_TARGET_PROJECT_FOLDER)/SystemConfig.mk)))
keys += $(sub_keys)
endif
endif
ifdef MTK_TARGET_PROJECT_FOLDER
ifneq ($(wildcard $(MTK_TARGET_PROJECT_FOLDER)/ProjectConfig.mk),)
sub_keys := $(strip $(subst =, ,$(shell grep -o "^\s*\w\+\s*=" $(MTK_TARGET_PROJECT_FOLDER)/ProjectConfig.mk)))
keys += $(sub_keys)
endif
endif
ifneq ($(wildcard device/mediatek/build/core/mssi_fo.mk),)
ifdef FO_NEEDED_DEFINE_MSSI_LIST
sub_keys := $(strip $(addprefix MSSI_,$(FO_NEEDED_DEFINE_MSSI_LIST)))
keys += $(sub_keys)
endif
endif

keys += \
	HAVE_AEE_FEATURE \
	MSSI_HAVE_AEE_FEATURE \
	MTK_BASE_PROJECT \
	MTK_TARGET_PROJECT \
	TARGET_BOARD_PLATFORM \
	TARGET_BRM_PLATFORM

keys += \
	SYS_BASE_PROJECT \
	SYS_TARGET_PROJECT

keys += \
	PRODUCT_LOCALES \
	PRODUCTS \
	TARGET_ARCH \
	TARGET_BUILD_VARIANT

keys += \
	VSYNC_EVENT_PHASE_OFFSET_NS

ifeq ($(filter mtkPlugin,$(SOONG_CONFIG_NAMESPACES)),)
SOONG_CONFIG_NAMESPACES += mtkPlugin
endif
SOONG_CONFIG_mtkPlugin := $(keys)
$(foreach key,$(keys),$(eval SOONG_CONFIG_mtkPlugin_$(key):=$($(key))))

# Include kernel build makefile
ifdef LINUX_KERNEL_VERSION
ifneq ($(PLATFORM_VERSION), R)
LLVM_PREBUILTS_PATH := prebuilts/clang/host/linux-x86/clang-r353983c/bin
else
LLVM_PREBUILTS_PATH := prebuilts/clang/host/linux-x86/clang-r365631/bin
endif
-include $(LINUX_KERNEL_VERSION)/kenv.mk
endif

