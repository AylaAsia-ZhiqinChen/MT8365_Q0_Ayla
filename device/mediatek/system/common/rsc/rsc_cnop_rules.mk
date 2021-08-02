
OPTR := $(word 1,$(subst _,$(space),$(OPTR_SPEC_SEG_DEF)))

ifeq (OP01, $(OPTR))
    PRODUCT_MTK_RSC_NAMES += op01
else ifeq (OP02, $(OPTR))
    PRODUCT_MTK_RSC_NAMES += op02
else ifeq (OP09, $(OPTR))
    PRODUCT_MTK_RSC_NAMES += op09
else ifeq (KIT, $(OPTR))
    ifndef MTK_OPERATOR_KIT
        $(error MTK_OPERATOR_KIT must defined when OPTR_SPEC_SEG_DEF defined as KIT)
    else
        ifneq ($(filter OP01%,$(MTK_OPERATOR_KIT)),)
            PRODUCT_MTK_RSC_NAMES += op01
        endif
        ifneq ($(filter OP02%,$(MTK_OPERATOR_KIT)),)
            PRODUCT_MTK_RSC_NAMES += op02
        endif
        ifneq ($(filter OP09%,$(MTK_OPERATOR_KIT)),)
            PRODUCT_MTK_RSC_NAMES += op09
        endif
    endif
endif

