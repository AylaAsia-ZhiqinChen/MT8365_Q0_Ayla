# Bluetooth Configuration
ifeq ($(strip $(MTK_BT_SUPPORT)), yes)
ifneq ($(filter MTK_MT76%, $(MTK_BT_CHIP)),)
  PRODUCT_PACKAGES += libbt-vendor
  PRODUCT_PACKAGES += libbluetooth_mtk
  PRODUCT_PACKAGES += boots_srv
  PRODUCT_PACKAGES += boots
  PRODUCT_PACKAGES += picus
  PRODUCT_PACKAGES += btmtksdio.ko
  cfg_folder := vendor/mediatek/proprietary/hardware/connectivity/bluetooth/driver/mt76xx/config
ifeq (MTK_MT7668, $(MTK_BT_CHIP))
ifneq ($(wildcard $(cfg_folder)/mt7668/bt.cfg),)
  PRODUCT_COPY_FILES += $(cfg_folder)/mt7668/bt.cfg:$(TARGET_COPY_OUT_VENDOR)/firmware/bt.cfg:mtk
endif
endif
ifeq (MTK_MT7663, $(MTK_BT_CHIP))
ifneq ($(wildcard $(cfg_folder)/mt7663/bt.cfg),)
  PRODUCT_COPY_FILES += $(cfg_folder)/mt7663/bt.cfg:$(TARGET_COPY_OUT_VENDOR)/firmware/bt.cfg:mtk
endif
endif
else
  PRODUCT_PACKAGES += libbt-vendor
  PRODUCT_PACKAGES += libbluetooth_mtk
  PRODUCT_PACKAGES += libbluetooth_mtk_pure
  PRODUCT_PACKAGES += libbluetoothem_mtk
  PRODUCT_PACKAGES += libbluetooth_relayer
  PRODUCT_PACKAGES += libbluetooth_hw_test
  PRODUCT_PACKAGES += autobt
  PRODUCT_PACKAGES += bt_drv.ko
endif
endif

