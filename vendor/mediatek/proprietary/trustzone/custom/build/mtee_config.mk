include $(TRUSTZONE_CUSTOM_BUILD_PATH)/common_config.mk

### IN_HOUSE TEE SETTINGS ###
MTEE_ADDITIONAL_DEPENDENCIES := $(TRUSTZONE_PROJECT_MAKEFILE) $(TRUSTZONE_CUSTOM_BUILD_PATH)/common_config.mk $(TRUSTZONE_CUSTOM_BUILD_PATH)/mtee_config.mk $(TRUSTZONE_CUSTOM_BUILD_PATH)/mtee_config_internal.mk

my_trustzone_suffix :=
include $(TRUSTZONE_CUSTOM_BUILD_PATH)/mtee_config_internal.mk
my_trustzone_suffix := -wv
include $(TRUSTZONE_CUSTOM_BUILD_PATH)/mtee_config_internal.mk
