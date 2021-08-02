###################################################################
# Include Files Directery
###################################################################

#include $(D_ROOT)/mtk_cust.mak

###################################################################
# Using GCC
###################################################################

CROSS_COMPILE = arm-linux-androideabi-

AS	= $(CROSS_COMPILE)as
LD	= $(CROSS_COMPILE)ld
CC	= $(CROSS_COMPILE)gcc
CPP	= $(CC)-E
AR	= $(CROSS_COMPILE)ar
NM	= $(CROSS_COMPILE)nm
STRIP	= $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
RANLIB	= $(CROSS_COMPILE)RANLIB
THUMB_MODE = TRUE


###################################################################
# Initialize GCC Compile Parameter
###################################################################
DEFINE           = -D$(MTK_PLATFORM)
OBJCFLAGS 	 = --gap-fill=0xff
AFLAGS_DEBUG 	 = -Wa,-gstabs,
STRIP_SYMBOL	 = -fdata-sections -ffunction-sections

INCLUDE_FILE = $(INCLUDE_FILE_COMMON)
INCLUDE_FILE += \
    -I$(EMIGEN_OUT)/inc \
    -I$(MTK_PATH_CUSTOM)/inc \
    -I$(D_ROOT)/custom/common/inc \
    -I$(D_ROOT)/inc/$(_CHIP) \
    -I$(MTK_ROOT_CUSTOM)/$(TARGET)/common \
    -I$(MTK_ROOT_CUSTOM)/kernel/dct \
    -I$(PTGEN_OUT)/inc

###################################################################
# GCC Compile Options
###################################################################

ifeq ($(CREATE_SEC_LIB),TRUE)

INCLUDE_FILE     +=  \
    -I$(MTK_PATH_PLATFORM)/src/secure_lib/ \
    -I$(MTK_PATH_PLATFORM)/src/secure_lib/inc \
    -I$(MTK_PATH_PLATFORM)/src/secure_lib/crypto \
    -I$(MTK_PATH_PLATFORM)/src/secure_lib/src/platform/$(MTK_PLATFORM)/inc \

# if it's security.lib, we must remove gcc debug message
C_OPTION	 := -gdwarf-2 -Os -fdata-sections -ffunction-sections -fno-strict-aliasing -fno-common -ffixed-r8 -fno-builtin -ffreestanding -pipe -mno-thumb-interwork -Wstrict-prototypes -march=armv7-a $(DEFINE) -c $(INCLUDE_FILE) -msoft-float -D__ASSEMBLY__  -DPRELOADER_HEAP -mno-unaligned-access
C_OPTION_OPTIMIZE	 := -Os -fdata-sections -ffunction-sections -fno-strict-aliasing -fno-common -ffixed-r8 -fno-builtin -ffreestanding -pipe -mno-thumb-interwork -Wstrict-prototypes -march=armv7-a $(DEFINE) -c $(INCLUDE_FILE) -msoft-float -D__ASSEMBLY__  -DPRELOADER_HEAP -mno-unaligned-access
AFLAGS 		 := -c -march=armv7-a -g
AFLAGS_OPTIMIZE	 := -c -march=armv7-a -g

else

C_OPTION	    := -gdwarf-2 -Os $(STRIP_SYMBOL) -fno-strict-aliasing -fno-common -ffixed-r8 -fno-builtin -ffreestanding -pipe -Wstrict-prototypes -march=armv7-a $(DEFINE) -c $(INCLUDE_FILE) -msoft-float -D__ASSEMBLY__ -g -mno-unaligned-access
C_OPTION_OPTIMIZE   := -Os $(STRIP_SYMBOL) -fno-strict-aliasing -fno-common -ffixed-r8 -fno-builtin -ffreestanding -pipe -Wstrict-prototypes -march=armv7-a $(DEFINE) -c $(INCLUDE_FILE) -msoft-float -D__ASSEMBLY__ -g -mno-unaligned-access
AFLAGS 		 := -c -march=armv7-a -g
AFLAGS_OPTIMIZE	 := -c -march=armv7-a -g
endif

ifeq ($(THUMB_MODE),TRUE)
#thumb
C_OPTION            += -mthumb-interwork -mthumb
C_OPTION_OPTIMIZE   += -mthumb-interwork -mthumb
else
C_OPTION	    += -mno-thumb-interwork
C_OPTION_OPTIMIZE   += -mno-thumb-interwork
endif

#priority: mode > project config > cust > default
include $(MTK_PATH_PLATFORM)/default.mak
include ${MTK_ROOT_CUSTOM}/${TARGET}/cust_bldr.mak
PROJECT_CONFIGS := $(MTK_ROOT_CUSTOM)/$(MTK_PROJECT)/$(MTK_PROJECT).mk
include $(PROJECT_CONFIGS)

ifdef PL_MODE
C_OPTION += -D$(PL_MODE)
C_OPTION_OPTIMIZE += -D$(PL_MODE)
include $(MTK_PATH_PLATFORM)/mode/$(PL_MODE).mak
endif

ifeq ("$(MTK_SECURITY_SW_SUPPORT)","yes")
	C_OPTION += -DMTK_SECURITY_SW_SUPPORT
endif

ifeq ($(strip $(TARGET_BUILD_VARIANT)),eng)
    C_OPTION += -DTARGET_BUILD_VARIANT_ENG
endif

ifeq ("$(MTK_EMMC_SUPPORT)","yes")
    C_OPTION += -DMTK_EMMC_SUPPORT
endif

ifeq ("$(MTK_COMBO_NAND_SUPPORT)","yes")
    C_OPTION += -DMTK_COMBO_NAND_SUPPORT
endif

ifeq ("$(MTK_MLC_NAND_SUPPORT)","yes")
    C_OPTION += -DMTK_MLC_NAND_SUPPORT
    C_OPTION += -DMTK_NAND_PAGE_SIZE="8K"
endif

ifeq ("$(MTK_NAND_UBIFS_SUPPORT)","yes")
    C_OPTION += -DMTK_NAND_UBIFS_SUPPORT
endif

ifeq ("$(MTK_TLC_NAND_SUPPORT)","yes")
    C_OPTION += -DMTK_TLC_NAND_SUPPORT
endif

ifeq ("$(MTK_RAWFS_NAND_SUPPORT)","yes")
    C_OPTION += -DMTK_RAWFS_NAND_SUPPORT
endif

include $(MTK_PATH_PLATFORM)/feature.mak

ifeq ("$(MTK_COMBO_NAND_SUPPORT)","yes")
    C_OPTION += -I$(MTK_PATH_PLATFORM)/src/drivers/nandx/include/preloader \
		-I$(MTK_PATH_PLATFORM)/src/drivers/nandx/include/internal
endif

MTK_CDEFS := $(PL_MTK_CDEFS)
MTK_ADEFS := $(PL_MTK_ADEFS)

# make unmark for PL booting time profiling
#C_OPTION += -DPL_PROFILING
C_OPTION += $(MTK_CFLAGS) $(MTK_CDEFS) $(MTK_INC)
AFLAGS   += $(MTK_AFLAGS)

#clean build error when build mt8167_fpga

###################################################################
# gcc link descriptor
###################################################################

ifeq ($(findstring fpga, $(TARGET)),fpga)
LDSCRIPT	:= $(MTK_PATH_PLATFORM)/link_descriptor_fpga.ld
else
LDSCRIPT	:= $(MTK_PATH_PLATFORM)/link_descriptor.ld
endif

LINKFILE	:= $(LD)
LINK		:= $(LINKFILE) -Bstatic -T $(LDSCRIPT) --gc-sections

###################################################################
# Object File
###################################################################

export All_OBJS
