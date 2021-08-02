#
# Customer ID=13943; Build=0x75f5e; Copyright (c) 2012 by Tensilica Inc. ALL RIGHTS RESERVED.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

XTENSA_TOOLS_ROOT = $(shell xt-run --show-config=xttools)
include $(XTENSA_TOOLS_ROOT)/misc/defs.mk

# Variables common to both types of file
shell = /bin/sh

CC = xt-xcc
CFLAGS = -O2 -Wall

LIBRARY_SRCS = library.c

# Overlay test code here
LIBRARY_OBJS = $(LIBRARY_SRCS:.c=.o)

fixed_location.lib : $(LIBRARY_OBJS)
	$(CC) $(CFLAGS) -mlsp=./overlaylsp -o $@ $<

packaged_flo.o : fixed_location.lib
	xt-pkg-loadlib -e fixed_location_overlay -o $@ -s stripped_lib.o $^

OVERLAY_TEST_SRCS = overlay_test.c
OVERLAY_TEST_OBJS = $(OVERLAY_TEST_SRCS:.c=.o)

flo_test.exe : packaged_flo.o $(OVERLAY_TEST_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -mlsp=./mainlsp -lloader

test_overlay: flo_test.exe
	xt-run flo_test.exe

# Position Independent Library test code here
PIC_LIBRARY_OBJS = $(LIBRARY_SRCS:.c=.pic.o)

library.pic.o : library.c
	$(CC) $(CFLAGS) -fpic -o $@ -c $^

position_independent.lib : $(PIC_LIBRARY_OBJS)
	$(CC) $(CFLAGS) -mlsp=piload -Wl,--shared-pagesize=128 \
	-Wl,-pie -o $@ $<

packaged_pil.o : position_independent.lib
	xt-pkg-loadlib -p -e pi_library -o $@ $^

PIL_TEST_SRCS = pil_test.c
PIL_TEST_OBJS = $(PIL_TEST_SRCS:.c=.o)

pil_loader.exe : packaged_pil.o $(PIL_TEST_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lloader

test_pil: pil_loader.exe
	xt-run pil_loader.exe

# Position Independent Split Load Library test code here
PICSL_LIBRARY_OBJS = $(LIBRARY_SRCS:.c=.picsl.o)

library.picsl.o : library.c
	$(CC) $(CFLAGS) -fpic -o $@ -c $^

position_independent.splitload.lib : $(PICSL_LIBRARY_OBJS)
	$(CC) $(CFLAGS) -mlsp=pisplitload -Wl,--shared-pagesize=128 \
	-Wl,-pie -o $@ $<

packaged_pilsl.o : position_independent.splitload.lib
	xt-pkg-loadlib -p -e pi_library_sl -o $@ $^

PILSL_TEST_SRCS = pilsl_test.c
PILSL_TEST_OBJS = $(PILSL_TEST_SRCS:.c=.o)

pil_splitloader.exe : packaged_pilsl.o $(PILSL_TEST_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lloader

test_pilsl: pil_splitloader.exe
	xt-run pil_splitloader.exe

# Generic Targets here

clean:
	$(RM) *.o *.lib *~ a.out *.exe

test: test_pil test_pilsl test_overlay

all: 
	@echo nothing to be done for all
