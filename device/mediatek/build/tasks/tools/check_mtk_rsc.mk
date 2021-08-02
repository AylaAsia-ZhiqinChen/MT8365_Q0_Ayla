#ifdef MTK_RSC_XML_OPTR.$(MTK_RSC_NAME)\
ifneq ($(MTK_RSC_XML_OPTR.$(MTK_RSC_NAME)),NONE)\
  my_optr := $(word 1,$(subst _,$(space),$(MTK_RSC_XML_OPTR.$(MTK_RSC_NAME))))\
  ifeq ($(filter ro.vendor.operator.optr=$(my_optr),$(MTK_RSC_VENDOR_FINAL_PROPERTIES)),)\
    $(call mtk_rsc_check_fail, ro.vendor.operator.optr=$(my_optr) not set in $(MTK_RSC_NAME) MTK_RSC_VENDOR_PROPERTIES)\
  endif\
  ifeq ($(filter persist.vendor.operator.optr=$(my_optr),$(MTK_RSC_VENDOR_FINAL_PROPERTIES)),)\
    $(call mtk_rsc_check_fail, persist.vendor.operator.optr=$(my_optr) not set in $(MTK_RSC_NAME) MTK_RSC_VENDOR_PROPERTIES)\
  endif\
  ifeq ($(my_optr),OP01)\
    # MTK_RSC_XML_OPTR vs. MTK_RSC_VENDOR_PROPERTIES\
    ifeq ($(filter persist.vendor.mtk_ct_volte_support=0,$(MTK_RSC_VENDOR_FINAL_PROPERTIES)),)\
      $(call mtk_rsc_check_fail, persist.vendor.mtk_ct_volte_support=0 not set in $(MTK_RSC_NAME) MTK_RSC_VENDOR_PROPERTIES)\
    endif\
    ifeq ($(filter persist.vendor.radio.mtk_dsbp_support=0,$(MTK_RSC_VENDOR_FINAL_PROPERTIES)),)\
      $(call mtk_rsc_check_fail, persist.vendor.radio.mtk_dsbp_support=0 not set in $(MTK_RSC_NAME) MTK_RSC_VENDOR_PROPERTIES)\
    endif\
    ifeq ($(filter ro.vendor.mtk_md_sbp_custom_value=1,$(MTK_RSC_VENDOR_FINAL_PROPERTIES)),)\
      $(call mtk_rsc_check_fail, ro.vendor.mtk_md_sbp_custom_value=1 not set in $(MTK_RSC_NAME) MTK_RSC_VENDOR_PROPERTIES)\
    endif\
  endif\
  ifeq ($(my_optr),OP09)\
    ifeq ($(filter persist.vendor.mtk_ct_volte_support=1,$(MTK_RSC_VENDOR_FINAL_PROPERTIES)),)\
      $(call mtk_rsc_check_fail, persist.vendor.mtk_ct_volte_support=1 not set in $(MTK_RSC_NAME) MTK_RSC_VENDOR_PROPERTIES)\
    endif\
  endif\
endif\
endif\
