include $(CLEAR_VARS)

LOCAL_MODULE := trustzone$(my_trustzone_suffix)
LOCAL_MODULE_CLASS := ETC
LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := mtk
LOCAL_PATH := $(patsubst %/,%,$(dir $(TRUSTZONE_IMG_PROTECT_CFG)))
LOCAL_MODULE_PATH := $(TRUSTZONE_IMAGE_OUTPUT_PATH)/bin
LOCAL_BUILT_MODULE_STEM := trustzone$(my_trustzone_suffix)
ifeq ($(strip $(MTK_IN_HOUSE_TEE_SUPPORT)),yes)
  LOCAL_MODULE_STEM := tz$(my_trustzone_suffix).img
endif
#LOCAL_UNINSTALLABLE_MODULE := true

include $(BUILD_SYSTEM)/base_rules.mk

ifeq ($(strip $(MTK_ATF_SUPPORT)),yes)
$(LOCAL_BUILT_MODULE): $(patsubst %.img, %$(findstring -ota, $(my_trustzone_suffix)).img, $(ATF_COMP_IMAGE_NAME))
endif
ifeq ($(strip $(MTK_IN_HOUSE_TEE_SUPPORT)),yes)
$(LOCAL_BUILT_MODULE): $(call intermediates-dir-for,ETC,tz$(my_trustzone_suffix))/tz$(my_trustzone_suffix)
endif
ifeq ($(strip $(TRUSTONIC_TEE_SUPPORT)),yes)
$(LOCAL_BUILT_MODULE): $(TEE_COMP_IMAGE_NAME)
endif
ifeq ($(strip $(MICROTRUST_TEE_SUPPORT)),yes)
$(LOCAL_BUILT_MODULE): $(MICROTRUST_COMP_IMAGE_NAME)
endif
ifeq ($(strip $(MTK_GOOGLE_TRUSTY_SUPPORT)),yes)
$(LOCAL_BUILT_MODULE): $(TRUSTY_COMP_IMAGE_NAME)
endif
$(LOCAL_BUILT_MODULE):
	@echo "Trustzone build: $@ <= $^"
	$(hide) mkdir -p $(dir $@)
	$(hide) cat $^ > $@

trustzone_images_to_check := $(trustzone_images_to_check) $(LOCAL_INSTALLED_MODULE)
