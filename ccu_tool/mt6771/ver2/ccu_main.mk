ifneq ($(wildcard $(LINUX_KERNEL_VERSION)),)
my_ccu_kernel_path ?= $(LINUX_KERNEL_VERSION)
else
my_ccu_kernel_path ?= kernel
endif

#Priority: project > platform > common
ifneq ($(wildcard $(my_ccu_kernel_path)/drivers/misc/mediatek/imgsensor/src/$(MTK_PLATFORM_DIR)/Makefile),)

COMMON_VERSION := $(strip $(shell cat $(my_ccu_kernel_path)/drivers/misc/mediatek/imgsensor/src/$(MTK_PLATFORM_DIR)/Makefile | grep ^COMMON_VERSION | sed -e s/^.*:=/''/g))
sensor_driver_src_project  ?= $(my_ccu_kernel_path)/drivers/misc/mediatek/imgsensor/src/$(MTK_PLATFORM_DIR)/camera_project/$(MTK_TARGET_PROJECT)
sensor_driver_src_platform ?= $(my_ccu_kernel_path)/drivers/misc/mediatek/imgsensor/src/$(MTK_PLATFORM_DIR)
sensor_driver_src_common   ?= $(my_ccu_kernel_path)/drivers/misc/mediatek/imgsensor/src/common/$(COMMON_VERSION)
else
sensor_driver_src_project  =
sensor_driver_src_platform =
sensor_driver_src_common   =
endif
my_ccu_src_path :=

my_ccu_inc_file ?= $(my_ccu_kernel_path)/drivers/misc/mediatek/imgsensor/inc/kd_imgsensor.h

MTK_CCU_TOOLCHAIN_VERSION := arc
#MTK_CCU_TOOLCHAIN_VERSION := ccmd32
my_ccu_binary_mk := $(MTK_PLATFORM_SW_VER_DIR)/ccu_$(strip $(MTK_CCU_TOOLCHAIN_VERSION))_binary.mk

UpperCase = $(subst a,A,$(subst b,B,$(subst c,C,$(subst d,D,$(subst e,E,$(subst f,F,$(subst g,G,$(subst h,H,$(subst i,I,$(subst j,J,$(subst k,K,$(subst l,L,$(subst m,M,$(subst n,N,$(subst o,O,$(subst p,P,$(subst q,Q,$(subst r,R,$(subst s,S,$(subst t,T,$(subst u,U,$(subst v,V,$(subst w,W,$(subst x,X,$(subst y,Y,$(subst z,Z,$(1)))))))))))))))))))))))))))

# max ELF code size in byte for split
MAX_SIZE_PM ?= 4096
MAX_SIZE_DM ?= 2048
MAX_SIZE_ALL ?= 16384

ifneq ($(wildcard $(my_ccu_kernel_path)/drivers/misc/mediatek/imgsensor/inc/kd_imgsensor.h),)

$(foreach m,MTK_TARGET_PROJECT MTK_PLATFORM_DIR,$(if $($(m)),,$(error $(m) is not defined)))
$(foreach m,LOCAL_PATH my_ccu_kernel_path my_ccu_inc_file,$(if $(wildcard $($(m))),,$(error $(m)=$($(m)) does not exist)))

my_ccu_drvname_map := $(shell cat $(my_ccu_inc_file) | grep '^\s*\#define SENSOR_DRVNAME_' | sed -e 's/^\s*\#define SENSOR_DRVNAME_\(\w\+\)\s\+"\(\w\+\)"/\1=libccu_\2/g')
$(foreach m,$(my_ccu_drvname_map),\
	$(eval MY_CCU_DRVNAME_$(m))\
)

$(foreach c,$(CUSTOM_KERNEL_IMGSENSOR),\
	$(eval my_ccu_sensor := $(c))\
	$(if $(BUILD_SYSTEM),$(eval include $(CLEAR_VARS)))\
	$(eval include $(MTK_PLATFORM_SW_VER_DIR)/ccu_extract.mk)\
	$(if $(BUILD_SYSTEM),$(eval include $(my_ccu_binary_mk)))\
)
else

include $(CLEAR_VARS)

LOCAL_MODULE := ccu_tool_nk
LOCAL_MODULE_CLASS := EXECUTABLES
intermediates := $(call intermediates-dir-for,$(LOCAL_MODULE_CLASS),$(LOCAL_MODULE),,,$(if $(TARGET_2ND_ARCH),$(TARGET_2ND_ARCH_VAR_PREFIX)))
ifeq ($(strip $(intermediates)),)
  intermediates := out/target/product/$(MTK_TARGET_PROJECT)/obj_arm/$(LOCAL_MODULE_CLASS)/$(LOCAL_MODULE)_intermediates
endif

$(info alalalala = $(intermediates))

include $(BUILD_SYSTEM)/base_rules.mk
LOCAL_IS_HOST_MODULE :=
LOCAL_UNINSTALLABLE_MODULE := true
LOCAL_MULTILIB := 32

$(LOCAL_BUILT_MODULE) = $(LOCAL_MODULE)
$(LOCAL_INSTALLED_MODULE) = $(LOCAL_MODULE)

LOCAL_MODULE_PATH := $(intermediates)
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
$(LOCAL_BUILT_MODULE):
	$(hide) mkdir -p $(dir $@)
	@touch $@

ALL_DEFAULT_INSTALLED_MODULES += $(LOCAL_INSTALLED_MODULE)

my_ccu_generated_sources += $(LOCAL_MODULE).ddr
endif
