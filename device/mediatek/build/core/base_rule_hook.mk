define base-rules-device-hook
$(eval -include device/mediatek/build/core/base_rules.mk)
endef

define base-rules-hook
$(call base-rules-device-hook)
endef
