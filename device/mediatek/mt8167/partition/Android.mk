ifeq ($(strip $(MTK_PLATFORM)), MT8167)

LOCAL_PATH := $(call my-dir)

ifneq ($(strip $(MTK_COMBO_NAND_SUPPORT)), yes)
ifeq ($(strip $(MTK_AB_OTA_UPDATER)), yes)
ifeq ($(strip $(MTK_DYNAMIC_PARTITION_SUPPORT)), yes)
PARTITION := $(LOCAL_PATH)/partition_ab_dynamic.xml
else
PARTITION := $(LOCAL_PATH)/partition_ab.xml
endif
else
ifeq ($(strip $(PRODUCT_FULL_TREBLE_OVERRIDE)), false)
PARTITION := $(LOCAL_PATH)/partition_ota.xml
else
ifeq ($(strip $(MTK_DYNAMIC_PARTITION_SUPPORT)), yes)
PARTITION := $(LOCAL_PATH)/partition_dynamic.xml
else
PARTITION := $(LOCAL_PATH)/partition.xml
endif
endif
endif

SCATTER_JSON := $(LOCAL_PATH)/scatter.json
ifeq ($(strip $(MTK_TARGET_PROJECT)), tb8167p3_bsp_p)
PARTITION := $(LOCAL_PATH)/partition_ota.xml
SCATTER_JSON := $(LOCAL_PATH)/scatter_ota.json
endif
ifeq ($(strip $(MTK_TARGET_PROJECT)), tb8167p5_64_bsp_p)
PARTITION := $(LOCAL_PATH)/partition_ab_ota.xml
endif

PARTITION_IMG := $(PRODUCT_OUT)/GPT
SCATTER_FILE := $(PRODUCT_OUT)/$(MTK_PLATFORM)_Android_scatter.txt

$(PARTITION_IMG) $(SCATTER_FILE): $(PARTITION)
$(shell device/mediatek/build/build/tools/partition/gen-partition.py $(PARTITION) $(SCATTER_JSON) $(PARTITION_IMG) $(SCATTER_FILE) $(MTK_PLATFORM) $(MTK_TARGET_PROJECT))
else
PARTITION := $(LOCAL_PATH)/partition_nandx.xml
ifeq ($(strip $(MTK_DYNAMIC_PARTITION_SUPPORT)), yes)
PARTITION := $(LOCAL_PATH)/partition_nandx_dynamic.xml
endif
SCATTER_JSON := $(LOCAL_PATH)/scatter_nandx.json
SCATTER_FILE := $(PRODUCT_OUT)/$(MTK_PLATFORM)_Android_scatter.txt
GPT_XML_FILE := $(PRODUCT_OUT)/partition_nand.xml
GPT_MK_FILE := $(PRODUCT_OUT)/partition_nand.mk

$(SCATTER_FILE) $(GPT_XML_FILE) $(GPT_MK_FILE): $(PARTITION)
$(shell $(LOCAL_PATH)/gen-partition-nandx.py $(PARTITION) $(SCATTER_JSON) $(GPT_XML_FILE) $(SCATTER_FILE) $(MTK_PLATFORM) $(MTK_TARGET_PROJECT) $(GPT_MK_FILE))
endif

MTK_SCATTER_ORIGINAL_IMAGES := $(sort $(filter-out file_name: NONE,$(shell cat $(MTK_SCATTER_FILE_NAME) | grep "file_name:")))

.PHONY: partition
droidcore partition: $(PARTITION_IMG) $(SCATTER_FILE)

endif
