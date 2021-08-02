/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#ifndef _MTK_PLATFORM_HARDWARE_INCLUDE_MTKCAM_IOPIPE_CAMIO_ISTATPIPE_H_
#define _MTK_PLATFORM_HARDWARE_INCLUDE_MTKCAM_IOPIPE_CAMIO_ISTATPIPE_H_

#include "IHalCamIO.h"

/******************************************************************************
 *
 ******************************************************************************/
namespace NSCam {
namespace NSIoPipe {
namespace NSCamIOPipe {

#define NORMALPIPE_MAX_INST_CNT 3


/*****************************************************************************
*
* @class INormalPipe
* @brief CamIO Pipe Interface for Normal pipe in frame based mode.
* @details 
* The data path will be Sensor --> ISP-- Mem. 
*
******************************************************************************/
class IStatisticPipe : public IHalCamStatIO
{
public:     ////                    Instantiation.
    static  IStatisticPipe  *createInstance(MUINT32 sensorIndex, char const* szCallerName);
    void                    destroyInstance(char const* szCallerName);

public:     ////                    Attributes.

    /**
    * @brief get the pipe name
    *
    * @details
    *
    * @note
    *
    * @return
    * A null-terminated string indicating the name of the pipe.
    *
    */
    static  char const*             pipeName() { return "CamIO::StatisticPipe"; }

    /**
    * @brief get the pipe name
    *
    * @details
    *
    * @note
    *
    * @return
    * A null-terminated string indicating the name of the pipe.
    *
    */
    virtual char const*             getPipeName() const { return pipeName(); }

    virtual MBOOL sendCommand(MINT32 cmd, MINTPTR arg1, MINTPTR arg2, MINTPTR arg3) = 0;
};

enum ESPipeCmd {
    ESPipeCmd_GET_TG_INDEX          = 0x0800,
    ESPipeCmd_GET_BURSTQNUM         = 0x0801,
    ESPipeCmd_GET_CAMSV_IMGO_SIZE   = 0x0802,
    ESPipeCmd_GET_STT_CUR_BUF       = 0x0812,
    ESPipeCmd_GET_TG_OUT_SIZE       = 0x110F,
    ESPipeCmd_SET_TG_INT_LINE       = 0x110E,
    ESIPECmd_MAX
};


/******************************************************************************
 *
 ******************************************************************************/
};  //namespace NSCamIO
};  //namespace NSIoPipe
};  //namespace NSCam
#endif  //_MTK_PLATFORM_HARDWARE_INCLUDE_MTKCAM_IOPIPE_CAMIO_ISTATPIPE_H_

