ifeq (yes,$(strip $(MTK_GMO_RAM_OPTIMIZE)))
    #A-GO
    MALLOC_SVELTE := true
    ifeq ($(HOST_OS),linux)
        TARGET_USERIMAGES_USE_F2FS := true
    endif
    #Config data partition for F2FS
    BOARD_USERDATAIMAGE_FILE_SYSTEM_TYPE := f2fs
    # Filesystem management tools
    PRODUCT_PACKAGES += fsck.f2fs mkfs.f2fs
endif
