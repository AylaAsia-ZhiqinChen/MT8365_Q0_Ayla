###########################################################
## Generic build configurations
###########################################################
include $(BUILD_DIR)/clear_vars.mk

###########################################################
## Create processor-based environment and targets
###########################################################
PLATFORM_BASE_DIR              := $(BASE_DIR)/$(PLATFORM)
PLATFORM_DIR                   := $(PLATFORM_BASE_DIR)/platform
PROJECT_DIR                    := $(PLATFORM_BASE_DIR)/$(CONFIG_DIR)/$(PROJECT)/$(PROCESSOR)
FEATURE_CONFIG_DIR             := $(PLATFORM_DIR)/feature_config
MY_BUILT_DIR                   := $(BUILT_DIR)/$(PROCESSOR)
DRIVERS_PLATFORM_DIR           := $(PLATFORM_DIR)/drivers/$(PROCESSOR)
DRIVERS_PLATFORM_COMMON_DIR    := $(PLATFORM_DIR)/drivers/common
RENAME_SECTION_LIB_DIR         := $(MY_BUILT_DIR)/lib
GENERATED_DIR                  := $(MY_BUILT_DIR)/generated
GENERATED_INCLUDE_DIR          := $(GENERATED_DIR)/include
GENERATED_SYSTEM_LD            := $(GENERATED_DIR)/system.ld
GENERATED_SYSTEM_LD_OVERLAY    := $(GENERATED_DIR)/system.ld.overlay
SYSTEM_LD_TEMPLATE             := $(PLATFORM_DIR)/system.ld.template
SYSTEM_LD_OVERLAY              ?= $(PLATFORM_DIR)/system.ld.overlay
PROJECT_LD                     := $(strip $(wildcard $(PROJECT_DIR)/project.ld))
$(PROCESSOR).TINYSYS_CONFIG_H  := $(GENERATED_INCLUDE_DIR)/tinysys_config.h

PLATFORM_MK                    := $(PLATFORM_DIR)/build/platform.mk
ifeq ($(wildcard $(PLATFORM_MK)),)
  $(error $(TINYSYS_NAME): $(PLATFORM_MK) is missing)
endif

# Reset any previously defined SCP options before including my own definitions
$(foreach v,$(SCP_PREVIOUS_CFG_OPTS), \
  $(eval $(v) := ) \
)
scp_old_cfgs := $(.VARIABLES)

include $(PLATFORM_MK)

ifneq ($(filter HIFI4_%,$(PROCESSOR)),)
  include $(PLATFORM_DIR)/build/config_hifi4.mk
else
  $(error $(TINYSYS_NAME): unsupported processor: $(PROCESSOR))
endif

ifneq (no,$(BUILD_FROM_SOURCE))
scp_new_cfgs := $(.VARIABLES)
SCP_PREVIOUS_CFG_OPTS := \
  $(filter CFG_%,$(filter-out $(scp_old_cfgs),$(scp_new_cfgs)))

INCLUDES += $(GENERATED_INCLUDE_DIR)

# Set dependency
DEPS += $(filter-out %.d,$(MAKEFILE_LIST))

$(OBJS): $($(PROCESSOR).TINYSYS_CONFIG_H)

# Stash the list of configuration names and values to generate config header
CONFIG_MK_FILES := \
  $(PLATFORM_MK) \
  $(wildcard $(PROJECT_DIR)/ProjectConfig.mk)
$(call stash_config_options,$(CONFIG_MK_FILES))

INCLUDES := $(sort $(call normalize-includes,$(INCLUDES)))

###########################################################
## Generic build targets
###########################################################
$(C_OBJS): PRIVATE_CC := $(CC)
$(C_OBJS): PRIVATE_CFLAGS := $(CFLAGS)
$(C_OBJS): PRIVATE_INCLUDES := $(INCLUDES)
$(C_OBJS): $(MY_BUILT_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(compile-c-or-s-to-o)

$(S_OBJS): PRIVATE_CC := $(CC)
$(S_OBJS): PRIVATE_CFLAGS := $(CFLAGS)
$(S_OBJS): PRIVATE_INCLUDES := $(INCLUDES)
$(S_OBJS): $(MY_BUILT_DIR)/%.o: $(SOURCE_DIR)/%.S
	$(compile-c-or-s-to-o)

$(OBJS): $(DEPS)
-include $(OBJS:.o=.d)
endif # BUILD_FROM_SOURCE

# Generate header file that contains all config options and its values
.PHONY: configheader
configheader: $($(PROCESSOR).TINYSYS_CONFIG_H)

$($(PROCESSOR).TINYSYS_CONFIG_H): PRIVATE_PROCESSOR := $(PROCESSOR)
# Let target header file depends on FORCE to ensure that for each make the
# config option updater mechanism kicks off. Target header file will not
# be updated if config option are not changed.
$($(PROCESSOR).TINYSYS_CONFIG_H): FORCE
	$(call gen-tinysys-header,\
		__TINYSYS_CONFIG_H, \
		$($(PRIVATE_PROCESSOR).CONFIG_OPTIONS), \
		$(TINYSYS_NAME) \
	)
