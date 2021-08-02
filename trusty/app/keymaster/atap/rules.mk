# Copyright (C) 2017 The Android Open Source Project
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

CUR_DIR := $(GET_LOCAL_DIR)

ATAP_ROOT := $(TRUSTY_TOP)/system/iot/attestation/atap

MODULE_SRCS += \
  $(ATAP_ROOT)/libatap/atap_commands.c \
  $(ATAP_ROOT)/libatap/atap_sysdeps_posix.c \
  $(ATAP_ROOT)/libatap/atap_util.c \
  $(ATAP_ROOT)/ops/atap_ops_provider.cpp \
  $(ATAP_ROOT)/ops/openssl_ops.cpp \
  $(CUR_DIR)/trusty_atap_ops.cpp

MODULE_INCLUDES += \
  $(ATAP_ROOT) \
  $(CUR_DIR)

CUR_DIR =
