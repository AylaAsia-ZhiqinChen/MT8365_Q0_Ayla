###########################################################
## Obtain processor and platform name by project name
###########################################################
my_project_paths := $(strip $(shell find $(BASE_DIR) \
  -name ProjectConfig.mk -printf '%P\n' | grep $(PROJECT)))
ifeq ($(my_project_paths),)
  $(error $(TINYSYS_NAME): Cannot find project $(PROJECT) under $(BASE_DIR))
endif

my_processor_and_platform_names :=
$(foreach p,$(my_project_paths), \
  $(eval my_processor_and_platform_names += \
    $(call get_processor_and_platform,$(p))) \
)
$(foreach i,$(my_processor_and_platform_names), \
  $(eval PROCESSORS += $(strip $(word 2,$(subst :, ,$(i))))) \
  $(eval PLATFORM   += $(strip $(word 1,$(subst :, ,$(i))))) \
  $(eval CONFIG_DIR += $(strip $(word 3,$(subst :, ,$(i))))) \
)

PROCESSORS := $(sort $(PROCESSORS))
PLATFORM   := $(sort $(PLATFORM))
ifneq (1,$(words $(PLATFORM)))
  $(error $(TINYSYS_NAME): $(PROJECT) is found in platforms [$(PLATFORM)], but one project cannot belong to multiple platforms. Please choose different project names for different platform.)
endif

ifneq (1,$(words $(PROCESSORS)))
  MANDATORY_PROJECT_LD := true
endif

CONFIG_DIR := $(sort $(CONFIG_DIR))
ifneq (1,$(words $(CONFIG_DIR)))
  $(error $(TINYSYS_NAME): $(PROJECT) is found in multi-config dir [$(CONFIG_DIR)].)
endif

###########################################################
## Initialize environment and targets for each processor
###########################################################
$(foreach p,$(PROCESSORS), \
  $(eval PROCESSOR := $(p)) \
  $(eval PROCESSOR_LC := $(shell echo '$(p)' | tr 'A-Z' 'a-z')) \
  $(eval include $(BUILD_DIR)/config_common.mk) \
)

my_project_path :=
my_project :=
my_processor_and_platform_names :=
