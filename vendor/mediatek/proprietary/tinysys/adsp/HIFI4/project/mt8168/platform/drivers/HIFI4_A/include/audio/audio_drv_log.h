/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*/
/* MediaTek Inc. (C) 2018. All rights reserved.
*
* BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
* RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
* AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
* NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
* SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
* SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
* THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
* THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
* CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
* SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
* STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
* CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
* AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
* OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
* MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
* The following software/firmware and/or related documentation ("MediaTek Software")
* have been modified by MediaTek Inc. All revisions are subject to any receiver\'s
* applicable license agreements with MediaTek Inc.
*/

#ifndef _AUDIO_DRV_LOG_H_
#define _AUDIO_DRV_LOG_H_

#include <stdio.h>
#include <stdarg.h> /* va_list, va_start, va_arg, va_end */

#include <FreeRTOSConfig.h>

/*Definition for debugging */

#define pr_fmt(fmt) fmt

#ifdef AUD_DRV_LOG_D
#undef AUD_DRV_LOG_D
#endif
#ifdef AUDIO_LOG_DEBUG
#define AUD_DRV_LOG_D(fmt, ...) PRINTF_D("[D]" pr_fmt(fmt), ##__VA_ARGS__)
#else
#define AUD_DRV_LOG_D(x...)
#endif

#ifdef AUD_DRV_LOG_I
#undef AUD_DRV_LOG_I
#endif
#define AUD_DRV_LOG_I(fmt, ...) PRINTF_I("[I]" pr_fmt(fmt), ##__VA_ARGS__)


#ifdef AUD_DRV_LOG_W
#undef AUD_DRV_LOG_W
#endif
#define AUD_DRV_LOG_W(fmt, ...) PRINTF_W("[W]" pr_fmt(fmt), ##__VA_ARGS__)


#ifdef AUD_DRV_LOG_E
#undef AUD_DRV_LOG_E
#endif
#define AUD_LOG_E(fmt, ...) PRINTF_E("[E]" pr_fmt(fmt), ##__VA_ARGS__)



#ifdef AUD_DRV_LOG_V
#undef AUD_DRV_LOG_V
#endif
#define AUD_DRV_LOG_V(x...)


#ifdef AUD_DRV_LOG_VV
#undef AUD_DRV_LOG_VV
#endif
#define AUD_DRV_LOG_VV(x...)

#ifdef AUD_LOG_D
#undef AUD_LOG_D
#endif
#define AUD_LOG_D(fmt, ...) PRINTF_D("[D]" pr_fmt(fmt), ##__VA_ARGS__)

#ifdef AUD_LOG_W
#undef AUD_LOG_W
#endif
#define AUD_LOG_W(fmt, ...) PRINTF_W("[W]" pr_fmt(fmt), ##__VA_ARGS__)

#ifdef AUD_LOG_V
#undef AUD_LOG_V
#endif
#if 0
#define AUD_LOG_V(x...) AUD_LOG_D(x)
#else
#define AUD_LOG_V(x...)
#endif

#ifdef AUD_LOG_VV
#undef AUD_LOG_VV
#endif
#define AUD_LOG_VV(x...)

#endif /* end of AUDIO_DRV_LOG_H */

