/*
 * Copyright (C) 2012 The Android Open Source Project
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
#ifndef _BDROID_BUILDCFG_H
#define _BDROID_BUILDCFG_H

/* Default class of device
* {SERVICE_CLASS, MAJOR_CLASS, MINOR_CLASS}
*
* SERVICE_CLASS:0x24 (Bit18 - Rendering,Bit21 -Audio)
* MAJOR_CLASS:0x04 - Audio/Video
* MINOR_CLASS:0x07 - Portable Audio
*
*/


#define BTA_DM_COD {0x4a, 0x0a, 0x0C}

#define BTA_AV_SINK_INCLUDED     TRUE

#define BLE_VND_INCLUDED         TRUE
#define KERNEL_MISSING_CLOCK_BOOTTIME_ALARM TRUE

/**
 * if need source and sink enable both
 */
#define MTK_A2DP_SRC_SINK_BOTH        TRUE

#endif
