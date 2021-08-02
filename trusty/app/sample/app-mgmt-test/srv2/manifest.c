/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "app_mgmt_srv2.h"

#include <trusty_app_manifest.h>
#include <stddef.h>
#include <stdio.h>
#include <trusty_ipc.h>


trusty_app_manifest_t TRUSTY_APP_MANIFEST_ATTRS trusty_app_manifest =
{
    /* UUID :{50a0686a-48b7-4089-87b1-20093168ef52} */
    { 0x50a0686a, 0x48b7, 0x4089,
     {0x87, 0xb1, 0x20, 0x09, 0x31, 0x68, 0xef, 0x52} },

    /* optional configuration options here */
    {
        TRUSTY_APP_CONFIG_MIN_HEAP_SIZE(4096),
        TRUSTY_APP_CONFIG_MGMT_FLAGS(TRUSTY_APP_MGMT_FLAGS_DEFERRED_START),
    },
};

/* Start the application on connection to START_PORT */
TRUSTY_APP_START_PORT(START_PORT, IPC_PORT_ALLOW_NS_CONNECT);
