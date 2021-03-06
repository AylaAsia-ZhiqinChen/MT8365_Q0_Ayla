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
# This is the shared library included by the JNI test app.
#

LOCAL_PATH:= $(call my-dir)

define jvmti-attaching-test-agent
include $(CLEAR_VARS)

LOCAL_MODULE := libjvmtiattachingtestagent$1

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := agent.c

LOCAL_C_INCLUDES := $(JNI_H_INCLUDE)

LOCAL_SDK_VERSION := current

LOCAL_CFLAGS := -Wall -Werror -Wno-unused-parameter

LOCAL_CFLAGS += -DAGENT_NR=$1

include $(BUILD_SHARED_LIBRARY)
endef

$(eval $(call jvmti-attaching-test-agent,1))
$(eval $(call jvmti-attaching-test-agent,2))
$(eval $(call jvmti-attaching-test-agent,3))
$(eval $(call jvmti-attaching-test-agent,4))
