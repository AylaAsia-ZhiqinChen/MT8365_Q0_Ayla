ifeq ($(strip $(MTK_TARGET_PROJECT)), aiot8365p2_64_bsp)

LOCAL_PATH := $(call my-dir)

ifeq ($(strip $(MTK_DYNAMIC_PARTITION_SUPPORT)), yes)
PARTITION := $(LOCAL_PATH)/partition_dynamic.xml
else
PARTITION := $(LOCAL_PATH)/partition.xml
endif
SCATTER_JSON := $(LOCAL_PATH)/scatter.json
PARTITION_IMG := $(PRODUCT_OUT)/GPT.img
SCATTER_FILE := $(PRODUCT_OUT)/$(MTK_PLATFORM)_Android_scatter.txt

$(PARTITION_IMG) $(SCATTER_FILE): $(PARTITION)
	$(hide) MTK_AB_OTA_UPDATER=$(MTK_AB_OTA_UPDATER) MTK_IN_HOUSE_TEE_SUPPORT=$(MTK_IN_HOUSE_TEE_SUPPORT) device/mediatek/build/build/tools/partition/gen-partition.py $< $(SCATTER_JSON) $(PARTITION_IMG) $(SCATTER_FILE) $(MTK_PLATFORM) $(MTK_TARGET_PROJECT)

.PHONY: partition
droidcore partition: $(PARTITION_IMG) $(SCATTER_FILE)

endif
