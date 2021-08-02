/*
 * Copyright 2015 The Android Open Source Project
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

#ifndef __TRUSTZONE_TA_JPEG__
#define __TRUSTZONE_TA_JPEG__

#define TZ_TA_JPEG_UUID   "ff33a6e0-8635-11e2-9e96-036637253874"

#define USE_MTEE_M4U
#define USE_MTEE_DAPC

/* Command for JPEG TA */
#define TZCMD_JPEG_ENC_START       0
#define TZCMD_JPEG_ENC_DUMP_INPUT  1
#define TZCMD_JPEG_ENC_DUMP_OUTPUT 2
#define TZCMD_JPEG_ENC_FILL_SECMEM 3

#endif /* __TRUSTZONE_TA_JPEG__ */

