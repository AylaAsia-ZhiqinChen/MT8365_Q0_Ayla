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

#include <hardware/audio_effect.h>
//------------------------------------------------------------------------------
// Effect descriptors
//------------------------------------------------------------------------------

// UUIDs for effect types have been generated from http://www.itu.int/ITU-T/asn1/uuid.html
// as the pre processing effects are not defined by OpenSL ES

// Acoustic Echo Cancellation 27dab416-23f8-11e8-b467-0ed5f89f718b
const effect_descriptor_t qcom_product_aec_descriptor = {
        { 0x7b491460, 0x8d4d, 0x11e0, 0xbd61, { 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b } }, // type
        { 0x27dab416, 0x23f8, 0x11e8, 0xb467, { 0x0e, 0xd5, 0xf8, 0x9f, 0x71, 0x8b } }, // uuid
        EFFECT_CONTROL_API_VERSION,
        (EFFECT_FLAG_TYPE_PRE_PROC|EFFECT_FLAG_DEVICE_IND|EFFECT_FLAG_HW_ACC_TUNNEL),
        0,
        0,
        "Acoustic Echo Canceler",
        "Qualcomm Fluence for Blueline/Crosshatch"
};

// Noise suppression 27dab6d2-23f8-11e8-b467-0ed5f89f718b
const effect_descriptor_t qcom_product_ns_descriptor = {
        { 0x58b4b260, 0x8e06, 0x11e0, 0xaa8e, { 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b } }, // type
        { 0x27dab6d2, 0x23f8, 0x11e8, 0xb467, { 0x0e, 0xd5, 0xf8, 0x9f, 0x71, 0x8b } }, // uuid
        EFFECT_CONTROL_API_VERSION,
        (EFFECT_FLAG_TYPE_PRE_PROC|EFFECT_FLAG_DEVICE_IND|EFFECT_FLAG_HW_ACC_TUNNEL),
        0,
        0,
        "Noise Suppression",
        "Qualcomm Fluence for Blueline/Crosshatch"
};
