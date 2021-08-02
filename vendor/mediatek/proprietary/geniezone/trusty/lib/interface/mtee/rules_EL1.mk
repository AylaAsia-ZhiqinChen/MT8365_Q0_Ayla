# Copyright (C) 2015 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_DIR := $(GET_LOCAL_DIR)

GLOBAL_INCLUDES += $(LOCAL_DIR)/include/common/include

MODULE := $(LOCAL_DIR)

MODULE_TYPE := trustylib

MODULE_COMPILEFLAGS += -I$(LOCAL_DIR)/include/common/include \
			-Itrusty/lib/lib/mtee_api/include
 
MODULE_SRCS := $(LOCAL_DIR)/src/malloc.c

MODULE_STATIC_LIBS := \
	trusty/lib/lib/mtee_api

#MODULE_DEPS := \
	interface/uuid

include make/module.mk
