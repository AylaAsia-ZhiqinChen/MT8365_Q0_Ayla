###########################################################
## Create processor-based environment and targets
###########################################################

BUILD_FROM_SOURCE := no
BUILD_WITH_XTENSA := yes

ifeq (yes,$(CFG_MTK_AUDIODSP_SUPPORT))
ifeq (yes,$(BUILD_WITH_XTENSA))
BUILD_FROM_SOURCE := yes

MY_BIN_STEM              := $(MY_BUILT_DIR)/$(PROCESSOR_LC)
$(PROCESSOR).ELF_FILE    := $(MY_BIN_STEM).elf
$(PROCESSOR).MAP_FILE    := $(MY_BIN_STEM).map
$(PROCESSOR).LOAD_BIN    := $(BUILT_DIR)/hifi4dsp_load.bin
$(PROCESSOR).SIZE_FILE   := $(MY_BIN_STEM).size
$(PROCESSOR).AUTO_VERSION_H := $(GENERATED_INCLUDE_DIR)/auto_version.h

$(PROCESSOR).DRAM_TEXT   := $(MY_BIN_STEM)_dtcm.txt
$(PROCESSOR).DRAM_BIN_NH := $(MY_BIN_STEM)_dtcm_no_mtk_header.bin
$(PROCESSOR).DRAM_BIN    := $(MY_BIN_STEM)_dtcm.bin
$(PROCESSOR).DRAM_HDRCFG := \
  $(MY_BUILT_DIR)/img_hdr_$(notdir $(basename $($(PROCESSOR).DRAM_BIN))).cfg
$(PROCESSOR).IRAM_TEXT   := $(MY_BIN_STEM)_itcm.txt
$(PROCESSOR).IRAM_BIN_NH := $(MY_BIN_STEM)_itcm_no_mtk_header.bin
$(PROCESSOR).IRAM_BIN    := $(MY_BIN_STEM)_itcm.bin
$(PROCESSOR).IRAM_HDRCFG := \
  $(MY_BUILT_DIR)/img_hdr_$(notdir $(basename $($(PROCESSOR).IRAM_BIN))).cfg
$(PROCESSOR).SRAM_TEXT   := $(MY_BIN_STEM)_dram.txt
$(PROCESSOR).SRAM_BIN_NH := $(MY_BIN_STEM)_dram_no_mtk_header.bin
$(PROCESSOR).SRAM_BIN    := $(MY_BIN_STEM)_dram.bin
$(PROCESSOR).SRAM_HDRCFG := \
  $(MY_BUILT_DIR)/img_hdr_$(notdir $(basename $($(PROCESSOR).SRAM_BIN))).cfg

ALL_ADSP_BINS := $(ALL_ADSP_BINS) $($(PROCESSOR).LOAD_BIN)\
  $($(PROCESSOR).DRAM_BIN) $($(PROCESSOR).IRAM_BIN) $($(PROCESSOR).SRAM_BIN)

ifeq (no,$(CFG_HIFI4_DUAL_CORE))
  NUM_LOAD_BINS := 1
else
  NUM_LOAD_BINS := 2
endif # CFG_HIFI4_DUAL_CORE

include $(PLATFORM_DIR)/build/env_hifi4.mk

# toolchain is unavailable but BUILD_WITH_XTENSA is on? Trigger error
ifeq (,$(wildcard $(CC)))
  $(error $(TINYSYS_ADSP): BUILD_WITH_XTENSA is yes, but toolchain $(CC) is not available)
endif

# Make sure license is ready
ifeq (,$(XTENSAD_LICENSE_FILE))
ifeq (,$(shell ls $${HOME}/.flexlmrc))
  $(error $(TINYSYS_ADSP): XTENSAD_LICENSE_FILE is undefined and $${HOME}/.flexlmrc is missing. Please export XTENSAD_LICENSE_FILE in the build command)
endif
endif

INCLUDES += $(GENERATED_INCLUDE_DIR)

BUILT_TIME_STRING := `date "+%Y%m%d%H%M%S"`
$($(PROCESSOR).AUTO_VERSION_H): FORCE
	@mkdir -p $(dir $@)
	@echo "#define BUILT_TIME \"built@$(BUILT_TIME_STRING)\"" > $@

C_OBJS   := $(sort $(C_FILES:$(SOURCE_DIR)/%.c=$(MY_BUILT_DIR)/%.o))
S_OBJS   := $(sort $(S_FILES:$(SOURCE_DIR)/%.S=$(MY_BUILT_DIR)/%.o))
OBJS     += $(sort $(C_OBJS) $(S_OBJS))

TCM_C_OBJS  := $(sort $(TCM_C_FILES:$(SOURCE_DIR)/%.c=$(MY_BUILT_DIR)/%.o))
TCM_S_OBJS  := $(sort $(TCM_S_FILES:$(SOURCE_DIR)/%.S=$(MY_BUILT_DIR)/%.o))
TCM_OBJS    := $(sort $(TCM_C_OBJS) $(TCM_S_OBJS))

DRAM_C_OBJS  := $(sort $(NORMAL_SECTION_C_FILES:$(SOURCE_DIR)/%.c=$(MY_BUILT_DIR)/%.o))
DRAM_OBJS    := $(sort $(DRAM_C_OBJS))
DRAM_LDFLAGS := dram_section_libs

$(OBJS) $(TCM_OBJS) $(DRAM_OBJS): $($(PROCESSOR).TINYSYS_CONFIG_H) $($(PROCESSOR).AUTO_VERSION_H)

CFLAGS += -include $($(PROCESSOR).TINYSYS_CONFIG_H)

INCLUDES := $(sort $(call normalize-includes,$(INCLUDES)))

###########################################################
## Processor-based build targets
###########################################################
$($(PROCESSOR).LOAD_BIN): BIN_PATH := $($(PROCESSOR).LOAD_BIN)
$($(PROCESSOR).LOAD_BIN): BIN_ITCM := $($(PROCESSOR).IRAM_BIN)
$($(PROCESSOR).LOAD_BIN): BIN_DTCM := $($(PROCESSOR).DRAM_BIN)
$($(PROCESSOR).LOAD_BIN): BIN_DRAM := $($(PROCESSOR).SRAM_BIN)
$($(PROCESSOR).LOAD_BIN): ADDR_ITCM := $(CFG_HIFI4_ITCM_CPU_VIEW)
$($(PROCESSOR).LOAD_BIN): ADDR_DTCM := $(CFG_HIFI4_DTCM_CPU_VIEW)
$($(PROCESSOR).LOAD_BIN): ADDR_DRAM := $(CFG_HIFI4_DRAM_DSP_VIEW)
$($(PROCESSOR).LOAD_BIN): ADDR_BOOT := $(CFG_HIFI4_BOOTUP_ADDR_DSP_VIEW)
$($(PROCESSOR).LOAD_BIN): $($(PROCESSOR).IRAM_BIN) $($(PROCESSOR).DRAM_BIN) $($(PROCESSOR).SRAM_BIN)
	@echo '$(TINYSYS_ADSP): BIN   $@'
	$(hide)$(PACK_BIN_NOSIG) $(MY_BUILT_DIR) $(BIN_PATH) $(BIN_ITCM) $(BIN_DTCM) $(BIN_DRAM) $(ADDR_ITCM) $(ADDR_DTCM) $(ADDR_DRAM) $(ADDR_BOOT)
	@mkdir -p $(O)/adsp_images/$(PLATFORM)/$(PROJECT)
	@cp -f $(BIN_PATH) $(O)/adsp_images/$(PLATFORM)/$(PROJECT)
	@cp -f $($(PROCESSOR).ELF_FILE) $(O)/adsp_images/$(PLATFORM)/$(PROJECT)

$($(PROCESSOR).IRAM_BIN): $($(PROCESSOR).ELF_FILE) $($(PROCESSOR).SIZE_FILE)
	@mkdir -p $(dir $@)
	$(hide)$(DUMPBIN) --width=32 --little-endian --sparse --default=0x00000000 --base=$(CFG_HIFI4_ITCM_ADDRESS) --size=$(CFG_HIFI4_ITCM_SIZE) $< > $@

$($(PROCESSOR).DRAM_BIN): $($(PROCESSOR).ELF_FILE) $($(PROCESSOR).SIZE_FILE)
	@mkdir -p $(dir $@)
	$(hide)$(OBJCOPY) $(XT_SYS) -j .dram0.rodata -j .rodata -j .dram0.literal -j .dram0.data -j .data -j .dram0.bss -j .bss -O binary $< $@
#	$(hide)$(DUMPBIN) --width=32 --little-endian --sparse --default=0x00000000 --base=$(CFG_HIFI4_DTCM_ADDRESS) --size=$(CFG_HIFI4_DTCM_SIZE) $< > $@

$($(PROCESSOR).SRAM_BIN): $($(PROCESSOR).ELF_FILE) $($(PROCESSOR).SIZE_FILE)
	@mkdir -p $(dir $@)
	$(hide)$(OBJCOPY) $(XT_SYS) -j .dram.rodata -j .dram.literal -j .dram.text -j .dram.data -j .dram.bss -O binary $< $@
#	$(hide)$(DUMPBIN) --width=32 --little-endian --sparse --default=0x00000000 --base=$(CFG_HIFI4_DRAM_ADDRESS) --size=$(CFG_HIFI4_DRAM_SIZE) $< > $@

$($(PROCESSOR).SIZE_FILE): $($(PROCESSOR).ELF_FILE)
	$(eval OBJ_FILES:=$<)
	$(eval OBJ_FILES+=$(shell find $(dir $<) -name '*.o'))
	@printf "%8s %8s %8s %8s %9s %11s %9s %9s %8s %s\n" "text" "rodata" "data" "bss" "dram.text" "dram.rodata" "dram.data" "dram.bss" "total" "filename" > $@
	@for obj in $(OBJ_FILES); do \
		SECTIONS=`$(OBJDUMP) $(XT_SYS) -h $$obj`; \
		TEXT_SIZE=`echo "$$SECTIONS" | grep -E 'text|literal' | grep -v 'dram' | gawk 'BEGIN {sum=0}{sum+=strtonum("0x"$$3)} END {print sum}'`; \
		RODATA_SIZE=`echo "$$SECTIONS" | grep -w 'rodata' | grep -wv 'dram' | gawk 'BEGIN {sum=0}{sum+=strtonum("0x"$$3)} END {print sum}'`; \
		DATA_SIZE=`echo "$$SECTIONS" | grep -w 'data' | grep -wv 'dram' | gawk 'BEGIN {sum=0}{sum+=strtonum("0x"$$3)} END {print sum}'`; \
		BSS_SIZE=`echo "$$SECTIONS" | grep 'bss' | grep -wv 'dram' | gawk 'BEGIN {sum=0}{sum+=strtonum("0x"$$3)} END {print sum}'`; \
		DRAM_TEXT_SIZE=`echo "$$SECTIONS" | grep -w 'dram' | grep -E 'text|literal' | gawk 'BEGIN {sum=0}{sum+=strtonum("0x"$$3)} END {print sum}'`; \
		DRAM_RODATA_SIZE=`echo "$$SECTIONS" | grep -w 'dram.rodata' | gawk 'BEGIN {sum=0}{sum+=strtonum("0x"$$3)} END {print sum}'`; \
		DRAM_DATA_SIZE=`echo "$$SECTIONS" | grep -w 'dram.data' | gawk 'BEGIN {sum=0}{sum+=strtonum("0x"$$3)} END {print sum}'`; \
		DRAM_BSS_SIZE=`echo "$$SECTIONS" | grep -w 'dram.bss' | gawk 'BEGIN {sum=0}{sum+=strtonum("0x"$$3)} END {print sum}'`; \
		let TOTAL_SIZE=$$TEXT_SIZE+$$RODATA_SIZE+$$DATA_SIZE+$$BSS_SIZE+$$DRAM_TEXT_SIZE+$$DRAM_RODATA_SIZE+$$DRAM_DATA_SIZE+$$DRAM_BSS_SIZE; \
		printf "%8d %8d %8d %8d %9d %11d %9d %9d %8d %s\n" $$TEXT_SIZE $$RODATA_SIZE $$DATA_SIZE $$BSS_SIZE $$DRAM_TEXT_SIZE $$DRAM_RODATA_SIZE $$DRAM_DATA_SIZE $$DRAM_BSS_SIZE $$TOTAL_SIZE $$obj >> $@; \
	done

$($(PROCESSOR).ELF_FILE): PRIVATE_CC := $(CC)
$($(PROCESSOR).ELF_FILE): PRIVATE_INCLUDES := $(INCLUDES)
$($(PROCESSOR).ELF_FILE): PRIVATE_CFLAGS := $(CFLAGS)
$($(PROCESSOR).ELF_FILE): PRIVATE_LDFLAGS := $(LDFLAGS)
$($(PROCESSOR).ELF_FILE): PRIVATE_NORMAL_SECTION_LIBS := $(NORMAL_SECTION_LIBS)
$($(PROCESSOR).ELF_FILE): PRIVATE_RENAME_SECTION_LIB_DIR := $(RENAME_SECTION_LIB_DIR)
$($(PROCESSOR).ELF_FILE): PRIVATE_OBJS := $(TCM_OBJS) $(OBJS) $(DRAM_OBJS)
# FIXME: hard-coded LSP
$($(PROCESSOR).ELF_FILE): PRIVATE_LSP := $(PLATFORM_DIR)/build/lnk-hifi4
$($(PROCESSOR).ELF_FILE): PRIVATE_BUILT_DIR := $(MY_BUILT_DIR)
$($(PROCESSOR).ELF_FILE): PRIVATE_PLATFORM_DIR := $(PLATFORM_DIR)
$($(PROCESSOR).ELF_FILE): PRIVATE_PROJECT_DIR := $(PROJECT_DIR)
$($(PROCESSOR).ELF_FILE): PRIVATE_MAP_FILE := $($(PROCESSOR).MAP_FILE)
$($(PROCESSOR).ELF_FILE): $(TCM_OBJS) $(OBJS) $(DRAM_OBJS) $(DEPS) $(DRAM_LDFLAGS)
	@echo '$(TINYSYS_ADSP): BIN   $@'
	@echo 'LIBFLAGS: $(sort $(LIBFLAGS))'
	@echo 'DRAM_LIBFLAGS: $(sort $(DRAM_LIBFLAGS))'
	@mkdir -p $(dir $@)
ifeq (no,$(CFG_BUILD_CHECK_BIG_FUNCTIONS))
	$(hide)$(PRIVATE_CC) $(PRIVATE_CFLAGS) $(PRIVATE_INCLUDES) -o $@ $(PRIVATE_OBJS) -mlsp=$(PRIVATE_LSP) $(PRIVATE_LDFLAGS) -Wl,--start-group $(sort $(LIBFLAGS)) $(sort $(DRAM_LIBFLAGS)) -Wl,--end-group;
else
	$(hide)$(PRIVATE_CC) $(PRIVATE_CFLAGS) $(PRIVATE_INCLUDES) -o $@ $(PRIVATE_OBJS) -mlsp=$(PRIVATE_LSP) $(PRIVATE_LDFLAGS) -Wl,--start-group $(sort $(LIBFLAGS)) $(sort $(DRAM_LIBFLAGS)) -Wl,--end-group; $(TOOLS_DIR)/hifi4tools/find_libc_big_function.sh $(PRIVATE_MAP_FILE)
endif

$($(PROCESSOR).DRAM_HDRCFG) $($(PROCESSOR).IRAM_HDRCFG) $($(PROCESSOR).SRAM_HDRCFG): $(DEPS)
	$(call gen-image-header,$(TINYSYS_ADSP))

###########################################################
## DRAM specific
###########################################################
# DRAM_CFLAGS must wait for fully-defined CLFAGS to remove *-sections.
DRAM_CFLAGS := $(filter-out -ffunction-sections -fdata-sections,$(CFLAGS))
DRAM_CFLAGS += $(DRAM_RENAME_FLAGS)

$(DRAM_C_OBJS): PRIVATE_CC := $(CC)
$(DRAM_C_OBJS): PRIVATE_CFLAGS := $(DRAM_CFLAGS)
$(DRAM_C_OBJS): PRIVATE_INCLUDES := $(INCLUDES)
$(DRAM_C_OBJS): $(MY_BUILT_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(compile-c-or-s-to-o)

$(DRAM_LDFLAGS): $(NORMAL_SECTION_LIBS)
	@echo 'NORMAL_SECTION_LIBS: $^'
ifeq ($(NORMAL_SECTION_LIBS),)
	$(eval DRAM_LIBFLAGS :=)
else
	$(eval libpath := $(RENAME_SECTION_LIB_DIR))
	@echo $(libpath)
	$(foreach v,$^, \
		$(eval liba := $(notdir $(v))) \
		$(shell mkdir -p $(libpath)) \
		$(shell $(OBJCOPY) $(DRAM_RENAME_LIBFLAGS) $(v) $(libpath)/$(subst .a,_ns.a,$(liba))) \
		$(eval DRAM_LIBFLAGS += -L$(libpath) -l$(subst lib,,$(subst .a,_ns,$(liba)))) \
	)
endif

###########################################################
## TCM specific
###########################################################
# TCM_CFLAGS must wait for fully-defined CLFAGS
#TCM_CFLAGS := $(filter-out -ffunction-sections -fdata-sections,$(CFLAGS))
TCM_CFLAGS := $(CFLAGS)

$(TCM_C_OBJS): PRIVATE_CC := $(CC)
$(TCM_C_OBJS): PRIVATE_CFLAGS := $(TCM_CFLAGS)
$(TCM_C_OBJS): PRIVATE_INCLUDES := $(INCLUDES)
$(TCM_C_OBJS): $(MY_BUILT_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(compile-c-or-s-to-o)

$(TCM_S_OBJS): PRIVATE_CC := $(CC)
$(TCM_S_OBJS): PRIVATE_CFLAGS := $(TCM_CFLAGS)
$(TCM_S_OBJS): PRIVATE_INCLUDES := $(INCLUDES)
$(TCM_S_OBJS): $(MY_BUILT_DIR)/%.o: $(SOURCE_DIR)/%.S
	$(compile-c-or-s-to-o)

-include $(OBJS:.o=.d)
-include $(TCM_OBJS:.o=.d)
-include $(DRAM_OBJS:.o=.d)

else # BUILD_WITH_XTENSA is not enabled. Use default image
$(info $(TINYSYS_ADSP): BUILD_WITH_XTENSA is not enabled. Use default image.)
ADSP_DEFAULT_IMAGE := ../adsp_lib/$(PLATFORM)/$(PLATFORM)_default_adsp.img
ALL_ADSP_BINS := $(ALL_ADSP_BINS) $(ADSP_DEFAULT_IMAGE)
endif # BUILD_WITH_XTENSA
endif # CFG_MTK_AUDIODSP_SUPPORT is enabled
