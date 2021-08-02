# sign image used to get images' sig files

IMAGE_SIGN_TOOL := $(wildcard vendor/mediatek/proprietary/scripts/sign-image_v2/sign_flow.py)
ENV_CFG_FILE := $(wildcard vendor/mediatek/proprietary/scripts/sign-image_v2/env.cfg)

define sign-image-in-list
$(PRODUCT_OUT)/$(2):
$(1): $(PRODUCT_OUT)/$(2) $(3) $(4) $(5)
	@echo sign: $$<
	@if [ -f "$$<" ]; then \
          PYTHONDONTWRITEBYTECODE=True \
          PRODUCT_OUT=$(PRODUCT_OUT) \
          BOARD_AVB_ENABLE=$(BOARD_AVB_ENABLE) \
          python $(4) -target $(2) -env_cfg $(5) $$(MTK_PLATFORM_DIR) $$(MTK_BASE_PROJECT); \
         else \
          echo $$< does not exist, no need to sign!; \
         fi
endef

define sign-image-without-list
$(PRODUCT_OUT)/$(2):
$(1): $(PRODUCT_OUT)/$(2)
	@echo sign: $$<
	$(PERL) vendor/mediatek/proprietary/scripts/sign-image/SignTool.pl $(MTK_BASE_PROJECT) $(MTK_PROJECT_NAME) $(MTK_PATH_CUSTOM) $(MTK_SEC_SECRO_AC_SUPPORT) $(MTK_NAND_PAGE_SIZE) $(BOARD_AVB_ENABLE) $(PRODUCT_OUT) $(2)
endef

ifndef build-signimage-target
ifneq ($(wildcard $(IMAGE_LIST_TXT)),)
define build-signimage-target
	@echo "v2 sign flow"
	PYTHONDONTWRITEBYTECODE=True PRODUCT_OUT=$(PRODUCT_OUT) BOARD_AVB_ENABLE=$(BOARD_AVB_ENABLE) python $(IMAGE_SIGN_TOOL) -env_cfg $(ENV_CFG_FILE) $(MTK_PLATFORM_DIR) $(MTK_BASE_PROJECT)
endef
else
define build-signimage-target
	@echo "v1 sign flow"
	$(PERL) vendor/mediatek/proprietary/scripts/sign-image/SignTool.pl $(MTK_BASE_PROJECT) $(MTK_PROJECT_NAME) $(MTK_PATH_CUSTOM) $(MTK_SEC_SECRO_AC_SUPPORT) $(MTK_NAND_PAGE_SIZE) $(BOARD_AVB_ENABLE) $(PRODUCT_OUT) all
endef
endif
endif


MTK_SPLIT_VENDOR_DTBOIMAGE_TARGET :=


intermediates := $(call intermediates-dir-for,PACKAGING,mtk_signed_image)

ifneq (,$(IMAGE_LIST_TXT))
$(foreach IMAGE_PAIR,$(IMAGE_LIST), \
  $(eval IMAGE_NAME := $(word 1,$(subst =, ,$(IMAGE_PAIR)))) \
  $(if $(filter $(MTK_VERIFIED_BOOT_CURRENT_IMAGES),$(IMAGE_NAME)),\
    $(eval IMAGE_BASE_NAME := $(basename $(IMAGE_NAME))) \
    $(eval SIGN_IMAGE_NAME := $(addsuffix -verified$(suffix $(IMAGE_NAME)),$(IMAGE_BASE_NAME))) \
    $(eval SIGN_IMAGE_FILE := $(addprefix $(PRODUCT_OUT)/,$(SIGN_IMAGE_NAME))) \
    $(eval $(call sign-image-in-list,$(SIGN_IMAGE_FILE),$(IMAGE_NAME),$(IMAGE_LIST_TXT),$(IMAGE_SIGN_TOOL),$(ENV_CFG_FILE))) \
    $(eval .PHONY: $(IMAGE_NAME)) \
    $(eval $(IMAGE_NAME): $(SIGN_IMAGE_FILE)) \
    $(if $(filter dtbo.img odmdtbo.img,$(IMAGE_NAME)),\
      $(eval MTK_SPLIT_VENDOR_DTBOIMAGE_TARGET += $(SIGN_IMAGE_FILE))\
    )\
    $(if $(filter $(ADD_IMG_TO_TARGET_FILES_IMAGES),$(IMAGE_NAME)),,\
      $(eval $(call copy-one-file,$(SIGN_IMAGE_FILE),$(intermediates)/$(IMAGE_NAME)))\
    )\
    $(if $(filter yes,$(BOARD_SIGN_IMG)),\
      $(eval droidcore: $(filter-out $(addprefix $(PRODUCT_OUT)/,boot.img recovery.img),$(SIGN_IMAGE_FILE))) \
    )\
  ) \
)
else
SIGN_IMAGE_TARGET := $(filter %.img logo.bin,$(MTK_SCATTER_ORIGINAL_IMAGES))
$(foreach img,$(SIGN_IMAGE_TARGET), \
  $(eval IMAGE_BASE_NAME := $(basename $(notdir $(img)))) \
  $(eval SIGN_IMAGE_NAME := $(addsuffix -sign$(suffix $(notdir $(img))),$(IMAGE_BASE_NAME))) \
  $(eval SIGN_IMAGE_FILE := $(addprefix $(PRODUCT_OUT)/,$(SIGN_IMAGE_NAME))) \
  $(eval $(call sign-image-without-list,$(SIGN_IMAGE_FILE),$(notdir $(img)))) \
  $(eval .PHONY: $(notdir $(img))) \
  $(eval $(notdir $(img)): $(SIGN_IMAGE_FILE)) \
  $(eval droidcore: $(SIGN_IMAGE_FILE)) \
)
$(foreach IMAGE_NAME,$(MTK_SECURE_DOWNLOAD_CURRENT_IMAGES),\
  $(eval SIGN_IMAGE_NAME := $(basename $(IMAGE_NAME))-sign$(suffix $(IMAGE_NAME)))\
  $(eval SIGN_IMAGE_FILE := $(addprefix $(PRODUCT_OUT)/,$(SIGN_IMAGE_NAME)))\
  $(if $(filter dtbo.img odmdtbo.img,$(IMAGE_NAME)),\
    $(eval MTK_SPLIT_VENDOR_DTBOIMAGE_TARGET += $(SIGN_IMAGE_FILE))\
  )\
  $(if $(filter $(ADD_IMG_TO_TARGET_FILES_IMAGES) odmdtbo.img,$(IMAGE_NAME)),,\
    $(if $(filter $(MTK_VERIFIED_BOOT_CURRENT_IMAGES),$(IMAGE_NAME)),\
      $(eval VERIFIED_IMAGE_NAME := $(basename $(IMAGE_NAME))-verified$(suffix $(IMAGE_NAME)))\
      $(eval VERIFIED_IMAGE_FILE := $(PRODUCT_OUT)/$(VERIFIED_IMAGE_NAME))\
      $(eval $(VERIFIED_IMAGE_FILE): $(SIGN_IMAGE_FILE);)\
      $(eval $(call copy-one-file,$(VERIFIED_IMAGE_FILE),$(intermediates)/$(IMAGE_NAME)))\
    )\
  )\
)
endif

ifdef MTK_TARGET_PROJECT
ifndef SYS_TARGET_PROJECT
ifndef MTK_MODEM_INSTALLED_MODULES
ifeq ($(wildcard vendor/mediatek/proprietary/modem/Android.mk),)
# vendor/mediatek/proprietary/modem may be not ready in customer codebase
$(foreach m,md1img.img md1dsp.img md3img.img,\
  $(eval $(call copy-one-file,device/mediatek/build/build/tools/modem/$(m),$(PRODUCT_OUT)/$(m)))\
)
endif
endif
endif
endif

update_modem_depend_targets := $(INSTALLED_SYSTEMIMAGE_TARGET) $(INSTALLED_VENDORIMAGE_TARGET)
ifeq ($(BOARD_AVB_ENABLE),true)
update_modem_depend_targets += $(INSTALLED_VBMETAIMAGE_TARGET) $(MTK_BOOTIMAGE_TARGET)
endif
ifeq (true,$(BOARD_BUILD_SUPER_IMAGE_BY_DEFAULT))
update_modem_depend_targets += $(INSTALLED_SUPERIMAGE_TARGET)
endif

.PHONY: update-modem
update-modem: $(update_modem_depend_targets)

update_modem_sign :=
ifneq ($(strip $(IMAGE_LIST)),)
update_modem_sign := true
else
INSTALLED_SIGNIMAGE_TARGET := $(wildcard $(PRODUCT_OUT)/*-verified.*) $(wildcard $(PRODUCT_OUT)/*-sign.*)
ifneq ($(strip $(INSTALLED_SIGNIMAGE_TARGET)),)
$(info Found sign-image: $(INSTALLED_SIGNIMAGE_TARGET))
update_modem_sign := true
endif
endif
.PHONY: sign-image sign-image-nodeps
sign-image-nodeps:
	$(call build-signimage-target)

ifeq ($(update_modem_sign),true)
update-modem: sign-image-nodeps
sign-image-nodeps: $(update_modem_depend_targets)
sign-image-nodeps: $(filter-out $(TARGET_OUT)/% $(TARGET_OUT_VENDOR)/%,$(MTK_MODEM_INSTALLED_MODULES))
endif
