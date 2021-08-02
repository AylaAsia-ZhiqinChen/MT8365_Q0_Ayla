###############################################################################
# This Makefie is a sample to build static library.
# Please copy this file to your own library source code directory.
# Usage: make lib=libname.a
###############################################################################

###############################################################################
# Configure your own toolchain and xtensa system path
###############################################################################
TC := /mtkeda/xtensa/Xplorer-7.0.9/XtDevTools/install/tools/RG-2018.9-linux/XtensaTools
XTENSA_SYSTEM := $(TC)/config
XTENSA_CORE := hifi4_Aquila_E2_PROD

CC := $(TC)/bin/xt-xcc
AR := $(TC)/bin/xt-ar
AS := $(TC)/bin/xt-xcc
STRIP := $(TC)/bin/xt-strip

###############################################################################
# Configure your own source directories, souece files(*.c|*.S|*.s),
# include path and marco definition
###############################################################################
C_FILES := test.c
S_FILES :=
INCLUDES :=
C_DEF :=
SRC_DIRS :=

XT_SYS := --xtensa-system=$(XTENSA_SYSTEM) --xtensa-core=$(XTENSA_CORE)
CFLAGS := $(XT_SYS) -O2 -Os -g -gdwarf-2 -Wall -mno-coproc -mlongcalls -Werror

ifeq ($(strip $(O)),)
O := out
endif

ifeq ($(strip $(lib)),)
lib := libx.a
endif

V ?= 1
ifeq ($(V),1)
hide :=
else
hide := @
endif

VPATH = $(SRC_DIRS)

CFLAGS += $(addprefix -I, $(INCLUDES)) $(addprefix -D, $(C_DEF))
CFLAGS := $(strip $(CFLAGS))
C_OBJS := $(patsubst %.c, %.o, $(C_FILES))
S_OBJS :=  $(patsubst %.S, %.o, $(patsubst %.s, %.o, $(S_FILES)))

.PHONY: all
all: $(lib)

$(lib): $(C_OBJS) $(S_OBJS)
	$(hide)$(AR) r $(O)/$(patsubst %.a,%_debug.a,$@) $(patsubst %.o,$(O)/%.o,$^)
	$(hide)$(STRIP) $(XT_SYS) --strip-unneeded -R .comment -R .xtensa.info $(O)/$(patsubst %.a,%_debug.a,$@) -o $(O)/$@

%.o : %.c
	@mkdir -p $(O)
	$(hide)$(CC) $(CFLAGS) -c $< -o $(O)/$(notdir $@)

%.o : %.S
	@mkdir -p $(O)
	$(hide)$(CC) $(CFLAGS)  -c $< -o $(O)/$(notdir $@)

%.o : %.s
	@mkdir -p $(O)
	$(hide)$(CC) $(CFLAGS)  -c $< -o $(O)/$(notdir $@)

clean:
	@rm -rf $(O)
