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

#ifndef _MTK_PLATFORM_HARDWARE_INCLUDE_MTKCAM_IOPIPE_POSTPROC_IPORTENUM_ISP3_H_
#define _MTK_PLATFORM_HARDWARE_INCLUDE_MTKCAM_IOPIPE_POSTPROC_IPORTENUM_ISP3_H_

/******************************************************************************
 *
 ******************************************************************************/
namespace NSCam {
namespace NSIoPipeIsp3 {
namespace NSPostProc {

/*******************************************************************************
* Pipe Port index.
********************************************************************************/
enum EPipePortIndex             //mapping with the EPortIndex in ispio_pipe_ports.h
{
    EPipePortIndex_TG1I,        // 0
    EPipePortIndex_TG2I,
    EPipePortIndex_CAMSV_TG1I,
    EPipePortIndex_CAMSV_TG2I,
    EPipePortIndex_IMGI,
    EPipePortIndex_LSCI,        //5
    EPipePortIndex_LCEI,
    EPipePortIndex_VIPI,
    EPipePortIndex_VIP2I,
    EPipePortIndex_VIP3I,
    EPipePortIndex_UFDI,        //10
    EPipePortIndex_CQI,
    EPipePortIndex_TDRI,
    EPipePortIndex_IMGO,
    EPipePortIndex_UFEO,
    EPipePortIndex_RRZO,        //15
    EPipePortIndex_IMGO_D,
    EPipePortIndex_RRZO_D,
    EPipePortIndex_CAMSV_IMGO,
    EPipePortIndex_CAMSV_IMGO_D,
    EPipePortIndex_IMG2O,       //20
    EPipePortIndex_IMG3O,
    EPipePortIndex_IMG3BO,
    EPipePortIndex_IMG3CO,
    EPipePortIndex_MFBO,
    EPipePortIndex_LCSO,        //25
    EPipePortIndex_AAO,
    EPipePortIndex_ESFKO,
    EPipePortIndex_AFO_D,
    EPipePortIndex_FEO,
    EPipePortIndex_WROTO,       //30
    EPipePortIndex_WDMAO,
    EPipePortIndex_JPEGO,
    //
    EPipePortIndex_DISPO,
    EPipePortIndex_VIDO,
    //
};

/******************************************************************************
 *
 ******************************************************************************/
};  //namespace NSPostProc
};  //namespace NSIoPipe
};  //namespace NSCam
#endif  //_MTK_PLATFORM_HARDWARE_INCLUDE_MTKCAM_IOPIPE_POSTPROC_IPORTENUM_H_

