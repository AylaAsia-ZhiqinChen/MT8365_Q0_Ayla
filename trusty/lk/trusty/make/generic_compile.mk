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
# GENERIC_CC : path to compiler (required)
# GENERIC_SRCS : list of source files (required)
# GENERIC_OBJ_DIR : a directory to hold the created .o files (required)
# GENERIC_FLAGS : list of flags for the compiler
# GENERIC_CFLAGS : list of flags for the compiler, when compiling C files.
# GENERIC_CPPFLAGS : list of flags for the compiler, when compiling C++ files.

# Validate arguments.
ifeq ($(GENERIC_CC), )
$(error GENERIC_CC must be specified)
endif

ifeq ($(GENERIC_SRCS), )
$(error GENERIC_SRCS must be specified)
endif

ifeq ($(GENERIC_OBJ_DIR), )
$(error GENERIC_OBJ_DIR must be specified)
endif

# Group the source files so we can differ the flags between C and C++.
GENERIC_C_SRCS := $(filter %.c,$(GENERIC_SRCS))
GENERIC_C_OBJS := $(addprefix $(GENERIC_OBJ_DIR)/,$(patsubst %.c,%.o,$(GENERIC_C_SRCS)))

GENERIC_CC_SRCS := $(filter %.cc,$(GENERIC_SRCS))
GENERIC_CC_OBJS := $(addprefix $(GENERIC_OBJ_DIR)/,$(patsubst %.cc,%.o,$(GENERIC_CC_SRCS)))

GENERIC_CPP_SRCS := $(filter %.cpp,$(GENERIC_SRCS))
GENERIC_CPP_OBJS := $(addprefix $(GENERIC_OBJ_DIR)/,$(patsubst %.cpp,%.o,$(GENERIC_CPP_SRCS)))

GENERIC_OBJS := $(strip $(GENERIC_C_OBJS) $(GENERIC_CC_OBJS) $(GENERIC_CPP_OBJS))

# Bind inputs.
$(GENERIC_OBJS): CC := $(GENERIC_CC)
$(GENERIC_OBJS): FLAGS := $(GENERIC_FLAGS)
$(GENERIC_OBJS): CFLAGS := $(GENERIC_CFLAGS)
$(GENERIC_OBJS): CPPFLAGS := $(GENERIC_CPPFLAGS)

$(GENERIC_C_OBJS): $(GENERIC_OBJ_DIR)/%.o: %.c
	@echo building $@
	@$(MKDIR)
	$(NOECHO)$(CC) $(FLAGS) $(CFLAGS) -c $< -MMD -o $@

$(GENERIC_CC_OBJS): $(GENERIC_OBJ_DIR)/%.o: %.cc
	@echo building $@
	@$(MKDIR)
	$(NOECHO)$(CC) $(FLAGS) $(CPPFLAGS) -c $< -MMD -o $@

$(GENERIC_CPP_OBJS): $(GENERIC_OBJ_DIR)/%.o: %.cpp
	@echo building $@
	@$(MKDIR)
	$(NOECHO)$(CC) $(FLAGS) $(CPPFLAGS) -c $< -MMD -o $@

# Ensure recompilation on header file change.
-include $(GENERIC_OBJS:.o=.d)

# Cleanup inputs
GENERIC_CC :=
GENERIC_SRCS :=
GENERIC_OBJ_DIR :=
GENERIC_FLAGS :=
GENERIC_CFLAGS :=
GENERIC_CPPFLAGS :=
# Cleanup internal
GENERIC_C_SRCS :=
GENERIC_C_OBJS :=
GENERIC_CC_SRCS :=
GENERIC_CC_OBJS :=
GENERIC_CPP_SRCS :=
GENERIC_CPP_OBJS :=

# GENERIC_OBJS is returned.
