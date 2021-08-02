#
# Copyright (c) 2017, Google, Inc. All rights reserved
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files
# (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge,
# publish, distribute, sublicense, and/or sell copies of the Software,
# and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

# args:
# HOST_TEST : name of the test binary (required)
# HOST_SRCS : list of source files (required)
# HOST_INCLUDE_DIRS : list of include directories
# HOST_FLAGS : list of flags for the compiler
# HOST_LIBS : list of libraries to link against

# Validate arguments.
ifeq ($(HOST_TEST), )
$(error HOST_TEST must be specified)
endif

ifeq ($(HOST_SRCS), )
$(error HOST_SRCS must be specified)
endif

ifeq ($(call TOBOOL,$(CLANGBUILD)),true)
HOST_CC := $(CLANG_BINDIR)/clang
HOST_SANITIZER_FLAGS := -fsanitize=address -fno-omit-frame-pointer
HOST_RUN_ENV := ASAN_OPTIONS=symbolize=1 ASAN_SYMBOLIZER_PATH=$(CLANG_BINDIR)/llvm-symbolizer
else
# TODO: use hermetic version of GCC.
# To do this, we'd need to compile boringssl from source rather than using a system library.
HOST_CC := gcc
HOST_SANITIZER_FLAGS :=
HOST_RUN_ENV :=
endif

HOST_INCLUDE_DIRS += $(GLOBAL_UAPI_INCLUDES) $(GLOBAL_SHARED_INCLUDES) $(GLOBAL_USER_INCLUDES)

# Compile test sources.
GENERIC_CC := $(HOST_CC)
GENERIC_SRCS := $(HOST_SRCS)
GENERIC_OBJ_DIR := $(SAVED_BUILDDIR)/host_tests/obj/$(HOST_TEST)
GENERIC_FLAGS := $(HOST_FLAGS) -O1 -g -Wall -Wextra -Wno-unused-parameter -Werror $(HOST_SANITIZER_FLAGS) $(addprefix -I, $(HOST_INCLUDE_DIRS))
GENERIC_CFLAGS := -std=c11 -D_POSIX_C_SOURCE=199309 -Wno-missing-field-initializers
GENERIC_CPPFLAGS := -std=c++11
include make/generic_compile.mk

# Link
HOST_TEST_BIN := $(SAVED_BUILDDIR)/host_tests/$(HOST_TEST)
$(HOST_TEST_BIN): CC := $(HOST_CC)
$(HOST_TEST_BIN): LDFLAGS := -g $(HOST_SANITIZER_FLAGS) $(addprefix -l, $(HOST_LIBS))
$(HOST_TEST_BIN): $(GENERIC_OBJS)
	@echo linking $@
	@$(MKDIR)
	$(NOECHO)$(CC) $^ $(LDFLAGS) -o $@

# Aliases
host_tests: $(HOST_TEST_BIN)

run_$(HOST_TEST): RUN_ENV := $(HOST_RUN_ENV)
run_$(HOST_TEST): $(HOST_TEST_BIN) .PHONY
	@echo running $<
	$(NOECHO)$(RUN_ENV) gdb -batch -ex run -ex where $<

run_host_tests: run_$(HOST_TEST) .PHONY

# Cleanup inputs
HOST_TEST :=
HOST_SRCS :=
HOST_INCLUDE_DIRS :=
HOST_FLAGS :=
HOST_LIBS :=
# Cleanup internal
HOST_CC :=
HOST_SANITIZER_FLAGS :=
HOST_RUN_ENV :=
HOST_TEST_BIN :=
HOST_OBJ_DIR :=
GENERIC_OBJS :=
