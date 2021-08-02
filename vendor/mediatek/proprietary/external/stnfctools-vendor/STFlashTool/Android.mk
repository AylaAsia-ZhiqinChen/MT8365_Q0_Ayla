# Include the correct firmware depending on the chip
# they are always called st21nfc_fw.bin for simplicity of the config file.
ifeq ($(strip $(NFC_CHIP_SUPPORT)), yes)
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := st21nfcd_firmware
LOCAL_MODULE_STEM := st21nfc_fw.bin
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/firmware/
LOCAL_SRC_FILES:= firmware/st21nfc_fw.bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := st54j_firmware
LOCAL_MODULE_STEM := st54j_fw.bin
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/firmware/
LOCAL_SRC_FILES:= firmware/st54j_fw.bin
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PREBUILT)
endif