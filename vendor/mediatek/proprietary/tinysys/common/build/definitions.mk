QUOTE   := "
SQUOTE  := '
PERCENT := %
COMMA   := ,

###########################################################
## Get processor and platform names by project path.
## Return $(PROCESSOR):$(PLATFORM) string.
## Project path format: $(PROCESSOR)/$(PLATFORM)/$(PROJECT)
###########################################################

# $(1): path of project directory
define get_processor_and_platform
$(eval my_path_words := $(subst /, ,$(1))) \
$(if $(filter 3,$(words $(my_path_words))), \
  $(word 1,$(my_path_words)):$(word 2,$(my_path_words)), \
  $(error Project path layout ($(1)) is incorrect) \
)
endef

###########################################################
## Get processor(s) by platform path.
## Return $(PROCESSOR) string.
## Platform path format: $(PROCESSOR)/$(PLATFORM)
###########################################################

# $(1): path of platform directory
define get_processors_from_platform_paths
$(foreach p,$(1),\
  $(eval my_path_words := $(subst /, ,$(p))) \
  $(word 1,$(my_path_words)) \
) \
$(eval my_path_words :=)
endef

###########################################################
## Create the variable $(PROCESSOR).CONFIG_OPTIONS that
## contains $(CFG_NAME)=$($(CFG_NAME)) pairs from
## given configuration file list for each processor.
##
## The purpose is to collect all config options from given
## config files, which belong to different processors, and
## stash their final values.
###########################################################

# $1: list of configuration files that registers config options
define stash_config_options
$(eval _vars :=) \
$(foreach f,$(strip $(1)), \
  $(eval _vars += $(shell sed -nr 's#^[[:space:]]*(CFG_[0-9A-Z_]+)[ \t]*[:+?]?=.*#\1#gp' $(f))) \
) \
$(foreach v,$(_vars), \
  $(if $($(v)),,$(eval $(v) := no)) \
  $(eval $(v) := $(subst $(QUOTE),\$(QUOTE),$($(v)))) \
  $(eval $(v) := $(subst $(SQUOTE),\$(SQUOTE),$($(v)))) \
  $(eval $(PROCESSOR).CONFIG_OPTIONS += $(strip $(v))=$(strip $($(v)))) \
) \
$(eval $(PROCESSOR).CONFIG_OPTIONS := $(sort $($(PROCESSOR).CONFIG_OPTIONS))) \
$(eval _vars :=)
endef

###########################################################
## Template for creating image header
###########################################################

# $1: identification string to be displayed in any output
define gen-image-header
	@echo '$(1): GEN     $@'
	@mkdir -p $(dir $@)
	$(hide)echo 'NAME = $(patsubst img_hdr_%.cfg,%,$(notdir $@))' > $@
endef

###########################################################
## Template for compiling C and S files to objects
###########################################################

define compile-c-or-s-to-o
	@echo '$(PRIVATE_BUILD_TAG): CC      $<'
	@mkdir -p $(dir $@)
	$(hide)$(PRIVATE_CC) $(PRIVATE_CFLAGS) $(PRIVATE_LIBFLAGS) \
		$(PRIVATE_INCLUDES) -MD -MP -MF $(patsubst %.o,%.d,$@) -c $< -o $@
endef

###########################################################
## Template for compiling C and S files to objects without
## .d files
###########################################################

define compile-c-or-s-to-o-without-d
	@echo '$(PRIVATE_BUILD_TAG): CC      $<'
	@mkdir -p $(dir $@)
	$(hide)$(PRIVATE_CC) $(PRIVATE_CFLAGS) $(PRIVATE_LIBFLAGS) \
		$(PRIVATE_INCLUDES) -c $< -o $@
endef

###########################################################
## Template for creating configuration header
###########################################################

# $1: wrapper macro name, e.g. __TINYSYS_CONFIG_H
# $2: config options in KEY=VALUE format, separated with spaces
# $3: identification string to be displayed in any output, such as TINYSYS_SCP
# $4: if true, redefine __FILE__ as __FILENAME__
define gen-tinysys-header
	@echo '$(strip $(3)): GEN     $@'
	@mkdir -p $(dir $@)
	$(hide)rm -f $(@).tmp
	@echo '/*' > $(@).tmp; \
	echo ' * Automatically generated file; DO NOT EDIT.' >> $(@).tmp; \
	echo ' */' >> $(@).tmp; \
	echo '#ifndef $(strip $(1))' >> $(@).tmp; \
	echo -e '#define $(strip $(1))\n' >> $(@).tmp; \
	if [ 'true' = $4 ]; then \
		echo -e '#undef __FILE__' >> $(@).tmp; \
		echo -e '#define __FILE__ __FILENAME__\n' >> $(@).tmp; \
	fi; \
	for i in $(2); do \
		KEY="$${i//=*}"; \
		VAL="$${i##*=}"; \
		if [ "$${VAL}" = 'yes' ]; then \
			echo "#define $${KEY}" >> $(@).tmp; \
		elif [ "$${VAL}" = 'no' ]; then \
			echo "/* $${KEY} is not set */" >> $(@).tmp; \
		else \
			echo "#define $${KEY} $${VAL}" >> $(@).tmp; \
		fi; \
	done; \
	echo -e '\n#endif /* $(strip $(1)) */' >> $(@).tmp; \
	if [ -f '$@' ]; then \
		if cmp -s '$@' '$(@).tmp'; then \
			rm '$(@).tmp'; \
			echo '$(3): $(@) is update to date.'; \
		else \
			mv '$(@).tmp' '$@'; \
			echo '$(strip $(3)): Updated $@'; \
		fi; \
	else \
		mv '$(@).tmp' '$@'; \
	fi
endef

###########################################################
## Add prefix -I to given include directories that lack it
###########################################################

# $1: config option name
define normalize-includes
$(patsubst %,-I%,$(patsubst -I%,%,$(1)))
endef
