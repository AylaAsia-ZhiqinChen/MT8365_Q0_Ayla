# <config>/misc/build.mk -- HAL makefile definitions that are dependent on build
#			    or environment configuration
#
#  NOTE: The location and contents of this file are highly subject to change.

#  Customer ID=13943; Build=0x75f5e; Copyright (c) 2002-2017 Tensilica Inc.
#
#  Permission is hereby granted, free of charge, to any person obtaining
#  a copy of this software and associated documentation files (the
#  "Software"), to deal in the Software without restriction, including
#  without limitation the rights to use, copy, modify, merge, publish,
#  distribute, sublicense, and/or sell copies of the Software, and to
#  permit persons to whom the Software is furnished to do so, subject to
#  the following conditions:
#
#  The above copyright notice and this permission notice shall be included
#  in all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
#  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
#  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
#  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
#  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
#  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#  Emulation boards (for XTBOARD, translate h/w names to s/w names):
XPAL_BUILD_XT1000 = 0
XPAL_BUILD_XT2000 = 0
XPAL_BUILD_XTBOARD = 0

#  Third-party and misc software packages:
XPAL_BUILD_VXWORKS = 0
XPAL_BUILD_NUCLEUS = 0
XPAL_BUILD_LINUX   = 0

#  Xtensa Exception Architecture version:
XPAL_XEA = 2

# used by XMON
XPAL_HAVE_DEBUG = 1

#  This config parameter affects use of -mtext-section-literals in Makefiles:
XPAL_HAVE_IMEM_LOADSTORE = 1

#  Presence of ROM in config affects availability of ROMing LSPs:
XPAL_HAVE_SYSROM = 0
XPAL_HAVE_ROM = 0

#  Alphanumeric core identifier (CoreID) set in the Xtensa Processor Generator:
XPAL_CORE_ID = hifi4_Aquila_E2_PROD

