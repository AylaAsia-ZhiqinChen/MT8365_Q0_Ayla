ifneq (true,$(strip $(TARGET_NO_KERNEL)))

ifneq ($(LINUX_KERNEL_VERSION),)

ifeq (,$(KERNEL_OUT))
include $(LINUX_KERNEL_VERSION)/kenv.mk
endif

# Check cross compiler exist
ifeq (,$(wildcard $(TARGET_KERNEL_CROSS_COMPILE)gcc))
    $(error $(TARGET_KERNEL_CROSS_COMPILE) is not existed)
endif

LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_TAGS := optional
LOCAL_MULTILIB := first

# By default install to vendor partition
ifeq (,$(strip $(LOCAL_MODULE_PATH)))
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/lib/modules
endif

ifeq (,$(strip $(LOCAL_SRC_FILES)))
LOCAL_SRC_FILES := $(patsubst $(LOCAL_PATH)/%,%,$(shell find $(LOCAL_PATH) -type f \( -name '*.[choS]' -o -name Kbuild -o -name Makefile \)))
endif

include $(BUILD_SYSTEM)/base_rules.mk

LOCAL_GENERATED_SOURCES := $(addprefix $(intermediates)/LINKED/,$(LOCAL_SRC_FILES))
$(LOCAL_GENERATED_SOURCES): $(intermediates)/LINKED/% : $(LOCAL_PATH)/% | $(ACP)
	@echo "Copy: $@"
	$(copy-file-to-target)

PRIVATE_KERNEL_MODULE_SIGN_KEY := $(KERNEL_MODULE_SIGN_KEY)
ifeq (,$(PRIVATE_KERNEL_MODULE_SIGN_KEY))
PRIVATE_KERNEL_MODULE_SIGN_KEY := $(KERNEL_OUT)/certs/signing_key.pem
endif

ifndef DEPS_$(PRIVATE_KERNEL_MODULE_SIGN_KEY)
$(PRIVATE_KERNEL_MODULE_SIGN_KEY): $(KERNEL_ZIMAGE_OUT);
DEPS_$(PRIVATE_KERNEL_MODULE_SIGN_KEY) := $(KERNEL_ZIMAGE_OUT)
endif
ifndef DEPS_$(KERNEL_OUT)/certs/signing_key.x509
$(KERNEL_OUT)/certs/signing_key.x509: $(KERNEL_ZIMAGE_OUT);
DEPS_$(KERNEL_OUT)/certs/signing_key.x509 := $(KERNEL_ZIMAGE_OUT)
endif
ifndef DEPS_$(KERNEL_OUT)/scripts/sign-file
$(KERNEL_OUT)/scripts/sign-file: $(KERNEL_ZIMAGE_OUT);
DEPS_$(KERNEL_OUT)/scripts/sign-file := $(KERNEL_ZIMAGE_OUT)
endif

# Set for connectivity kernel module Makefile
ifneq (,$(KERNEL_ROOT_OUT))
autoconf_h := $(KERNEL_ROOT_OUT)/include/generated/autoconf.h
else
autoconf_h := $(KERNEL_OUT)/include/generated/autoconf.h
endif

export_symbols := $(intermediates)/LINKED/Module.symvers
import_symbols := $(foreach m,$(LOCAL_REQUIRED_MODULES),$(call intermediates-dir-for,$(LOCAL_MODULE_CLASS),$(m))/LINKED/Module.symvers)

# Build $(intermediates)/LINKED/x.ko
linked_module := $(intermediates)/LINKED/$(LOCAL_MODULE)
$(linked_module): KOUT := $(KERNEL_OUT)
$(linked_module): OPTS := $(KERNEL_MAKE_OPTION) M=$(abspath $(intermediates)/LINKED) AUTOCONF_H=$(autoconf_h) $(if $(import_symbols),KBUILD_EXTRA_SYMBOLS="$(abspath $(import_symbols))")
$(linked_module): .KATI_IMPLICIT_OUTPUTS := $(export_symbols)
$(linked_module): SYMS := $(export_symbols)
$(linked_module): $(import_symbols)
$(linked_module): $(LOCAL_GENERATED_SOURCES) $(KERNEL_ZIMAGE_OUT)
	$(PREBUILT_MAKE_PREFIX)$(MAKE) -C $(KOUT) $(OPTS)
	touch -c $@ $(SYMS)

# Copy linked kernel module to $(PRODUCT_OUT)/symbols
symbolic_input := $(linked_module)
symbolic_output := $(patsubst $(PRODUCT_OUT)/%,$(TARGET_OUT_UNSTRIPPED)/%,$(LOCAL_INSTALLED_MODULE))
$(symbolic_output): $(symbolic_input)
	@echo "target Symbolic: $@"
	$(copy-file-to-target)

# Build $(intermediates)/x.ko
strip_input := $(symbolic_output)
strip_output := $(LOCAL_BUILT_MODULE)
$(strip_output): KOUT := $(KERNEL_OUT)
$(strip_output): CROSS_COMPILE := $(TARGET_KERNEL_CROSS_COMPILE)
$(strip_output): PRIVATE_SIGN_KEY := $(PRIVATE_KERNEL_MODULE_SIGN_KEY)
$(strip_output): $(PRIVATE_KERNEL_MODULE_SIGN_KEY)
$(strip_output): $(KERNEL_OUT)/certs/signing_key.x509
$(strip_output): $(wildcard vendor/mediatek/proprietary/scripts/kernel_tool/rm_ko_sig.py)
$(strip_output): $(strip_input) $(KERNEL_OUT)/scripts/sign-file
	$(copy-file-to-target)
ifneq (false,$(strip $(LOCAL_STRIP_MODULE)))
	@echo "target Stripped: $@"
	$(hide) $(CROSS_COMPILE)strip --strip-unneeded $@
endif
	$(hide) $(call sign-kernel-module,$(KOUT)/scripts/sign-file,$(PRIVATE_SIGN_KEY),$(KOUT)/certs/signing_key.x509)

endif # TARGET_NO_KERNEL != true
endif # LINUX_KERNEL_VERSION
