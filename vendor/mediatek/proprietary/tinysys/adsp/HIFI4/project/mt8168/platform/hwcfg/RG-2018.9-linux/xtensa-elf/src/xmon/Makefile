# Makefile for XMON

# Customer ID=13943; Build=0x75f5e; Copyright (c) 2004-2007 Tensilica Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#
# This makefile assumes GNU make features
#

# Select the Xtensa configuration that matches these source files.
XTENSA_ROOT = ../../..
export XTENSA_SYSTEM = $(XTENSA_ROOT)/config
export XTENSA_CORE = default

# Include some general definitions:
CONFIGDIR = $(XTENSA_ROOT)
#include $(CONFIGDIR)/misc/hostenv.mk
include $(CONFIGDIR)/misc/build.mk

XCC = xt-xcc
XOBJCOPY = xt-objcopy
XLD = xt-ld
XAR  = xt-ar
RM = rm -f
RM_R = rm -fr

LIB_DIR = ../../lib
XTTOOLS = $(subst \,/,$(shell xt-run --xtensa-system=$(XTENSA_SYSTEM) \
	    --xtensa-core=$(XTENSA_CORE) --show-config=xttools))

.PHONY: all clean clobber

# Compilation flags
IPATH = -I. -I$(XTTOOLS)/include
CPPFLAGS = $(IPATH)
ASFLAGS = -mlongcalls -x assembler-with-cpp $(DEBUG)
CFLAGS = $(DEBUG)
LDFLAGS = $(LPATH) $(DEBUG)

XMONSECFLAGS =	# -mtext-section-literals

# Supported boards (via XTBSP API)
BOARDS = xtml605 xtkc705
BOARD_LIB   = libxmon-xtbsp.a
BOARD_EXECS = $(BOARDS:%=xmon-%)

ALL_TARGETS =  libxmon-iss.a xmon-iss $(BOARD_LIB) $(BOARD_EXECS) $(BOARD_SRECS)

all: $(ALL_TARGETS)

XMON_OBJS = \
	xmon.o \
	debug-vector-mon.o

%.o: %.c
	$(XCC) -c $(CPPFLAGS) $(CFLAGS) $(XMONSECFLAGS) $<

%.o: %.S
	$(XCC) -c $(CPPFLAGS) $(ASFLAGS) $(XMONSECFLAGS) $<

libxmon-iss.a: $(XMON_OBJS) xmon-iss.o xmon-iss-asm.o
	$(XAR) -rs $@ $^

xmon-iss: xmon-main.o libxmon-iss.a
	$(XCC) -o $@ $(DEBUG) $< libxmon-iss.a -mlsp=sim

$(BOARD_LIB): $(XMON_OBJS) xmon-xtbsp.o
	$(XAR) -rs $@ $^

$(BOARD_EXECS): xmon-% : xmon-main.o $(BOARD_LIB)
	$(XCC) -o $@ $(LDFLAGS) $^ -mlsp=$*-rt

clean:
	-$(RM) $(XMON_OBJS) $(ALL_TARGETS) *.o *.a

clobber: clean
	-$(RM) memmap

# Dependencies (TODO)

