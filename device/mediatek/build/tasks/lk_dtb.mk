### LK + DTB
ifdef BOARD_PREBUILT_DTBIMAGE_DIR

MTK_LK_MKIMAGE_TOOL := vendor/mediatek/proprietary/scripts/sign-image_v2/mkimage20/mkimage
MTK_LK_CFG_FILE := $(call intermediates-dir-for,PACKAGING,lk_main_dtb)/img_hdr_lk_dtb.cfg

$(MTK_LK_CFG_FILE):
	$(hide) mkdir -p $(dir $@)
	$(hide) echo "NAME = lk_main_dtb" > $@

MTK_LK_DTB_TARGET := $(call intermediates-dir-for,PACKAGING,lk_main_dtb)/main_dtb_header.bin
INSTALLED_MTK_DTB_TARGET := $(BOARD_PREBUILT_DTBIMAGE_DIR)/mtk_dtb
$(MTK_LK_DTB_TARGET): $(INSTALLED_MTK_DTB_TARGET) $(MTK_LK_MKIMAGE_TOOL) $(MTK_LK_CFG_FILE)
	$(hide) mkdir -p $(dir $@)
	$(hide) $(MTK_LK_MKIMAGE_TOOL) $< $(MTK_LK_CFG_FILE) > $@

$(BUILT_LK_TARGET): $(MTK_LK_DTB_TARGET)

.PHONY: lk
droidcore lk: $(MTK_LK_DTB_TARGET)

endif
