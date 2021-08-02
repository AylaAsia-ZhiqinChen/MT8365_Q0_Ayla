# Copyright (C) 2016 The Android Open Source Project
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

HOST_TEST := storage_test

HOST_SRCS := \
	$(LOCAL_DIR)/../block_allocator.c \
	$(LOCAL_DIR)/../block_cache.c \
	$(LOCAL_DIR)/../block_mac.c \
	$(LOCAL_DIR)/../block_map.c \
	$(LOCAL_DIR)/../block_set.c \
	$(LOCAL_DIR)/../block_tree.c \
	$(LOCAL_DIR)/../crypt.c \
	$(LOCAL_DIR)/../file.c \
	$(LOCAL_DIR)/../super.c \
	$(LOCAL_DIR)/../transaction.c \
	$(LOCAL_DIR)/block_test.c \

HOST_FLAGS := -DBUILD_STORAGE_TEST=1

HOST_LIBS := \
	m \
	crypto \
	ssl \

include make/host_test.mk
