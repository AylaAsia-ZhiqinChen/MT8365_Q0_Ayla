ifdef MTK_RSC_OVERRIDE_APKS

  my_apks := $(filter $(MTK_RSC_OVERRIDE_APKS),$(LOCAL_REQUIRED_MODULES))
  ifdef my_apks
    $(info $(LOCAL_MODULE_MAKEFILE): remove $(my_apks) from LOCAL_REQUIRED_MODULES)
    LOCAL_REQUIRED_MODULES := $(filter-out $(MTK_RSC_OVERRIDE_APKS),$(LOCAL_REQUIRED_MODULES))
  endif
  my_apks :=

  ifeq ($(LOCAL_MODULE_CLASS),APPS)
  ifneq ($(filter $(MTK_RSC_OVERRIDE_APKS),$(LOCAL_PACKAGE_NAME)),)
    #LOCAL_UNINSTALLABLE_MODULE := true
  endif
  endif

endif
