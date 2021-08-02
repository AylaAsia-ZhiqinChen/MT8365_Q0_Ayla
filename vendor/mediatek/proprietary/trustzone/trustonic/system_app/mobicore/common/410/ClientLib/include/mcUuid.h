/*
 * Copyright (c) 2013-2018 TRUSTONIC LIMITED
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the TRUSTONIC LIMITED nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MC_UUID_H_
#define MC_UUID_H_

#ifdef WIN32
#define _UNUSED
#else
#define _UNUSED __attribute__((unused))
#endif

#define UUID_TYPE

#define UUID_LENGTH 16
/** Universally Unique Identifier (UUID) according to ISO/IEC 11578. */
typedef struct {
    uint8_t value[UUID_LENGTH]; /**< Value of the UUID. */
} mcUuid_t, *mcUuid_ptr;

/** UUID value used as free marker in service provider containers. */
#define MC_UUID_FREE_DEFINE \
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }

static _UNUSED const mcUuid_t MC_UUID_FREE = {
    MC_UUID_FREE_DEFINE
};

/** Reserved UUID. */
#define MC_UUID_RESERVED_DEFINE \
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

static _UNUSED const mcUuid_t MC_UUID_RESERVED = {
    MC_UUID_RESERVED_DEFINE
};

/** UUID for system applications. */
#define MC_UUID_SYSTEM_DEFINE \
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE }

static _UNUSED const mcUuid_t MC_UUID_SYSTEM = {
    MC_UUID_SYSTEM_DEFINE
};

#define MC_UUID_RTM_DEFINE \
    { 0x12, 0x34, 0x12, 0x34, 0x12, 0x34, 0x12, 0x34,       \
      0x12, 0x34, 0x12, 0x34, 0x12, 0x34, 0x12, 0x34 }

static _UNUSED const mcUuid_t MC_UUID_RTM = {
    MC_UUID_RTM_DEFINE
};

/** [INTERNAL]
 * TODO: Replace with v5 UUID (milestone #3) [/INTERNAL]
 */
#define LTA_UUID_DEFINE \
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,         \
      0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11}

/**
 * Monotonic counter TA for RPMB
 */
#define TA_MONOTONIC_COUNTER_UUID \
	{ 0x07, 0x05, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00,          \
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22}

/**
 * RPMB Driver UUID
 */
#define DRV_RPMB_UUID        \
    { 0x07, 0x05, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00,          \
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21 }

/**
 * Secure Storage Driver UUID
 */
#define DRV_SFS_UUID        \
    { 0x07, 0x05, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00,          \
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20 }

/**
 * SFS Proxy TA UUID
 */
#define TA_SFS_PROXY_UUID   \
    { 0x07, 0x05, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00,           \
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20 }

/**
 * Gatekeeper TA UUID
 */
#define TA_GATEKEEPER_UUID \
    { 0x07, 0x06, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, \
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

/**
 * Key Injection TA UUID
 */
#define KEY_INJECTION_TA_UUID \
    { 0x10, 0xC9, 0xCD, 0x73, 0xB8, 0x9A, 0x5D, 0xD3, \
      0x94, 0x1A, 0x17, 0x83, 0x39, 0x0B, 0xBD, 0xE4 }

#endif // MC_UUID_H_
