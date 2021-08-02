/*
 * Copyright (C) 2017 The Android Open Source Project
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

#include <stddef.h>
#include <stdio.h>
#include <trusty_app_manifest.h>

/* App UUID:   {7936e12e-a930-11e7-abc4-cec278b6b50a} */
#define KM_ATTESTATION_STORAGE_TEST_UUID                   \
    {                                                      \
        0x7936e12e, 0xa930, 0x11e7, {                      \
            0xab, 0xc4, 0xce, 0xc2, 0x78, 0xb6, 0xb5, 0x0a \
        }                                                  \
    }

trusty_app_manifest_t TRUSTY_APP_MANIFEST_ATTRS trusty_app_manifest = {
        .uuid = KM_ATTESTATION_STORAGE_TEST_UUID,

        /* optional configuration options here */
        {
                /* 16 pages for heap */
                TRUSTY_APP_CONFIG_MIN_HEAP_SIZE(16 * 4096),

                /* 2 pages for stack */
                TRUSTY_APP_CONFIG_MIN_STACK_SIZE(2 * 4096),
        },
};
