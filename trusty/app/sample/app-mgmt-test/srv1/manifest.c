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

#include <trusty_app_manifest.h>
#include <stddef.h>
#include <stdio.h>

trusty_app_manifest_t TRUSTY_APP_MANIFEST_ATTRS trusty_app_manifest =
{
    /* UUID :{6a5e5bb7-24fe-462f-9935-e25df5ef1469} */
    { 0x6a5e5bb7, 0x24fe, 0x462f,
     {0x99, 0x35, 0xe2, 0x5d, 0xf5, 0xef, 0x14, 0x69} },

    /* optional configuration options here */
    {
        TRUSTY_APP_CONFIG_MIN_HEAP_SIZE(4096),
        TRUSTY_APP_CONFIG_MGMT_FLAGS(TRUSTY_APP_MGMT_FLAGS_RESTART_ON_EXIT),
    },
};
