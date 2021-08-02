### GMS + GMO
ifeq ($(strip $(BUILD_GMS)), yes)

MTK_GMS_CLEAN_MODULES += \
    MtkSimProcessor \
    CallRecorderService \
    MtkPermissionController \
    ExtServices \
    ModuleMetadata \
    ModuleMetadataGoogle \
    PermissionController \
    Baidu_Location \
    liblocSDK6c \
    libnetworklocation

ifeq ($(strip $(MTK_GMO_ROM_OPTIMIZE)), yes)

MTK_GMS_CLEAN_MODULES += \
    PrivacyProtectionLock \
    ppl_agent \
    libdrmctaplugin \
    DataProtection \
    MDMLSample \
    MDMConfig \
    NetworkDataController \
    NetworkDataControllerService \
    AutoBootController

else

MTK_GMS_CLEAN_MODULES += \
    GoogleANGLE \
    ANGLEPrebuilt \
    CaptivePortalLogin \
    DocumentsUI \
    NetworkPermissionConfig \
    NetworkStack

endif

MTK_GMS_CLEAN_FILES := $(strip $(foreach p,$(MTK_GMS_CLEAN_MODULES),$(ALL_MODULES.$(p).INSTALLED)))
$(info [MTK_BUILD_GMS] clean module: $(MTK_GMS_CLEAN_MODULES))

ifneq ($(MTK_GMS_CLEAN_FILES),)
$(info [MTK_BUILD_GMS] rm -rf $(MTK_GMS_CLEAN_FILES))
$(shell rm -rf $(MTK_GMS_CLEAN_FILES))
endif

$(info [MTK_BUILD_GMS] rm -rf $(PRODUCT_OUT)/system/apex)
$(shell rm -rf $(PRODUCT_OUT)/system/apex)
endif
