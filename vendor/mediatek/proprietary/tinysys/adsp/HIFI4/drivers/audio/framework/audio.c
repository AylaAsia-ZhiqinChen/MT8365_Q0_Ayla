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

#include "audio.h"
#include "audio_task.h"

#include <audio_type.h>
#include <audio_drv_log.h>

#ifdef CFG_IPC_SUPPORT
#include <audio_messenger_ipi.h>
#include <adsp_ipi.h>
#endif
#include <afe_drv_pcm.h>

#if defined(FAKE_HOST_IPC_UT)
extern void fake_host_ipc_register(void);;
#endif

/* TODO config control */
#ifdef CFG_HW_RES_MGR
#include "hw_res_mgr.h"
#endif
#ifdef DSP_STATE_SUPPORT
#include "dsp_state_implement.h"
#include "dsp_state.h"
#endif
#include "va_state_implement.h"

NORMAL_SECTION_FUNC void audio_init(void)
{
#if defined(MTK_AUDIO_FRAMEWORK_SUPPORT)
    AUD_LOG_D("+%s(), Heap free/total = %d/%d\n",
              __func__, xPortGetFreeHeapSize(), configTOTAL_HEAP_SIZE);

#ifdef CFG_IPC_SUPPORT
    /* register IPI */
    audio_messenger_ipi_init();
#endif
    /* audio driver init*/
    audio_driver_init();
    AUD_LOG_D("-%s(), Heap free/total = %d/%d\n",
              __func__, xPortGetFreeHeapSize(), configTOTAL_HEAP_SIZE);

   /* TODO do hardware resource manger init here  */
#ifdef CFG_HW_RES_MGR
   hw_res_implement_init();
#endif

#ifdef DSP_STATE_SUPPORT
   /* data state init */
   dsp_state_implement_init();
#endif
   /* va state init */
   va_state_implement_init();

#ifdef DSP_STATE_SUPPORT
   dsp_state_event_proc(DSP_EVT_INIT);
#endif

#if defined(FAKE_HOST_IPC_UT)
    fake_host_ipc_register();
#endif

    /* audio task init*/
    init_audio_task(TASK_SCENE_AUDIO_CONTROLLER);

#ifdef CFG_IPC_SUPPORT
#if defined(CFG_HOST_READY_NOTIFY_SUPPORT)
    struct dsp_info_notify info;
#if defined(CFG_HW_RES_MGR) && defined(CFG_DSP_CLK_SUPPORT)
    info.max_pll = CFG_DSP_PLL_VALUE;
#else
    info.max_pll = 0;
#endif
    /* Send DSP audio ready to host */
    audio_ready_notify_host((void *)&info, sizeof(struct dsp_info_notify));
#endif
#endif
#endif
}


NORMAL_SECTION_FUNC void audio_deinit(void)
{
    //audio_task_factory_deinit();
    /* TODO */
    va_state_implement_uninit();
#ifdef DSP_STATE_SUPPORT
    dsp_state_implement_init();
#endif
#ifdef CFG_HW_RES_MGR
    hw_res_implement_uninit();
#endif
}

