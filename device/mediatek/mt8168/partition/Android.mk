ifeq ($(strip $(MTK_PLATFORM)), MT8168)

LOCAL_PATH := $(call my-dir)


ifeq ($(strip $(MTK_DYNAMIC_PARTITION_SUPPORT)), yes)
PARTITION := $(MTK_TARGET_PROJECT_FOLDER)/partition/partition_dynamic.xml
else
PARTITION := $(MTK_TARGET_PROJECT_FOLDER)/partition/partition.xml
endif

ifneq ($(wildcard $(PARTITION_XML)),)
SCATTER_JSON := $(MTK_TARGET_PROJECT_FOLDER)/partition/scatter.json
else
SCATTER_JSON := $(LOCAL_PATH)/scatter.json
ifeq ($(strip $(MTK_DYNAMIC_PARTITION_SUPPORT)), yes)
PARTITION := $(LOCAL_PATH)/partition_dynamic.xml
else
PARTITION := $(LOCAL_PATH)/partition.xml
endif
endif

PARTITION_IMG := $(PRODUCT_OUT)/GPT.img
SCATTER_FILE := $(PRODUCT_OUT)/$(MTK_PLATFORM)_Android_scatter.txt

$(PARTITION_IMG) $(SCATTER_FILE): $(PARTITION)
$(shell device/mediatek/build/build/tools/partition/gen-partition.py $(PARTITION) $(SCATTER_JSON) $(PARTITION_IMG) $(SCATTER_FILE) $(MTK_PLATFORM) $(MTK_TARGET_PROJECT))

MTK_SCATTER_ORIGINAL_IMAGES := $(sort $(filter-out file_name: NONE,$(shell cat $(MTK_SCATTER_FILE_NAME) | grep "file_name:")))

.PHONY: partition
droidcore partition: $(PARTITION_IMG) $(SCATTER_FILE)

endif
