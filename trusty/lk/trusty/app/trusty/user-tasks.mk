#
# Copyright (c) 2014, Google, Inc. All rights reserved
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

$(info Include Trusty user tasks support)

TRUSTY_APP_DIR := $(GET_LOCAL_DIR)

#
# Input variables
#
#   TRUSTY_PREBUILT_USER_TASKS - list of precompiled user tasks to be included into final image
#   TRUSTY_ALL_USER_TASKS      - list of compiled from source user tasks to be included into final image
#

# generate user task build rule: $(1): user task
define user-task-build-rule
$(eval XBIN_NAME := $(notdir $(1)))\
$(eval XBIN_TOP_MODULE := $(1))\
$(eval XBIN_TYPE := USER_TASK)\
$(eval XBIN_ARCH := $(TRUSTY_USER_ARCH))\
$(eval XBIN_BUILDDIR := $(BUILDDIR)/user_tasks/$(1))\
$(eval XBIN_LINKER_SCRIPT := $(XBIN_BUILDDIR)/user_task.ld)\
$(eval XBIN_LDFLAGS := )\
$(eval XBIN_ALIGNMENT := 4096)\
$(eval include make/xbin.mk)
endef

# generate list of all user tasks we need to build

# sort and remove duplicates
TRUSTY_USER_TASKS := $(sort $(TRUSTY_USER_TASKS))

ALLUSER_TASK_ELFS := $(foreach t, $(TRUSTY_ALL_USER_TASKS),\
   $(addsuffix /$(notdir $(t)).elf, $(t)))

ALLUSER_TASK_ELFS := $(addprefix $(BUILDDIR)/user_tasks/,$(ALLUSER_TASK_ELFS))

# Add prebuilt user tasks
ALLUSER_TASK_ELFS += $(TRUSTY_PREBUILT_USER_TASKS)

#
# Generate build rules for each user task
#
$(foreach t,$(TRUSTY_ALL_USER_TASKS),\
   $(call user-task-build-rule,$(t)))

#
# Generate combined user task obj/bin if necessary
#
ifneq ($(strip $(ALLUSER_TASK_ELFS)),)

ALLUSER_TASK_OBJS := $(patsubst %.elf,%.o,$(ALLUSER_TASK_ELFS))

$(ALLUSER_TASK_OBJS): CC := $(CC)
$(ALLUSER_TASK_OBJS): GLOBAL_COMPILEFLAGS := $(GLOBAL_COMPILEFLAGS)
$(ALLUSER_TASK_OBJS): ARCH_COMPILEFLAGS := $(ARCH_$(ARCH)_COMPILEFLAGS)
$(ALLUSER_TASK_OBJS): USER_TASK_OBJ_ASM:=$(TRUSTY_APP_DIR)/appobj.S
$(ALLUSER_TASK_OBJS): %.o: %.elf $(USER_TASK_OBJ_ASM)
	@$(MKDIR)
	@echo converting $< to $@
	$(NOECHO)$(CC) -DUSER_TASK_ELF=\"$<\" $(GLOBAL_COMPILEFLAGS) $(ARCH_COMPILEFLAGS) -c $(USER_TASK_OBJ_ASM) -o $@

EXTRA_OBJS += $(ALLUSER_TASK_OBJS)

endif

