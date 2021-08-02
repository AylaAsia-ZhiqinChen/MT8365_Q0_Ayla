# Copyright (C) 2014-2015 The Android Open Source Project
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

MODULE := $(LOCAL_DIR)

KEYMASTER_ROOT := $(TRUSTY_TOP)/system/keymaster
PLATFORM_SSL_PATH := $(ANDROIDMAKEROOT)/external/boringssl

MODULE_SRCS += \
	$(KEYMASTER_ROOT)/android_keymaster/android_keymaster.cpp \
	$(KEYMASTER_ROOT)/android_keymaster/android_keymaster_messages.cpp \
	$(KEYMASTER_ROOT)/android_keymaster/android_keymaster_utils.cpp \
	$(KEYMASTER_ROOT)/android_keymaster/keymaster_enforcement.cpp \
	$(KEYMASTER_ROOT)/android_keymaster/logger.cpp \
	$(KEYMASTER_ROOT)/android_keymaster/authorization_set.cpp \
	$(KEYMASTER_ROOT)/android_keymaster/operation.cpp \
	$(KEYMASTER_ROOT)/android_keymaster/operation_table.cpp \
	$(KEYMASTER_ROOT)/android_keymaster/serializable.cpp \
	$(KEYMASTER_ROOT)/android_keymaster/keymaster_stl.cpp \
	$(KEYMASTER_ROOT)/android_keymaster/keymaster_tags.cpp \
	$(KEYMASTER_ROOT)/key_blob_utils/auth_encrypted_key_blob.cpp \
	$(KEYMASTER_ROOT)/key_blob_utils/ocb.c \
	$(KEYMASTER_ROOT)/key_blob_utils/ocb_utils.cpp \
	$(KEYMASTER_ROOT)/km_openssl/aes_key.cpp \
	$(KEYMASTER_ROOT)/km_openssl/aes_operation.cpp \
	$(KEYMASTER_ROOT)/km_openssl/asymmetric_key.cpp \
	$(KEYMASTER_ROOT)/km_openssl/asymmetric_key_factory.cpp \
	$(KEYMASTER_ROOT)/km_openssl/attestation_record.cpp \
	$(KEYMASTER_ROOT)/km_openssl/attestation_utils.cpp \
	$(KEYMASTER_ROOT)/km_openssl/block_cipher_operation.cpp \
	$(KEYMASTER_ROOT)/km_openssl/ckdf.cpp \
	$(KEYMASTER_ROOT)/km_openssl/ec_key.cpp \
	$(KEYMASTER_ROOT)/km_openssl/ec_key_factory.cpp \
	$(KEYMASTER_ROOT)/km_openssl/ecdsa_operation.cpp \
	$(KEYMASTER_ROOT)/km_openssl/hmac_key.cpp \
	$(KEYMASTER_ROOT)/km_openssl/hmac_operation.cpp \
	$(KEYMASTER_ROOT)/km_openssl/openssl_err.cpp \
	$(KEYMASTER_ROOT)/km_openssl/openssl_utils.cpp \
	$(KEYMASTER_ROOT)/km_openssl/rsa_key.cpp \
	$(KEYMASTER_ROOT)/km_openssl/rsa_key_factory.cpp \
	$(KEYMASTER_ROOT)/km_openssl/rsa_operation.cpp \
	$(KEYMASTER_ROOT)/km_openssl/software_random_source.cpp \
	$(KEYMASTER_ROOT)/km_openssl/symmetric_key.cpp \
	$(LOCAL_DIR)/manifest.c \
	$(LOCAL_DIR)/openssl_keymaster_enforcement.cpp \
	$(LOCAL_DIR)/test_attestation_keys.cpp \
	$(LOCAL_DIR)/trusty_keymaster.cpp \
	$(LOCAL_DIR)/trusty_keymaster_context.cpp \
	$(LOCAL_DIR)/trusty_keymaster_enforcement.cpp \
	$(LOCAL_DIR)/secure_storage.cpp

MODULE_INCLUDES := \
	$(KEYMASTER_ROOT)/include \
	$(KEYMASTER_ROOT) \
	$(TRUSTY_TOP)/hardware/libhardware/include \
	$(LOCAL_DIR)

MODULE_CPPFLAGS := -std=c++14 -fno-short-enums

MODULE_COMPILEFLAGS := -U__ANDROID__ -D__TRUSTY__

#
# Defining KEYMASTER_DEBUG will allow configure() to succeed without root of
# trust from bootloader.
#
#MODULE_COMPILEFLAGS += -DKEYMASTER_DEBUG

MODULE_DEPS += \
	app/trusty \
	lib/libc-trusty \
	lib/libstdc++-trusty \
	lib/rng \
	lib/hwkey \
	lib/storage \
	$(PLATFORM_SSL_PATH)

#include $(LOCAL_DIR)/atap/rules.mk
include $(LOCAL_DIR)/ipc/rules.mk

include make/module.mk
