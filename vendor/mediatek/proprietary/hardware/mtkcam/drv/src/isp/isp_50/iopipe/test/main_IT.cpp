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

///////////////////////////////////////////////////////////////////////////////
// No Warranty
// Except as may be otherwise agreed to in writing, no warranties of any
// kind, whether express or implied, are given by MTK with respect to any MTK
// Deliverables or any use thereof, and MTK Deliverables are provided on an
// "AS IS" basis.  MTK hereby expressly disclaims all such warranties,
// including any implied warranties of merchantability, non-infringement and
// fitness for a particular purpose and any warranties arising out of course
// of performance, course of dealing or usage of trade.  Parties further
// acknowledge that Company may, either presently and/or in the future,
// instruct MTK to assist it in the development and the implementation, in
// accordance with Company's designs, of certain softwares relating to
// Company's product(s) (the "Services").  Except as may be otherwise agreed
// to in writing, no warranties of any kind, whether express or implied, are
// given by MTK with respect to the Services provided, and the Services are
// provided on an "AS IS" basis.  Company further acknowledges that the
// Services may contain errors, that testing is important and Company is
// solely responsible for fully testing the Services and/or derivatives
// thereof before they are used, sublicensed or distributed.  Should there be
// any third party action brought against MTK, arising out of or relating to
// the Services, Company agree to fully indemnify and hold MTK harmless.
// If the parties mutually agree to enter into or continue a business
// relationship or other arrangement, the terms and conditions set forth
// hereunder shall remain effective and, unless explicitly stated otherwise,
// shall prevail in the event of a conflict in the terms in any agreements
// entered into between the parties.
////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008, MediaTek Inc.
// All rights reserved.
//
// Unauthorized use, practice, perform, copy, distribution, reproduction,
// or disclosure of this information in whole or in part is prohibited.
////////////////////////////////////////////////////////////////////////////////
// AcdkCLITest.cpp  $Revision$
////////////////////////////////////////////////////////////////////////////////

//! \file  AcdkCLITest.cpp
//! \brief

#define LOG_TAG "p1p2Test"

#include <vector>
#include <list>
#include <stdlib.h>

using namespace std;

//#include <linux/cache.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <stdio.h>
//
#include <errno.h>
#include <fcntl.h>

#include <mtkcam/def/common.h>
//#include <mtkcam/common/hw/hwstddef.h>

#include <mtkcam/drv/iopipe/PostProc/INormalStream.h>
#include <ispio_pipe_ports.h>
#include <ispio_pipe_scenario.h>
#include <imem_drv.h>
#include <isp_drv.h>
#include <isp_reg.h>


#include <mtkcam/utils/imgbuf/IImageBuffer.h>
#include <utils/StrongPointer.h>
#include <mtkcam/utils/std/common.h>
#include <mtkcam/utils/imgbuf/ImageBufferHeap.h>


//thread
#include <utils/threads.h>
#include <mtkcam/def/PriorityDefs.h>
//thread priority
#include <system/thread_defs.h>
#include <sys/resource.h>
#include <utils/ThreadDefs.h>
#include <pthread.h>
#include <semaphore.h>

///
//pass1 needed
#include <mtkcam/drv/IHalSensor.h>
#include <mtkcam/drv/iopipe/CamIO/INormalPipe.h>
#include <ispio_utility.h>
//tuning
#include <tuning_mgr.h>

//
using namespace android;

using namespace NSCam;
using namespace NSIoPipe;
using namespace NSPostProc;
using namespace NSCam::NSIoPipe::NSCamIOPipe;
using namespace NSImageio;
using namespace NSIspio;

/*************************************************************************************
* Log Utility
*************************************************************************************/
#ifndef USING_MTK_LDVT   // Not using LDVT.
#define  DBG_LOG_TAG        ""
#else
#define  DBG_LOG_TAG        LOG_TAG
#endif

#include <imageio_log.h>    // Note: DBG_LOG_TAG/LEVEL will be used in header file, so header must be included after definition.
DECLARE_DBG_LOG_VARIABLE(p1p2Test);
// Clear previous define, use our own define.
#undef LOG_VRB
#undef LOG_DBG
#undef LOG_INF
#undef LOG_WRN
#undef LOG_ERR
#undef LOG_AST
#define LOG_VRB(fmt, arg...)        do { if (p1p2Test_DbgLogEnable_VERBOSE) { BASE_LOG_VRB(fmt, ##arg); } } while(0)
#define LOG_DBG(fmt, arg...)        do { if (p1p2Test_DbgLogEnable_DEBUG  ) { BASE_LOG_DBG(fmt, ##arg); } } while(0)
#if 0  //kk test
#define LOG_INF(fmt, arg...)        do { if (P2PipeWrapper_DbgLogEnable_INFO   ) { BASE_LOG_INF(fmt, ##arg); } } while(0)
#else
#define LOG_INF        printf
#endif

#define LOG_WRN(fmt, arg...)        do { if (p1p2Test_DbgLogEnable_WARN   ) { BASE_LOG_WRN(fmt, ##arg); } } while(0)
#define LOG_ERR(fmt, arg...)        do { if (p1p2Test_DbgLogEnable_ERROR  ) { BASE_LOG_ERR(fmt, ##arg); } } while(0)
#define LOG_AST(cond, fmt, arg...)  do { if (p1p2Test_DbgLogEnable_ASSERT ) { BASE_LOG_AST(cond, fmt, ##arg); } } while(0)

#define FUNCTION_LOG_START      LOG_INF("+");
#define FUNCTION_LOG_END        LOG_INF("-");
#define ERROR_LOG               LOG_ERR("Error");

///////////////////////////////////////////////
//test cases
int basic_flow(int num,int type);
int basic_flow_tuning(int num,int type);
int basic_flow_crop_resize(int num,int type);



//////////////////////////////////////////////
//global variables
static sem_t     mbufferSem;
static sem_t     mLeaveSem;
static bool bEnTuning=false;
static bool bEnZoom=false;
static bool bLeaveP2=false;
static bool bArbCrop=false;

static sem_t     mSem;

static pthread_t           mThread;
static int runningcase=0;
static int runningtype=0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;    //mutex for read/write pass1 dequeued ring buffer count

/******************************************************************************
* save the buffer to the file
*******************************************************************************/
static bool
saveBufToFile(char const*const fname, MUINT8 *const buf, MUINT32 const size)
{
    int nw, cnt = 0;
    uint32_t written = 0;

    //LOG_INF("(name, buf, size) = (%s, %x, %d)", fname, buf, size);
    //LOG_INF("opening file [%s]\n", fname);
    int fd = ::open(fname, O_RDWR | O_CREAT, S_IRWXU);
    if (fd < 0) {
        LOG_ERR("failed to create file [%s]: %s", fname, ::strerror(errno));
        return false;
    }

    //LOG_INF("writing %d bytes to file [%s]\n", size, fname);
    while (written < size) {
        nw = ::write(fd,
                     buf + written,
                     size - written);
        if (nw < 0) {
            LOG_ERR("failed to write to file [%s]: %s", fname, ::strerror(errno));
            break;
        }
        written += nw;
        cnt++;
    }
    //LOG_INF("done writing %d bytes to file [%s] in %d passes\n", size, fname, cnt);
    ::close(fd);
    return true;
}

MINT32 getUs()
{
   struct timeval tv;
   ::gettimeofday(&tv, NULL);
   return tv.tv_sec * 1000000 + tv.tv_usec;
}
static QBufInfo gBufInfoFromP1;

/*******************************************************************************
*  p1 tuning
********************************************************************************/
int setP1Tuning(int magicNum)
{
    int ret=0;

    TuningMgr::getInstance().init();
    TuningMgr::getInstance().flushSetting(eSoftwareScenario_Main_Normal_Stream);
        TuningMgr::getInstance().dequeBuffer(eSoftwareScenario_Main_Normal_Stream,magicNum);
    //set OB
    TuningMgr::getInstance().updateEngine(eSoftwareScenario_Main_Normal_Stream, eTuningMgrFunc_Obc);
    TUNING_MGR_WRITE_ENABLE_BITS(eSoftwareScenario_Main_Normal_Stream, CAM_CTL_EN_P1, OB_EN, 0x01)
    TUNING_MGR_WRITE_REG(eSoftwareScenario_Main_Normal_Stream, CAM_OBC_OFFST0, magicNum*10+1)
    TUNING_MGR_WRITE_REG(eSoftwareScenario_Main_Normal_Stream, CAM_OBC_OFFST1, magicNum*10+2)
    TUNING_MGR_WRITE_REG(eSoftwareScenario_Main_Normal_Stream, CAM_OBC_OFFST2, magicNum*10+3)
    TUNING_MGR_WRITE_REG(eSoftwareScenario_Main_Normal_Stream, CAM_OBC_OFFST3, magicNum*10+4)
    TUNING_MGR_WRITE_REG(eSoftwareScenario_Main_Normal_Stream, CAM_OBC_GAIN0, magicNum*10+5)
    TUNING_MGR_WRITE_REG(eSoftwareScenario_Main_Normal_Stream, CAM_OBC_GAIN1, magicNum*10+6)
    TUNING_MGR_WRITE_REG(eSoftwareScenario_Main_Normal_Stream, CAM_OBC_GAIN2, magicNum*10+7)
    TUNING_MGR_WRITE_REG(eSoftwareScenario_Main_Normal_Stream, CAM_OBC_GAIN3, magicNum*10+8)
    //
    TuningMgr::getInstance().enqueBuffer(eSoftwareScenario_Main_Normal_Stream,magicNum);
    return ret;
}
/*******************************************************************************
*  p2 tuning
********************************************************************************/
int setP2Tuning(int mode, isp_reg_t **ppIspPhyReg)
{
    int ret = 0;
    isp_reg_t *pIspPhyReg = (*ppIspPhyReg);
    MUINT32* pIspSetting = (MUINT32*)pIspPhyReg;

    if(mode==0){
        ppIspPhyReg[0]->CAM_CTL_EN_P1.Bits.OB_EN=0;
        ppIspPhyReg[0]->CAM_CTL_EN_P1.Bits.LSC_EN=0;
        ppIspPhyReg[0]->CAM_CTL_EN_P1.Bits.BNR_EN=0;
        ppIspPhyReg[0]->CAM_CTL_EN_P2.Bits.SL2_EN=0;
        ppIspPhyReg[0]->CAM_CTL_EN_P2.Bits.PGN_EN=0;
        ppIspPhyReg[0]->CAM_CTL_EN_P2.Bits.CCL_EN=0;
        ppIspPhyReg[0]->CAM_CTL_EN_P2.Bits.GGM_EN=0;
        //
        ppIspPhyReg[0]->CAM_CTL_EN_P2.Bits.NBC_EN=0;
        ppIspPhyReg[0]->CAM_CTL_EN_P2.Bits.PCA_EN=0;
        ppIspPhyReg[0]->CAM_CTL_EN_P2.Bits.SEEE_EN=0;
        //
        //G2G
        ppIspPhyReg[0]->CAM_CTL_EN_P2.Bits.G2G_EN=1;
        pIspSetting[0x4920>>2] = 0x00000200;
        pIspSetting[0x4924>>2] = 0x00000000;
        pIspSetting[0x4928>>2] = 0x02000000;
        pIspSetting[0x492C>>2] = 0x00000000;
        pIspSetting[0x4930>>2] = 0x00000000;
        pIspSetting[0x4934>>2] = 0x00000200;
        pIspSetting[0x4938>>2] = 0x00000009;
        //
        //CFA
        ppIspPhyReg[0]->CAM_CTL_EN_P2.Bits.CFA_EN=1;
        pIspSetting[0x48A0>>2] = 0x00000000;
        pIspSetting[0x48A4>>2] = 0x00000C03;
        pIspSetting[0x48A8>>2] = 0x01082006;
        pIspSetting[0x48AC>>2] = 0x80081008;
        pIspSetting[0x48B0>>2] = 0x37084208;
        pIspSetting[0x48B4>>2] = 0x1806200A;
        pIspSetting[0x48B8>>2] = 0x0010A020;
        pIspSetting[0x48BC>>2] = 0x0015C020;
        pIspSetting[0x48C0>>2] = 0x0015C040;
        pIspSetting[0x48C4>>2] = 0x00350050;
        pIspSetting[0x48C8>>2] = 0x00A41440;
        pIspSetting[0x48CC>>2] = 0x00421084;
        pIspSetting[0x48D0>>2] = 0x01484185;
        pIspSetting[0x48D4>>2] = 0x00410417;
        pIspSetting[0x48D8>>2] = 0x000203FF;
        pIspSetting[0x48DC>>2] = 0x00000008;
        pIspSetting[0x48E0>>2] = 0xE0088888;
        pIspSetting[0x48E4>>2] = 0x00000010;
        pIspSetting[0x48E8>>2] = 0x0052A5FF;
        pIspSetting[0x48F0>>2] = 0x781F55D4;
        pIspSetting[0x48F4>>2] = 0x00018006;
        //
        //G2C
        ppIspPhyReg[0]->CAM_CTL_EN_P2.Bits.G2C_EN=1;
        pIspSetting[0x4BA0>>2] = 0x012D0099;
        pIspSetting[0x4BA4>>2] = 0x0000003A;
        pIspSetting[0x4BA8>>2] = 0x075607AA;
        pIspSetting[0x4BAC>>2] = 0x00000100;
        pIspSetting[0x4BB0>>2] = 0x072A0100;
        pIspSetting[0x4BB4>>2] = 0x000007D6;

        //GGM: EN=0
        pIspSetting[0x5000>>2]=  0x0000F400;   /* 0x15005000: CAM_GGM_RB_GMT[0] */
        pIspSetting[0x5004>>2]=  0x040C5C3D;   /* 0x15005004: CAM_GGM_RB_GMT[1] */
        pIspSetting[0x5008>>2]=  0x0C154454;   /* 0x15005008: CAM_GGM_RB_GMT[2] */
        pIspSetting[0x500C>>2]=  0x081C3865;   /* 0x1500500C: CAM_GGM_RB_GMT[3] */
        pIspSetting[0x5010>>2]=  0x08223073;   /* 0x15005010: CAM_GGM_RB_GMT[4] */
        pIspSetting[0x5014>>2]=  0x04282C7F;   /* 0x15005014: CAM_GGM_RB_GMT[5] */
        pIspSetting[0x5018>>2]=  0x042D288A;   /* 0x15005018: CAM_GGM_RB_GMT[6] */
        pIspSetting[0x501C>>2]=  0x04322494;   /* 0x1500501C: CAM_GGM_RB_GMT[7] */
        pIspSetting[0x5020>>2]=  0x0437249D;   /* 0x15005020: CAM_GGM_RB_GMT[8] */
        pIspSetting[0x5024>>2]=  0x043C20A6;   /* 0x15005024: CAM_GGM_RB_GMT[9] */
        pIspSetting[0x5028>>2]=  0x00411CAE;   /* 0x15005028: CAM_GGM_RB_GMT[10] */
        pIspSetting[0x502C>>2]=  0x044520B5;   /* 0x1500502C: CAM_GGM_RB_GMT[11] */
        pIspSetting[0x5030>>2]=  0x004A1CBD;   /* 0x15005030: CAM_GGM_RB_GMT[12] */
        pIspSetting[0x5034>>2]=  0x004E18C4;   /* 0x15005034: CAM_GGM_RB_GMT[13] */
        pIspSetting[0x5038>>2]=  0x00521CCA;   /* 0x15005038: CAM_GGM_RB_GMT[14] */
        pIspSetting[0x503C>>2]=  0x005618D1;   /* 0x1500503C: CAM_GGM_RB_GMT[15] */
        pIspSetting[0x5040>>2]=  0x005A18D7;   /* 0x15005040: CAM_GGM_RB_GMT[16] */
        pIspSetting[0x5044>>2]=  0x005E14DD;   /* 0x15005044: CAM_GGM_RB_GMT[17] */
        pIspSetting[0x5048>>2]=  0x006218E2;   /* 0x15005048: CAM_GGM_RB_GMT[18] */
        pIspSetting[0x504C>>2]=  0x0C6618E8;   /* 0x1500504C: CAM_GGM_RB_GMT[19] */
        pIspSetting[0x5050>>2]=  0x006914EE;   /* 0x15005050: CAM_GGM_RB_GMT[20] */
        pIspSetting[0x5054>>2]=  0x006D14F3;   /* 0x15005054: CAM_GGM_RB_GMT[21] */
        pIspSetting[0x5058>>2]=  0x0C7114F8;   /* 0x15005058: CAM_GGM_RB_GMT[22] */
        pIspSetting[0x505C>>2]=  0x007414FD;   /* 0x1500505C: CAM_GGM_RB_GMT[23] */
        pIspSetting[0x5060>>2]=  0x0C781502;   /* 0x15005060: CAM_GGM_RB_GMT[24] */
    }
    else if(mode==1){
        ppIspPhyReg[0]->CAM_CTL_EN_P2.Bits.GGM_EN=1;
        pIspSetting[0x5000>>2]=  0x3000F400;   /* 0x15005000: CAM_GGM_RB_GMT[0] */
        pIspSetting[0x5004>>2]=  0x240C5C3D;   /* 0x15005004: CAM_GGM_RB_GMT[1] */
        pIspSetting[0x5008>>2]=  0x1C154454;   /* 0x15005008: CAM_GGM_RB_GMT[2] */
        pIspSetting[0x500C>>2]=  0x181C3865;   /* 0x1500500C: CAM_GGM_RB_GMT[3] */
        pIspSetting[0x5010>>2]=  0x18223073;   /* 0x15005010: CAM_GGM_RB_GMT[4] */
        pIspSetting[0x5014>>2]=  0x14282C7F;   /* 0x15005014: CAM_GGM_RB_GMT[5] */
        pIspSetting[0x5018>>2]=  0x142D288A;   /* 0x15005018: CAM_GGM_RB_GMT[6] */
        pIspSetting[0x501C>>2]=  0x14322494;   /* 0x1500501C: CAM_GGM_RB_GMT[7] */
        pIspSetting[0x5020>>2]=  0x1437249D;   /* 0x15005020: CAM_GGM_RB_GMT[8] */
        pIspSetting[0x5024>>2]=  0x143C20A6;   /* 0x15005024: CAM_GGM_RB_GMT[9] */
        pIspSetting[0x5028>>2]=  0x10411CAE;   /* 0x15005028: CAM_GGM_RB_GMT[10] */
        pIspSetting[0x502C>>2]=  0x144520B5;   /* 0x1500502C: CAM_GGM_RB_GMT[11] */
        pIspSetting[0x5030>>2]=  0x104A1CBD;   /* 0x15005030: CAM_GGM_RB_GMT[12] */
        pIspSetting[0x5034>>2]=  0x104E18C4;   /* 0x15005034: CAM_GGM_RB_GMT[13] */
        pIspSetting[0x5038>>2]=  0x10521CCA;   /* 0x15005038: CAM_GGM_RB_GMT[14] */
        pIspSetting[0x503C>>2]=  0x105618D1;   /* 0x1500503C: CAM_GGM_RB_GMT[15] */
        pIspSetting[0x5040>>2]=  0x105A18D7;   /* 0x15005040: CAM_GGM_RB_GMT[16] */
        pIspSetting[0x5044>>2]=  0x105E14DD;   /* 0x15005044: CAM_GGM_RB_GMT[17] */
        pIspSetting[0x5048>>2]=  0x106218E2;   /* 0x15005048: CAM_GGM_RB_GMT[18] */
        pIspSetting[0x504C>>2]=  0x0C6618E8;   /* 0x1500504C: CAM_GGM_RB_GMT[19] */
        pIspSetting[0x5050>>2]=  0x106914EE;   /* 0x15005050: CAM_GGM_RB_GMT[20] */
        pIspSetting[0x5054>>2]=  0x106D14F3;   /* 0x15005054: CAM_GGM_RB_GMT[21] */
        pIspSetting[0x5058>>2]=  0x0C7114F8;   /* 0x15005058: CAM_GGM_RB_GMT[22] */
        pIspSetting[0x505C>>2]=  0x107414FD;   /* 0x1500505C: CAM_GGM_RB_GMT[23] */
        pIspSetting[0x5060>>2]=  0x0C781502;   /* 0x15005060: CAM_GGM_RB_GMT[24] */
    }

    return ret;
}

/******************************************************************************
* pass1 usage
*******************************************************************************/
#define bitmap(bit)({\
            UINT32 _fmt = 0;\
            switch(bit){\
                case 8: _fmt = eImgFmt_BAYER8; break;\
                case 10: _fmt = eImgFmt_BAYER10; break;\
                case 12: _fmt = eImgFmt_BAYER12; break;\
                default:                break;\
            }\
            _fmt;})

#define bitmap_FG(bit)({\
            UINT32 _fmt = 0;\
            switch(bit){\
                case 8: _fmt = eImgFmt_FG_BAYER8; break;\
                case 10: _fmt = eImgFmt_FG_BAYER10; break;\
                case 12: _fmt = eImgFmt_FG_BAYER12; break;\
                default:                break;\
            }\
            _fmt;})


    //number of buf
#define Enque_buf (3)
#define Dummy_buf (3)
#define replace_buf (1)

#define SEN_PIX_BITDEPTH    (10)
#define RRZ_SCALING_RATIO	(2)	//divisor
#define __RRZO 0
#define __IMGO 1
#define __MAXDMAO	(__RRZO + __IMGO + 1)
#define __RRZO_ENABLE (0x1)
#define __IMGO_ENABLE (0x2)
#define _TG_1_	0
#define _TG_2_	1
#define _MAXTG_ 2

#define __CROP_Start(in,crop,fmt) ({\
    NSImageio::NSIspio::ISP_QUERY_RST queryRst;\
    MPoint _point;\
    NSImageio::NSIspio::ISP_QuerySize(_BY_PASSS_PORT,NSImageio::NSIspio::ISP_QUERY_CROP_START_X,\
            (EImageFormat)fmt,(in.w-crop.w)/2,queryRst);\
    _point = MPoint(queryRst.crop_x,(in.h-crop.h)/2);\
    _point;})
//K2 support arbitray cropping
#define __CROP_Start_2(in, crop,fmt,cropsel)({\
        NSImageio::NSIspio::ISP_QUERY_RST queryRst;\
        MPoint _point;\
        MUINT32 width, height;\
        switch (cropsel)\
        {\
            case 0:\
            default:\
                width =  (in.w-crop.w)/2;\
                height = (in.h-crop.h)/2;\
                break;\
            case 1:\
                width  = ((in.w-crop.w)/2-100);\
                height = ((in.h-crop.h)/2-100);\
                break;\
        }\
        NSImageio::NSIspio::ISP_QuerySize(_BY_PASSS_PORT,NSImageio::NSIspio::ISP_QUERY_CROP_START_X,\
                (EImageFormat)fmt,width,queryRst);\
        _point = MPoint(queryRst.crop_x,height);\
        _point;})


    //croping size have an alignment-rule in imgo/imgo_d (no resizer dma port)
    //rrzo do nothing, because of rrzo not using dmao cropping.
    //note :
//       the query operation, if cropsize != inputsize , use ISP_QUERY_CROP_X_PIX to query.
#define __CROP_SIZE(portId,fmt,size,pixmode) ({\
    MSize ___size;\
    ___size = MSize((size.w/RRZ_SCALING_RATIO + 64),(size.h/RRZ_SCALING_RATIO + 64));\
    if(portId == __IMGO){\
        NSImageio::NSIspio::E_ISP_QUERY _op;\
        _op = (size.w != ___size.w)?(NSImageio::NSIspio::ISP_QUERY_CROP_X_PIX):(NSImageio::NSIspio::ISP_QUERY_X_PIX);\
        NSImageio::NSIspio::ISP_QUERY_RST queryRst;\
        NSImageio::NSIspio::ISP_QuerySize(NSImageio::NSIspio::EPortIndex_IMGO,_op,\
            (EImageFormat)fmt,___size.w,queryRst,pixmode);\
            ___size.w = queryRst.x_pix;\
    }\
    if(portId == __RRZO){\
    }\
    ___size;})

//rrzo bypass
#define __CROP_SIZE_2(portId,fmt,size,pixmode) ({\
    MSize ___size;\
    if(portId == __IMGO){\
        ___size = MSize(size.w,size.h);\
        NSImageio::NSIspio::E_ISP_QUERY _op;\
        _op = (size.w != ___size.w)?(NSImageio::NSIspio::ISP_QUERY_CROP_X_PIX):(NSImageio::NSIspio::ISP_QUERY_X_PIX);\
        NSImageio::NSIspio::ISP_QUERY_RST queryRst;\
        NSImageio::NSIspio::ISP_QuerySize(NSImageio::NSIspio::EPortIndex_IMGO,_op,\
            (EImageFormat)fmt,___size.w,queryRst,pixmode);\
            ___size.w = queryRst.x_pix;\
    }\
    if(portId == __RRZO){\
        ___size = MSize(size.w,size.h);\
    }\
    ___size;})

#if 0
#define __RRZ_FMT(fmt)({\
    MUINT32 _fmt;\
    switch(fmt){\
        case eImgFmt_BAYER8: _fmt = eImgFmt_BAYER8; break;    \
        case eImgFmt_BAYER10: _fmt = eImgFmt_BAYER10; break; \
        case eImgFmt_BAYER12: _fmt = eImgFmt_BAYER12; break; \
        default: _fmt = eImgFmt_BAYER10; break;             \
    }\
    _fmt;})
#endif
//floria
#define __RRZ_FMT(fmt)({\
    MUINT32 __fmt;\
    switch(fmt){\
        case eImgFmt_BAYER8: __fmt = eImgFmt_FG_BAYER8; break;  \
        case eImgFmt_BAYER10: __fmt = eImgFmt_FG_BAYER10; break; \
        case eImgFmt_BAYER12: __fmt = eImgFmt_FG_BAYER12; break; \
        default: __fmt = eImgFmt_FG_BAYER10; break;             \
    }\
    __fmt;})

//imgo do nothing, because of imgo have no resizer
#define __SCALE_SIZE(portId,fmt,size,Ratio,pixmode)({\
    MSize __size;\
    __size = MSize((size.w/Ratio),(size.h/Ratio));\
    if(portId == __RRZO){\
        NSImageio::NSIspio::ISP_QUERY_RST queryRst;\
        NSImageio::NSIspio::ISP_QuerySize(NSImageio::NSIspio::EPortIndex_RRZO,NSImageio::NSIspio::ISP_QUERY_X_PIX,\
            (EImageFormat)__RRZ_FMT(fmt),__size.w,queryRst,pixmode);\
            __size.w = queryRst.x_pix;\
    }\
    __size;})

typedef struct HwPortConfig{
    NSCam::NSIoPipe::PortID            mPortID;
    EImageFormat    mFmt;
    MSize            mSize;
    MRect            mCrop;
    MBOOL            mPureRaw;
    MUINT32         mStrideInByte[3];
}HwPortConfig_t;

MBOOL getSensorPixelMode_2(MUINT32* pPixelMode,MUINT32 sensorIdx,MUINT32 scenario,MUINT32 sensorFps)
{
    IHalSensor* pSensorHalObj = NULL;
    IHalSensorList* const pHalSensorList = IHalSensorList::get();

    if( !pHalSensorList ) {
        LOG_INF("error:pHalSensorList == NULL\n");
        return MFALSE;
    }

    pSensorHalObj = pHalSensorList->createSensor(
            LOG_TAG,
            sensorIdx);
    //
    if( pSensorHalObj == NULL )
    {
        LOG_INF("error:pSensorHalObj is NULL\n");
        return MFALSE;
    }

    pSensorHalObj->sendCommand(
            pHalSensorList->querySensorDevIdx(sensorIdx),
            SENSOR_CMD_GET_SENSOR_PIXELMODE,
            (MINT32)&scenario,
            (MINT32)&sensorFps,
            (MINT32)pPixelMode);
    LOG_INF("sensorScenario(%d),sensorFps(%d),pixelMode(%d)\n",
            scenario,
            sensorFps,
            *pPixelMode);

    pSensorHalObj->destroyInstance(LOG_TAG);

    if( *pPixelMode != 0 && *pPixelMode != 1 )
    {
        LOG_INF("error: Un-supported pixel mode %d\n", *pPixelMode);
        return MFALSE;
    }

    return MTRUE;
}


int getSensorSize_2(MSize* pSize,SensorStaticInfo mSensorInfo,MUINT32 SenScenario)
{
    MBOOL ret = MTRUE;
    // sensor size
#define scenario_case(scenario, KEY, pSize)       \
        case scenario:                            \
            (pSize)->w = mSensorInfo.KEY##Width;  \
            (pSize)->h = mSensorInfo.KEY##Height; \
            break;
    switch(SenScenario)
    {
        scenario_case( SENSOR_SCENARIO_ID_NORMAL_PREVIEW, preview, pSize );
        scenario_case( SENSOR_SCENARIO_ID_NORMAL_CAPTURE, capture, pSize );
        scenario_case( SENSOR_SCENARIO_ID_NORMAL_VIDEO, video, pSize );
        scenario_case( SENSOR_SCENARIO_ID_SLIM_VIDEO1, video1, pSize );
        scenario_case( SENSOR_SCENARIO_ID_SLIM_VIDEO2, video2, pSize );
        default:
            LOG_INF("not support sensor scenario(0x%x)\n", SenScenario);
            ret = MFALSE;
            break;
    }
#undef scenario_case

lbExit:
    return ret;
}

MBOOL getOutputFmt_2(NSCam::NSIoPipe::PortID port,MUINT32 bitDepth,SensorStaticInfo mSensorInfo,EImageFormat* pFmt)
{
    MBOOL ret = MFALSE;
    // sensor fmt
#define case_Format( order_bit, mappedfmt, pFmt) \
        case order_bit:                          \
            (*(pFmt)) = mappedfmt;               \
        break;

    if( mSensorInfo.sensorType == SENSOR_TYPE_YUV )
    {
        switch( mSensorInfo.sensorFormatOrder )
        {
            case_Format( SENSOR_FORMAT_ORDER_UYVY, eImgFmt_UYVY, pFmt);
            case_Format( SENSOR_FORMAT_ORDER_VYUY, eImgFmt_VYUY, pFmt);
            case_Format( SENSOR_FORMAT_ORDER_YUYV, eImgFmt_YUY2, pFmt);
            case_Format( SENSOR_FORMAT_ORDER_YVYU, eImgFmt_YVYU, pFmt);
            default:
            LOG_INF("ERROR: formatOrder not supported, 0x%x\n", mSensorInfo.sensorFormatOrder);
            goto lbExit;
            break;
        }
        //LOG_INF("sensortype:(0x%x), fmt(0x%x)\n", mSensorInfo.sensorType, *pFmt);
    }
    else if( mSensorInfo.sensorType == SENSOR_TYPE_RAW )
    {
        if(port == PORT_IMGO) //imgo
        {
            switch( bitDepth )
            {
                case_Format(  8, eImgFmt_BAYER8 , pFmt);
                case_Format( 10, eImgFmt_BAYER10, pFmt);
                case_Format( 12, eImgFmt_BAYER12, pFmt);
                case_Format( 14, eImgFmt_BAYER14, pFmt);
                default:
                LOG_INF("ERROR: bitdepth not supported, 0x%x\n", bitDepth);
                goto lbExit;
                break;
            }
        }
        else // rrzo
        {
            switch( bitDepth)
            {
                case_Format(  8, eImgFmt_FG_BAYER8 , pFmt);
                case_Format( 10, eImgFmt_FG_BAYER10, pFmt);
                case_Format( 12, eImgFmt_FG_BAYER12, pFmt);
                case_Format( 14, eImgFmt_FG_BAYER14, pFmt);
                default:
                LOG_INF("ERROR: bitdepth not supported, 0x%x\n", bitDepth);
                goto lbExit;
                break;
            }
        }
        //LOG_INF("sensortype: 0x%x, port(0x%x), fmt(0x%x), order(%d)\n",
         //       mSensorInfo.sensorType, port.index, *pFmt, mSensorInfo.sensorFormatOrder);
    }
    else
    {
        LOG_INF("ERROR: sensorType not supported yet(0x%x)\n", mSensorInfo.sensorType);
        goto lbExit;
    }
    ret = MTRUE;
#undef case_Format

lbExit:
    return ret;
}

#define Multi_thread 1

class _test_camio_
{
    _test_camio_()
    {
        m_pNormalP_FrmB = NULL;
        for(int i=0;i<__MAXDMAO;i++)
            m_prepalce_Buf[i]=NULL;
        m_enablePort=0x0;m_bStop = MFALSE;m_TgSize = MSize(0,0);
        m_Sensoridx = 0;
    };
    ~_test_camio_(){};
    public:
        INormalPipe_FrmB*    m_pNormalP_FrmB;
        IImageBuffer*        m_prepalce_Buf[__MAXDMAO];
        MUINT32                 m_enablePort;
        MSize                m_TgSize;
        NSImageio::NSIspio::E_ISP_PIXMODE mPixMode;
        list<QBufInfo>     m_dequeBufInfoList;    //for ring buffer, magic number
        MUINT32          m_Sensoridx;

        static _test_camio_*     create(void);
        void             destroy(void);
        void             startThread(void);
        void             stopThread(void);

    private:
        static MVOID*    _infiniteloop(void* arg);
        static MVOID*   _p2ThreadLoop(MVOID *arg);
        MBOOL              m_bStop;
        pthread_t         m_Thread;
        sem_t            m_semThread;
};

_test_camio_* _test_camio_::create(void)
{
    return new _test_camio_();
}

void _test_camio_::destroy(void)
{
    delete this;
}

void _test_camio_::startThread(void)
{
    // Init semphore
    ::sem_init(&this->m_semThread, 0, 0);

    // Create main thread for preview and capture
    printf("error:temp borrow ispdequeuethread priority, need to create self priority\n");
    pthread_attr_t const attr = {0, NULL, 1024 * 1024, 4096, SCHED_RR, PRIO_RT_ISPDEQUEUE_THREAD};
    pthread_create(&this->m_Thread, &attr, this->_infiniteloop, this);
    //
    pthread_attr_t const attr2 = {0, NULL, 1024 * 1024, 4096, SCHED_OTHER, NICE_CAMERA_PASS2};
    pthread_create(&mThread, &attr2, _p2ThreadLoop, this);
}

void _test_camio_::stopThread(void)
{
    pthread_join(this->m_Thread, NULL);
    ::sem_wait(&this->m_semThread);
    ::sem_wait(&mLeaveSem);
}

MVOID* _test_camio_::_infiniteloop(void* arg)
{

    _test_camio_* _this = (_test_camio_*)arg;
    QBufInfo dequeBufInfo;
    QBufInfo _replace;
    dequeBufInfo.mvOut.reserve(2);
    printf("start infiniteloop\n");
    MUINT32 _loop = 0;
    //  detach thread => cannot be join
    ::pthread_detach(::pthread_self());

        _replace.mvOut.clear();
        if( _this->m_enablePort & __IMGO_ENABLE) {
            //enque
            NSCam::NSIoPipe::NSCamIOPipe::BufInfo OutBuf(PORT_IMGO, 0);
            dequeBufInfo.mvOut.push_back(OutBuf);
            MSize _cropsize = __CROP_SIZE(__IMGO,_this->m_prepalce_Buf[__IMGO]->getImgFormat(),_this->m_TgSize,_this->mPixMode);

            //replace
            //in replace testing , use mag num:7
            NSCam::NSIoPipe::NSCamIOPipe::BufInfo InBuf(PORT_IMGO, _this->m_prepalce_Buf[__IMGO],_cropsize,MRect(__CROP_Start(_this->m_TgSize,_cropsize,_this->m_prepalce_Buf[__IMGO]->getImgFormat()),_cropsize),(replace_buf + Enque_buf + Dummy_buf));
            _replace.mvOut.push_back(InBuf);
        }
        if( _this->m_enablePort & __RRZO_ENABLE) {
            //enque
            NSCam::NSIoPipe::NSCamIOPipe::BufInfo OutBuf(PORT_RRZO, 0);
            dequeBufInfo.mvOut.push_back(OutBuf);
            MSize _cropsize = __CROP_SIZE(__RRZO,_this->m_prepalce_Buf[__RRZO]->getImgFormat(),_this->m_TgSize,_this->mPixMode);

            //replace
            MSize _rrz = __SCALE_SIZE(__RRZO,_this->m_prepalce_Buf[__RRZO]->getImgFormat(),_this->m_TgSize,RRZ_SCALING_RATIO,_this->mPixMode);
            printf("_rrz size : 0x%x 0x%x (0x%x 0x%x) (0x%x 0x%x) %d\n",_rrz.w,_rrz.h,_this->m_TgSize.w,_this->m_TgSize.h, _cropsize.w, _cropsize.h, _this->m_prepalce_Buf[__RRZO]->getImgFormat());
            //in replace testing , use mag num:7
            NSCam::NSIoPipe::NSCamIOPipe::BufInfo InBuf(PORT_RRZO, _this->m_prepalce_Buf[__RRZO],_rrz, MRect(__CROP_Start(_this->m_TgSize,_cropsize,_this->m_prepalce_Buf[__RRZO]->getImgFormat()),_cropsize),(replace_buf + Enque_buf + Dummy_buf));
            _replace.mvOut.push_back(InBuf);
        }


    int max_cnt;
    max_cnt=50;
    void *pTuningQueBuf;
    isp_reg_t *pIspPhyReg;
    MUINT32 size;
    MUINT32 curMagic=0;
    while(_loop<max_cnt){
        if(_this->m_bStop == MTRUE){
            printf("stop enque/deque\n");
            break;
        }
        _loop++;
        printf("[%d]_loop(%d)/max_cnt(%d)\n",_this->m_Sensoridx, _loop,max_cnt);
        if(_this->m_pNormalP_FrmB->deque(dequeBufInfo) == MTRUE){
            for(int i=0;i<dequeBufInfo.mvOut.size();i++){
                printf("[E_L deque]: %d_dma:0x%x, PA:0x%x, crop:0x%x,0x%x,0x%x,0x%x_0x%x,0x%x,0x%x,0x%x, size:0x%x,0x%x, mag:0x%x, dummy:0x%x, bufidx:0x%x\n",\
                _this->m_Sensoridx,
                dequeBufInfo.mvOut.at(i).mPortID.index,dequeBufInfo.mvOut.at(i).mBuffer->getBufPA(0),\
                dequeBufInfo.mvOut.at(i).mMetaData.mCrop_s.p.x,dequeBufInfo.mvOut.at(i).mMetaData.mCrop_s.p.y,\
                dequeBufInfo.mvOut.at(i).mMetaData.mCrop_s.s.w,dequeBufInfo.mvOut.at(i).mMetaData.mCrop_s.s.h,\
                dequeBufInfo.mvOut.at(i).mMetaData.mCrop_d.p.x,dequeBufInfo.mvOut.at(i).mMetaData.mCrop_d.p.y,\
                dequeBufInfo.mvOut.at(i).mMetaData.mCrop_d.s.w,dequeBufInfo.mvOut.at(i).mMetaData.mCrop_d.s.h,\
                dequeBufInfo.mvOut.at(i).mMetaData.mDstSize.w,dequeBufInfo.mvOut.at(i).mMetaData.mDstSize.h,\
                dequeBufInfo.mvOut.at(i).mMetaData.mMagicNum_tuning,dequeBufInfo.mvOut.at(i).mMetaData.m_bDummyFrame,\
                dequeBufInfo.mvOut.at(i).mBufIdx);
                curMagic=dequeBufInfo.mvOut.at(i).mMetaData.mMagicNum_tuning;
            }

            if(!Multi_thread)
            {
                //use replace buffer to enque,simulate new request in camera3
                for(int i=0;i<_replace.mvOut.size();i++){
                    printf("[E_L enque]: %d_dma:0x%x, PA:0x%x, crop:0x%x,0x%x,0x%x,0x%x, size:0x%x,0x%x, mag:0x%x, bufidx:0x%x\n",\
                    _this->m_Sensoridx,
                    _replace.mvOut.at(i).mPortID.index,_replace.mvOut.at(i).mBuffer->getBufPA(0),\
                    _replace.mvOut.at(i).FrameBased.mCropRect.p.x,_replace.mvOut.at(i).FrameBased.mCropRect.p.y,\
                    _replace.mvOut.at(i).FrameBased.mCropRect.s.w,_replace.mvOut.at(i).FrameBased.mCropRect.s.h,\
                    _replace.mvOut.at(i).FrameBased.mDstSize.w,_replace.mvOut.at(i).FrameBased.mDstSize.h,\
                    _replace.mvOut.at(i).FrameBased.mMagicNum_tuning,\
                    _replace.mvOut.at(i).mBufIdx);
                }
            }

            //usleep(30000);//sleep 10ms to cross over vsync and simulate 3a calculation
            //usleep(10000);//sleep 10ms to cross over vsync and simulate 3a calculation
            //if(_loop % 100)
            //    usleep(20000);//make drop frame status happened intenionally every 100 frames

            if(Multi_thread)    //only do deque in this thread
            {
                QBufInfo dequeBuf=dequeBufInfo;
                pthread_mutex_lock(&mutex);
                _this->m_dequeBufInfoList.push_back(dequeBuf);
                pthread_mutex_unlock(&mutex);
            }
            else
            {
                /////////
                _this->m_pNormalP_FrmB->enque(_replace);
                //update replace buffer to previous deque result
                _replace = dequeBufInfo;
                //note: must asign result to input
                for(int i=0;i<dequeBufInfo.mvOut.size();i++){
                    if(dequeBufInfo.mvOut.at(i).mPortID == PORT_IMGO)
                        _replace.mvOut.at(i).FrameBased.mCropRect = dequeBufInfo.mvOut.at(i).mMetaData.mCrop_d;
                    else if(dequeBufInfo.mvOut.at(i).mPortID == PORT_RRZO)
                        _replace.mvOut.at(i).FrameBased.mCropRect = dequeBufInfo.mvOut.at(i).mMetaData.mCrop_s;

                    _replace.mvOut.at(i).FrameBased.mDstSize = dequeBufInfo.mvOut.at(i).mMetaData.mDstSize;
                    _replace.mvOut.at(i).FrameBased.mMagicNum_tuning = dequeBufInfo.mvOut.at(i).mMetaData.mMagicNum_tuning;
                    _replace.mvOut.at(i).mBufIdx = 0xffff;//must be, caused bufidx is that a output data of deque, this value will be reset by line:294
                }
            }
        }
        else{
            MBOOL _break = MFALSE;
            //if deque fail is because of current dequed frame is dummy frame, bypass this error return.
            for(int i=0;i<dequeBufInfo.mvOut.size();i++){
                if(dequeBufInfo.mvOut.at(i).mMetaData.m_bDummyFrame == MFALSE)
                    _break = MTRUE;
                else
                    printf("[%d]===  get dummy frame(0x%x) ===\n", _this->m_Sensoridx,dequeBufInfo.mvOut.at(i).mPortID);
            }
            if(_break == MTRUE){
                printf("[%d]deque fail, leave infinite_loop\n", _this->m_Sensoridx);
                break;
            }
        }
    }


    ////////////////////////////////////
    //debug
    #if 0
    IspDrv* mpIspDrv=NULL;
    mpIspDrv=IspDrv::createInstance();
    mpIspDrv->init(LOG_TAG);

    printf("bbbbbbbbbbe for dma_en(0x%x)",mpIspDrv->readReg(0x400c));
    mpIspDrv->writeReg(0x4094,0x00000400,ISP_DRV_USER_ISPF);
    printf("afffffffffffffff for dma_en(0x%x)",mpIspDrv->readReg(0x400c));

    mpIspDrv->uninit(LOG_TAG);
    mpIspDrv->destroyInstance();
    #endif
    ////////////////////////////////////

    _this->m_bStop=true;
    ::sem_post(&_this->m_semThread);
    return NULL;
}



/******************************************************************************
* another thread for pass2 operation test
*******************************************************************************/
MVOID* _test_camio_::_p2ThreadLoop(MVOID *arg)
{
    //[1] set thread
    // set thread name
    ::prctl(15,"p2ThreadLoop",0,0,0);
    // set policy/priority
    int const policy    = SCHED_OTHER;
    int const priority    = NICE_CAMERA_PASS2;
    int nTuningMode = 0;//
    //
    struct sched_param sched_p;
    ::sched_getparam(0, &sched_p);
    if(policy == SCHED_OTHER)
    {    //    Note: "priority" is nice-value priority.
        sched_p.sched_priority = 0;
        ::sched_setscheduler(0, policy, &sched_p);
        ::setpriority(PRIO_PROCESS, 0, priority);
    }
    else
    {    //    Note: "priority" is real-time priority.
        sched_p.sched_priority = priority;
        ::sched_setscheduler(0, policy, &sched_p);
    }
    //
    LOG_INF(
        "policy:(expect, result)=(%d, %d), priority:(expect, result)=(%d, %d)"
        , policy, ::sched_getscheduler(0)
        , priority, sched_p.sched_priority
    );
    //    detach thread => cannot be join, it means that thread would release resource after exit
    ::pthread_detach(::pthread_self());


    _test_camio_* _this = (_test_camio_*)arg;
    QBufInfo p1dequeBufInfo;
    QBufInfo _replace;
    _replace.mvOut.clear();
    p1dequeBufInfo.mvOut.reserve(2);
        if( _this->m_enablePort & __IMGO_ENABLE) {
            //enque
            NSCam::NSIoPipe::NSCamIOPipe::BufInfo OutBuf(PORT_IMGO, 0);
            p1dequeBufInfo.mvOut.push_back(OutBuf);
            MSize _cropsize = __CROP_SIZE(__IMGO,_this->m_prepalce_Buf[__IMGO]->getImgFormat(),_this->m_TgSize,_this->mPixMode);

            //replace
            //in replace testing , use mag num:7
            NSCam::NSIoPipe::NSCamIOPipe::BufInfo InBuf(PORT_IMGO, _this->m_prepalce_Buf[__IMGO],_cropsize,MRect(__CROP_Start(_this->m_TgSize,_cropsize,_this->m_prepalce_Buf[__IMGO]->getImgFormat()),_cropsize),(replace_buf + Enque_buf + Dummy_buf));
            _replace.mvOut.push_back(InBuf);
        }
        if( _this->m_enablePort & __RRZO_ENABLE) {
            //enque
            NSCam::NSIoPipe::NSCamIOPipe::BufInfo OutBuf(PORT_RRZO, 0);
            p1dequeBufInfo.mvOut.push_back(OutBuf);
            MSize _cropsize = __CROP_SIZE(__RRZO,_this->m_prepalce_Buf[__RRZO]->getImgFormat(),_this->m_TgSize,_this->mPixMode);

            //replace
            MSize _rrz = __SCALE_SIZE(__RRZO,_this->m_prepalce_Buf[__RRZO]->getImgFormat(),_this->m_TgSize,RRZ_SCALING_RATIO,_this->mPixMode);
            printf("_rrz size : 0x%x 0x%x (0x%x 0x%x) (0x%x 0x%x) %d\n",_rrz.w,_rrz.h,_this->m_TgSize.w,_this->m_TgSize.h, _cropsize.w, _cropsize.h, _this->m_prepalce_Buf[__RRZO]->getImgFormat());
            //in replace testing , use mag num:7
            //note, img2o crop is after scaler, so input is hrz out
            NSCam::NSIoPipe::NSCamIOPipe::BufInfo InBuf(PORT_RRZO, _this->m_prepalce_Buf[__RRZO],_rrz,MRect(__CROP_Start(_this->m_TgSize,_cropsize,_this->m_prepalce_Buf[__RRZO]->getImgFormat()),_cropsize),(replace_buf + Enque_buf + Dummy_buf));
            _replace.mvOut.push_back(InBuf);
        }


    int cnt=0;
    //////////////////////////////////////////////
    //pass2 obj
    NSCam::NSIoPipe::NSPostProc::INormalStream* pStreamObj;
    pStreamObj= NSCam::NSIoPipe::NSPostProc::INormalStream::createInstance("_p2ThreadLoop", NSCam::NSIoPipe::NSPostProc::ENormalStreamTag_Prv,0, bEnTuning);

    pStreamObj->init();
    IMemDrv* pImemDrv=IMemDrv::createInstance();
    pImemDrv->init();
    IspDrv* mpIspDrv=NULL;
    mpIspDrv=IspDrv::createInstance();
    mpIspDrv->init(LOG_TAG);
    //////////////////////////////////////////////
    MRect p1SrcCrop;
    MSize p1Dst;
    MRect p1DstCrop;

    int _loop=0;
    void *pTuningQueBuf;
    isp_reg_t *pIspPhyReg;
    MUINT32 size;
    MUINT32 curMagic=0;
    bool bwait=false;
    bool deqret=true;
    while(1)
    {
        _loop+=1;
        bwait=false;
        //wait get dequeued buffer from pass1
        pthread_mutex_lock(&mutex);
        if(_this->m_dequeBufInfoList.size()>0)
        {
            printf("===m_dequeBufInfoList size(%d)=== \n",_this->m_dequeBufInfoList.size());
            //get first one for pass2 operation
            list<QBufInfo>::iterator t = _this->m_dequeBufInfoList.begin();
            p1dequeBufInfo=(*t);
            _this->m_dequeBufInfoList.erase(t);
            pthread_mutex_unlock(&mutex);
        }
        else
        {
            pthread_mutex_unlock(&mutex);

            bwait=true;

        }

        if(bwait)
        {
            if(_this->m_bStop == MTRUE)
            {
                printf("[%d]stop p2 operation, exit\n", _this->m_Sensoridx);
                break;
            }
            //printf("waiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiit 30ms  \n");
            usleep(30000);    //sleep 10 ms
        }
        else
        {
            //printf("===p1dequeBufInfo.mvOut size(%d)=== \n",p1dequeBufInfo.mvOut.size());
            //pass2 operation if get pass1 output
            for(int i=0;i<p1dequeBufInfo.mvOut.size();i++)
            {
                if(i==0)
                {
                    curMagic=p1dequeBufInfo.mvOut.at(i).mMetaData.mMagicNum_tuning;
                    MUINT32 a,b;
                    a=getUs();
                    MINT32 t1,t2,t3,t4,t5,t2_2;
                    {    //life cycle for variable
                        MINT32 bufBoundaryInBytes[3] = {0, 0, 0};
                        //get pass1 out as pass2 in
                        QParams enqueParams;
                        enqueParams.mvPrivaData.push_back(NULL);
                        enqueParams.mvMagicNo.push_back(0);

                        //p1 private data for sl2
                        printf("===magic(0x%x) p1crop_s x/y/w/h(%d/%d/%d/%d), dst w/h(%d/%d), crop_d x/y/w/h(%d/%d/%d/%d)=== \n",curMagic,\
                            p1dequeBufInfo.mvOut.at(i).mMetaData.mCrop_s.p.x,p1dequeBufInfo.mvOut.at(i).mMetaData.mCrop_s.p.y,\
                            p1dequeBufInfo.mvOut.at(i).mMetaData.mCrop_s.s.w,\
                            p1dequeBufInfo.mvOut.at(i).mMetaData.mCrop_s.s.h,p1dequeBufInfo.mvOut.at(i).mMetaData.mDstSize.w, p1dequeBufInfo.mvOut.at(i).mMetaData.mDstSize.h,\
                            p1dequeBufInfo.mvOut.at(i).mMetaData.mCrop_d.p.x,p1dequeBufInfo.mvOut.at(i).mMetaData.mCrop_d.p.y,\
                            p1dequeBufInfo.mvOut.at(i).mMetaData.mCrop_d.s.w,p1dequeBufInfo.mvOut.at(i).mMetaData.mCrop_d.s.h);

                        p1SrcCrop.s.w = p1dequeBufInfo.mvOut.at(i).mMetaData.mCrop_s.s.w;//1280;
                        p1SrcCrop.s.h =  p1dequeBufInfo.mvOut.at(i).mMetaData.mCrop_s.s.h;//720;
                        p1Dst.w =p1dequeBufInfo.mvOut.at(i).mMetaData.mDstSize.w;// 1260;
                        p1Dst.h =  p1dequeBufInfo.mvOut.at(i).mMetaData.mDstSize.h;//708;
                        p1DstCrop.p.x =  p1dequeBufInfo.mvOut.at(i).mMetaData.mCrop_d.p.x;//10;
                        p1DstCrop.p.y =  p1dequeBufInfo.mvOut.at(i).mMetaData.mCrop_d.p.y;//6;
                        enqueParams.mvP1SrcCrop.push_back(p1SrcCrop);
                        enqueParams.mvP1Dst.push_back(p1Dst);
                        enqueParams.mvP1DstCrop.push_back(p1DstCrop);

                            #if 1

                        printf("[%d]p2 vInport fmt(0x%x)\n", _this->m_Sensoridx, p1dequeBufInfo.mvOut.at(i).mBuffer->getImgFormat());
                        Input src;
                        src.mPortID=PORT_IMGI;
                        src.mBuffer=p1dequeBufInfo.mvOut.at(i).mBuffer;
                        src.mPortID.group=0;
                        enqueParams.mvIn.push_back(src);

                        //crop/resize information
                        int out_w=p1dequeBufInfo.mvOut.at(i).mMetaData.mCrop_d.s.w;
                        int out_h=p1dequeBufInfo.mvOut.at(i).mMetaData.mCrop_d.s.h;
                        //printf("pass2 w/h (%d/%d) \n",out_w,out_h);
                        MCrpRsInfo crop;
                        crop.mGroupID=1;
                        crop.mCropRect.p_fractional.x=0;
                        crop.mCropRect.p_fractional.y=0;
                        crop.mCropRect.p_integral.x=0;
                        crop.mCropRect.p_integral.y=0;
                        crop.mCropRect.s.w=out_w;
                        crop.mCropRect.s.h=out_h;
                        crop.mResizeDst.w=out_w;
                        crop.mResizeDst.h=out_h;
                        enqueParams.mvCropRsInfo.push_back(crop);
                        MCrpRsInfo crop_2;
                        crop_2.mGroupID=2;
                        int temp=0;
                        int out_w_2=0,out_h_2=0;
                        if(bEnZoom)
                        {
                            switch((_loop%3))
                            {
                                case 0:
                                default:    //only crop
                                    crop_2.mCropRect.p_fractional.x=0;
                                    crop_2.mCropRect.p_fractional.y=0;
                                    crop_2.mCropRect.p_integral.x=100;
                                    crop_2.mCropRect.p_integral.y=100;
                                    crop_2.mCropRect.s.w=out_w-200;
                                    crop_2.mCropRect.s.h=out_h-200;
                                    crop_2.mResizeDst.w=out_w-200;
                                    crop_2.mResizeDst.h=out_h-200;
                                    out_w=out_w-200;
                                    out_h=out_h-200;
                                    out_w_2=out_w;
                                    out_h_2=out_h;
                                    break;
                                case 1:    //crop then zoom in
                                    crop_2.mCropRect.p_fractional.x=0;
                                    crop_2.mCropRect.p_fractional.y=0;
                                    crop_2.mCropRect.p_integral.x=100;
                                    crop_2.mCropRect.p_integral.y=100;
                                    crop_2.mCropRect.s.w=out_w-200;
                                    crop_2.mCropRect.s.h=out_h-200;
                                    crop_2.mResizeDst.w=out_w;
                                    crop_2.mResizeDst.h=out_h;
                                    out_w_2=out_w;
                                    out_h_2=out_h;
                                    break;
                                case 2: //crop then zoom out, rotate 90
                                    crop_2.mCropRect.p_fractional.x=0;
                                    crop_2.mCropRect.p_fractional.y=0;
                                    crop_2.mCropRect.p_integral.x=100;
                                    crop_2.mCropRect.p_integral.y=100;
                                    crop_2.mCropRect.s.w=out_w-200;
                                    crop_2.mCropRect.s.h=out_h-200;
                                    crop_2.mResizeDst.w=out_w/2;
                                    crop_2.mResizeDst.h=out_h/2;
                                    out_w = out_w/2;
                                    out_h=out_h/2;
                                    out_w_2=out_h;
                                    out_h_2=out_w;
                                    break;
                            }
                        }
                        else
                        {
                            crop_2.mCropRect.p_fractional.x=0;
                            crop_2.mCropRect.p_fractional.y=0;
                            crop_2.mCropRect.p_integral.x=0;
                            crop_2.mCropRect.p_integral.y=0;
                            crop_2.mCropRect.s.w=out_w;
                            crop_2.mCropRect.s.h=out_h;
                            crop_2.mResizeDst.w=out_w;
                            crop_2.mResizeDst.h=out_h;
                            out_w_2=out_w;
                            out_h_2=out_h;
                        }
                        enqueParams.mvCropRsInfo.push_back(crop_2);

                        //prepare output dma
                        IMEM_BUF_INFO wdmaoBuf;
                        wdmaoBuf.size=out_w*out_h*2;
                            pImemDrv->allocVirtBuf(&wdmaoBuf);
                        memset((MUINT8*)wdmaoBuf.virtAddr, 0xffffffff, wdmaoBuf.size);
                        MUINT32 bufStridesInBytes_2[3] = {out_w, out_w/2, out_w/2};
                        PortBufInfo_v1 portBufInfo_2 = PortBufInfo_v1( wdmaoBuf.memID,wdmaoBuf.virtAddr,0,wdmaoBuf.bufSecu, wdmaoBuf.bufCohe);
                            IImageBufferAllocator::ImgParam imgParam_2 = IImageBufferAllocator::ImgParam((eImgFmt_YV12),
                                                                            MSize(out_w,out_h),  bufStridesInBytes_2, bufBoundaryInBytes, 3);
                        sp<ImageBufferHeap> pHeap_2 = ImageBufferHeap::create( "onThreadLoop", imgParam_2,portBufInfo_2,true);
                        IImageBuffer* outBuffer = pHeap_2->createImageBuffer();
                            outBuffer->incStrong(outBuffer);
                            outBuffer->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
                        Output dst;
                        dst.mPortID=PORT_WDMAO;
                        dst.mBuffer=outBuffer;
                        dst.mPortID.group=0;
                        enqueParams.mvOut.push_back(dst);
                        //
                        IMEM_BUF_INFO wrotoBuf;
                        wrotoBuf.size=out_w_2*out_h_2*2;
                            pImemDrv->allocVirtBuf(&wrotoBuf);
                        memset((MUINT8*)wrotoBuf.virtAddr, 0x0, wrotoBuf.size);
                        MUINT32 bufStridesInBytes_4[3] = {out_w_2,out_w_2/2,out_w_2/2};
                        PortBufInfo_v1 portBufInfo_4 = PortBufInfo_v1( wrotoBuf.memID,wrotoBuf.virtAddr,0,wrotoBuf.bufSecu, wrotoBuf.bufCohe);
                            IImageBufferAllocator::ImgParam imgParam_4 = IImageBufferAllocator::ImgParam((eImgFmt_YV12),
                                                                            MSize(out_w_2, out_h_2),  bufStridesInBytes_4, bufBoundaryInBytes, 3);
                        sp<ImageBufferHeap> pHeap_4 = ImageBufferHeap::create( "pass2_loopTest", imgParam_4,portBufInfo_4,true);
                        IImageBuffer* outBuffer3 = pHeap_4->createImageBuffer();
                            outBuffer3->incStrong(outBuffer3);
                            outBuffer3->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
                        Output dst3;
                        dst3.mPortID=PORT_WROTO;
                        dst3.mBuffer=outBuffer3;
                        dst3.mPortID.group=0;
                        if(bEnZoom)
                        {
                            switch((_loop%3))
                            {
                                case 2:
                                    dst3.mTransform=eTransform_ROT_90;
                                    break;
                                default:
                                    break;
                            }
                        }
                        enqueParams.mvOut.push_back(dst3);
                        t1=getUs();
                        //printf("===== p2 stage1 period(%d us) =====\n",t1-a);


                        //pass2 buffer operation
                        pImemDrv->cacheFlushAll();
                        t2=getUs();
                        //printf("===== p2 flush period(%d us) =====\n",t2-t1);


                        ////////////////////////////////////////////
                        //pass2 tuning setting
                        if(bEnTuning)
                        {
                            nTuningMode = (_loop%3);
                            //pStreamObj->deTuningQue(size, pTuningQueBuf);
                            pIspPhyReg = (isp_reg_t *)pTuningQueBuf;
                            setP2Tuning(nTuningMode, &pIspPhyReg);
                            enqueParams.mvTuningData.push_back(pTuningQueBuf);
                        }
                        t2_2=getUs();
                        //printf("===== p2 tuning setting period(%d us) =====\n",t2_2-t2);
                        ////////////////////////////////////////////

                        //LOG_INF("pass2 enque\n");
                        pStreamObj->enque(enqueParams);

                        if(bEnTuning)
                        {
                            // return tuningque buffer
                            //pStreamObj->enTuningQue(pTuningQueBuf);
                        }

                        t3=getUs();
                        //printf("===== p2 enque period(%d us) =====\n",t3-t2_2);


                        QParams dequeParams;
                        //LOG_INF("pass2 deque\n");
                        deqret=pStreamObj->deque(dequeParams);
                        if(deqret==MFALSE)
                        {
                            printf("[%d]===== p2 deque faillllllllllllllllllllllllllllllllll =====\n",_this->m_Sensoridx);
                        }

                        t4=getUs();
                        //printf("===== p2 deque period(%d us) =====\n",t4-t3);

                        //////////////////////////////////////////////////////////////////
                        //check p2 tuning setting if needed
                        if(bEnTuning)
                        {
                            printf("[%d]=====================   p2 tuning =================\n",_this->m_Sensoridx);
                            printf("magic (%d), bdummy(%d), nTuningMode(%d)\n",p1dequeBufInfo.mvOut.at(0).mMetaData.mMagicNum_tuning,p1dequeBufInfo.mvOut.at(0).mMetaData.m_bDummyFrame, nTuningMode);

                                printf("0x15005480 (0x%x)\n",mpIspDrv->readReg(0x5480));//CAM_GGM_CTRL
                                printf("0x15005000 (0x%x)\n",mpIspDrv->readReg(0x5000));//CAM_GGM_LUT_RB 5000..523C
                                printf("0x15005004 (0x%x)\n",mpIspDrv->readReg(0x5004));//
                                printf("0x15005008 (0x%x)\n",mpIspDrv->readReg(0x5008));
                                printf("0x1500500c (0x%x)\n",mpIspDrv->readReg(0x500c));
                                printf("0x15005010 (0x%x)\n",mpIspDrv->readReg(0x5010));
                                printf("0x15005014 (0x%x)\n",mpIspDrv->readReg(0x5014));
                                printf("0x15005018 (0x%x)\n",mpIspDrv->readReg(0x5018));
                                printf("0x1500501c (0x%x)\n",mpIspDrv->readReg(0x501c));
                                printf("0x15005020 (0x%x)\n",mpIspDrv->readReg(0x5020));
                                printf("0x15005024 (0x%x)\n",mpIspDrv->readReg(0x5024));
                                printf("0x15005028 (0x%x)\n",mpIspDrv->readReg(0x5028));
                                printf("0x1500502c (0x%x)\n",mpIspDrv->readReg(0x502c));
                                printf("0x15005030 (0x%x)\n",mpIspDrv->readReg(0x5030));
                                printf("0x15005034 (0x%x)\n",mpIspDrv->readReg(0x5034));
                                printf("0x15005038 (0x%x)\n",mpIspDrv->readReg(0x5038));
                                printf("0x1500503c (0x%x)\n",mpIspDrv->readReg(0x503c));
                                printf("0x15005040 (0x%x)\n",mpIspDrv->readReg(0x5040));
                                printf("0x15005044 (0x%x)\n",mpIspDrv->readReg(0x5044));
                                printf("0x15005048 (0x%x)\n",mpIspDrv->readReg(0x5048));
                                printf("0x1500504c (0x%x)\n",mpIspDrv->readReg(0x504c));
                                printf("0x15005050 (0x%x)\n",mpIspDrv->readReg(0x5050));
                                printf("0x15005054 (0x%x)\n",mpIspDrv->readReg(0x5054));
                                printf("0x15005058 (0x%x)\n",mpIspDrv->readReg(0x5058));
                                printf("0x1500505c (0x%x)\n",mpIspDrv->readReg(0x505c));
                                printf("0x15005060 (0x%x)\n",mpIspDrv->readReg(0x5060));

                            printf("================================================\n");
                        }
                        /////////////////////////////////////////////////////////////////
                        //save pass2 output
                        //LOG_INF("pass2 deuqedOutsize (%d)",dequeParams.mvOut.size());
                        char filename[256];
                        sprintf(filename, "/data/IT/IT_loop_%d_input_%dx%d_%d_magic_0x%x_%d.raw",_loop,enqueParams.mvIn[0].mBuffer->getImgSize().w,\
                            enqueParams.mvIn[0].mBuffer->getImgSize().h, enqueParams.mvIn[0].mBuffer->getBufStridesInBytes(0) ,curMagic,_this->m_Sensoridx);
                        saveBufToFile(filename, reinterpret_cast<MUINT8*>(enqueParams.mvIn[0].mBuffer->getBufVA(0)),\
                                enqueParams.mvIn[0].mBuffer->getBufStridesInBytes(0) *enqueParams.mvIn[0].mBuffer->getImgSize().h);
                            //output
                        for(int j=0;j<dequeParams.mvOut.size();j++)
                        {
                            if(j==0)
                            {
                                sprintf(filename, "/data/IT/IT_loop_%d_output_%d_%dx%d_magic_0x%x_%d.yuv",_loop,j,out_w,out_h,curMagic,_this->m_Sensoridx);
                                    saveBufToFile(filename, reinterpret_cast<MUINT8*>(dequeParams.mvOut[j].mBuffer->getBufVA(0)), out_w *out_h * 2);
                                }
                                else if(j==1)
                                {
                                    sprintf(filename, "/data/IT/IT_loop_%d_output_%d_%dx%d_magic_0x%x_%d.yuv",_loop,j,out_w_2,out_h_2,curMagic,_this->m_Sensoridx);
                                    saveBufToFile(filename, reinterpret_cast<MUINT8*>(dequeParams.mvOut[j].mBuffer->getBufVA(0)), out_w_2 *out_h_2 * 2);
                                }

                        }
                        t5=getUs();
                        //printf("===== p2 save file period(%d us) =====\n",t5-t4);
                        #else
                        usleep(20000);
                        #endif
                    }
                    b=getUs();
                    printf("[%d] ===== p2 all period(%d us) =====\n", _this->m_Sensoridx,b-a);

                    //enque buffer back to pass1
                    //for carson
                    //NormalPipe_FRM_STATUS _drop_status;
                    //_this->m_pNormalP_FrmB->sendCommand(EPIPECmd_GET_CUR_FRM_STATUS,(MINT32)&_drop_status,0,0);
                    //while(_drop_status == _drop_frame_status){
                    //    usleep(1000);
                    //}
                    _this->m_pNormalP_FrmB->enque(_replace);
                    //update replace buffer to previous deque result
                    _replace = p1dequeBufInfo;
                    //note: must asign result to input
                    for(int i=0;i<p1dequeBufInfo.mvOut.size();i++){
                        if(p1dequeBufInfo.mvOut.at(i).mPortID == PORT_IMGO)
                            _replace.mvOut.at(i).FrameBased.mCropRect = p1dequeBufInfo.mvOut.at(i).mMetaData.mCrop_d;
                        else if(p1dequeBufInfo.mvOut.at(i).mPortID == PORT_RRZO)
                            _replace.mvOut.at(i).FrameBased.mCropRect = p1dequeBufInfo.mvOut.at(i).mMetaData.mCrop_s;
                        _replace.mvOut.at(i).FrameBased.mDstSize = p1dequeBufInfo.mvOut.at(i).mMetaData.mDstSize;
                        _replace.mvOut.at(i).FrameBased.mMagicNum_tuning = p1dequeBufInfo.mvOut.at(i).mMetaData.mMagicNum_tuning;
                        _replace.mvOut.at(i).mBufIdx = 0xffff;//must be, caused bufidx is that a output data of deque, this value will be reset by line:294
                    }
                    printf("[%d]===== p2 enque buffer back to p1 =====\n", _this->m_Sensoridx);

                    break;
                }
            }
        }
    }

    //////////////////////////////////////////////////////
    //pass2 obj
    pImemDrv->uninit();
    pImemDrv->destroyInstance();
    mpIspDrv->uninit(LOG_TAG);
    mpIspDrv->destroyInstance();
    pStreamObj->uninit();
    pStreamObj->destroyInstance("onThreadLoop");
    //////////////////////////////////////////////////////
    ::sem_post(&mLeaveSem);

    return NULL;
}


/******************************************************************************
*******************************************************************************/
/*******************************************************************************
*  basicpreview: single thread p1/p2 IT, type0: pass1 1 out. type1: pass1 two out
*
*  [Example] iopipetest 4 <testcase num> <sub-testcase> <testcaseType>
*            iopipetest 4 0 0 1: multi-thread w/o tuning
*            iopipetest 4 1 0 1: multi-thread with tuning
*            iopipetest 4 2 0 2: multi-thread with crop/resize function (imgo out), rrzo output with rrz setting
*
* * [ testcaseNum ] * *
*  - 0: p1-p2 multi thread w/o tuning
*  - 1: p1-p2 multi thread w tuning, and pass rrz crop setting (from p1 to p2)
*  - 2: p1-p2 multi thread w different crop setting (pass1 output cropped and non-croppped result, pass2 adopt different crop/resize)
*  - 3: p1-p2 multi thread w arbitray Crop /no tuning/ no zoom
*
* * [ testcaseType ] * * (can't open imgo only with RAW sensor)
*  x type 0: preview pass1 1 out[imgo]
*  - type 1: preview pass1 1 out[rrzo]
*  - type 2: preview pass1 2 out[imgo+rrzo]
*------------------------------------------------------------
*  x type 3: preview pass1 1 out[imgo]          dual sensor
*  - type 4: preview pass1 1 out[rrzo]          dual sensor
*  - type 5: preview pass1 2 out[imgo+rrzo]     dual sensor
*------------------------------------------------------------
*  x type 6: capture pass1 1 out[imgo]
*  - type 7: capture pass1 1 out[rrzo]
*  - type 8: capture pass1 2 out[imgo+rrzo]
*
********************************************************************************/
int p1p2_BasicIT(int argc, char** argv)
{
    LOG_INF("+");
    int ret = 0;

    if( argc < 3 )
    {
        printf("Usage: iopipetest 4 <0/1:tuning/2:scaling> <0/1:test rrz>\n");
        return -1;
    }

    int testcase = atoi(argv[1]);//0: no scaling/ 1:tuning/ 2:scaling
    int testcaseNum = atoi(argv[2]);
    int testcaseType = atoi(argv[3]);//type

    switch(testcase)
    {
        case 0:
            ret=basic_flow(testcaseNum,testcaseType);
            break;
        case 1:
            ret=basic_flow_tuning(testcaseNum,testcaseType);
            break;
        case 2:
            ret=basic_flow_crop_resize(testcaseNum,testcaseType);
            break;
        default:
            break;
    }

    return ret;
}


/*******************************************************************************
*  basicpreview
*  iopipetest 4 0 <num> <type>
* ------------------------------------------
*  num: 0: no tuning/ no zoom
*  num: 1: tuning/no zoom
*  num: 2: no tuning/zoom
*  num: 3: arbitray Crop /no tuning/ no zoom
* ------------------------------------------
*  x type 0: preview pass1 1 out[imgo]
*  - type 1: preview pass1 1 out[rrzo]
*  - type 2: preview pass1 2 out[imgo+rrzo]
*  x type 3: preview pass1 1 out[imgo]          dual sensor
*  - type 4: preview pass1 1 out[rrzo]          dual sensor
*  - type 5: preview pass1 2 out[imgo+rrzo]     dual sensor
*  x type 6: capture pass1 1 out[imgo]
*  - type 7: capture pass1 1 out[rrzo]
*  - type 8: capture pass1 2 out[imgo+rrzo]
********************************************************************************/
int basic_flow(int num,int type)
{
    int ret=0;
    runningcase=num;
    bEnTuning=false;
    bEnZoom=false;
    bArbCrop=false;

    switch(runningcase)
    {
        case 0:
        default:
            bEnTuning=false;
            break;
        case 1:
            bEnTuning=true;
            break;
        case 2:
            bEnZoom=true;
            break;
        case 3:
            bArbCrop=true;
            break;
    }
    IHalSensorList* const pHalSensorList = IHalSensorList::get();
    pHalSensorList->searchSensors();
    int scenario=0;
    MUINT32 dualSensor=1; // 1 for single , 2 for dual sensor, if dualSensor == 1 , always use main sensor
    MUINT32 enablePort=0x0;
    IMemDrv* mpImemDrv=NULL;
    mpImemDrv=IMemDrv::createInstance();
    mpImemDrv->init();
    sem_init(&mbufferSem, 0, 0);
    sem_init(&mLeaveSem, 0, 0);
    bLeaveP2=false;
    switch(type){
 //       case 0: //prv_fps30_imgo
 //           scenario = 0;
 //           enablePort = 0x2;
 //           break;
        case 1: //prv_fps30_rrzo
            scenario = 0;
            enablePort = 0x1;
            break;
        case 2: //prv_fps30_imgo+rrzo
            scenario = 0;
            enablePort = 0x3;
            break;
  //      case 3: //prv_fps30_imgo_dualSensor
  //          scenario = 0;
  //          enablePort = 0x2;
  //          dualSensor = 2;
  //          break;
        case 4: //prv_fps30_rrzo_dualSensor
            scenario = 0;
            enablePort = 0x1;
            dualSensor = 2;
            break;
        case 5: //prv_fps30_rrzo+imgo_dualSensor
            scenario = 0;
            enablePort = 0x3;
            dualSensor = 2;
            break;
   //     case 6: //cap_fps30_imgo
   //         scenario = 1;
   //         enablePort = 0x2;
   //         break;
        case 7: //cap_fps30_rrzo
            scenario = 1;
            enablePort = 0x1;
            break;
        case 8: //cap_fps30_imgo+rrzo
            scenario = 1;
            enablePort = 0x3;
            break;
        default:
            LOG_ERR("unsupported testcaseNum0x%x\n",type);
            return 0;
            break;
    }
    MUINT32 sensorIdx = 0;    //0 for main.
    MUINT32 sensorFps = 30;    //30 for 30fps
    LOG_INF("sensorIdx:0x%x,senario:0x%x,sensorFps:0x%x,enablePort = 0x%x\n",sensorIdx,scenario,sensorFps,enablePort);

    /////////////////////////////////////////
    //sensor operation
    MUINT32    sensorArray[2];
    SensorStaticInfo mSensorInfo[2];
	if(dualSensor > 2)
		dualSensor = 2;

	//powerOn sensor
	IHalSensor* pSensorHalObj[_MAXTG_] = {NULL};
	for(int i=0;i<dualSensor;i++){
		pSensorHalObj[i] = pHalSensorList->createSensor(LOG_TAG, i);
	    if(pSensorHalObj == NULL)
	    {
	       LOG_ERR("mpSensorHalObj is NULL");
	    }
	    //
	    sensorArray[i] = i;
	    pSensorHalObj[i]->powerOn(LOG_TAG, 1, &sensorArray[i]);
    }
	//

	//query sensor
	for(int i=0;i<dualSensor;i++){
	    IHalSensorList* const pHalSensorList = IHalSensorList::get();
	    pHalSensorList->querySensorStaticInfo(
	            pHalSensorList->querySensorDevIdx(i),
	            &mSensorInfo[i]);
	}
	//////////////////////////////////////////////////////////////////////
	//pass1 operation
	//create/init normalpipe_frmb
	INormalPipe_FrmB* mpCamIO[2] = {NULL,NULL};
	for(int i=0;i<dualSensor;i++){
		mpCamIO[i] = INormalPipe_FrmB::createInstance((MINT32)i, LOG_TAG,1);
		mpCamIO[i]->init();
	}

	//prepare sensor cfg
	MSize sensorSize[2];
	vector<IHalSensor::ConfigParam> vSensorCfg[2];
	for(int i=0;i<dualSensor;i++){
		getSensorSize_2(&sensorSize[i],mSensorInfo[i],scenario);
		IHalSensor::ConfigParam sensorCfg =
	    {
	        (MUINT)i,          				/* index            */
	        sensorSize[i],                  /* crop             */
	        scenario,            			/* scenarioId       */
	        0,                              /* isBypassScenario */
	        1,                              /* isContinuous     */
	        MFALSE, 						/* iHDROn           */
	        sensorFps,           			/* framerate        */
	        0,                              /* two pixel on     */
	        0,                              /* debugmode        */
	    };
	    vSensorCfg[i].push_back(sensorCfg);
	    LOG_INF("senidx:0x%x, sensor %dx%d, sce %d, bypass %d, con %d, hdr %d, fps %d, twopxl %d\n",
	    			i,
	                sensorCfg.crop.w,
	                sensorCfg.crop.h,
	                sensorCfg.scenarioId,
	                sensorCfg.isBypassScenario,
	                sensorCfg.isContinuous,
	                sensorCfg.HDRMode,
	                sensorCfg.framerate,
	                sensorCfg.twopixelOn);
	}
	////////////////////////////////////////////////////////////////////////
	//isp dmao cfg
	LOG_INF("start isp dmao cfg\n");
	EImageFormat fmt;
    NSImageio::NSIspio::ISP_QUERY_RST queryRst;
    MUINT32 pixelMode[2] = {0};
    MSize _cropsize;
    NSImageio::NSIspio::E_ISP_PIXMODE e_PixMode[2];
    list<HwPortConfig_t> lHwPortCfg[2];
	for(int i=0;i<dualSensor;i++){
		getSensorPixelMode_2(&pixelMode[i],i,scenario,sensorFps);
		e_PixMode[i] =  ((pixelMode[i] == 0) ?  (NSImageio::NSIspio::ISP_QUERY_1_PIX_MODE) : (NSImageio::NSIspio::ISP_QUERY_2_PIX_MODE));
		if(enablePort & __IMGO_ENABLE){
			LOG_INF("senidx:0x%x:port IMGO\n",i);
			if( !getOutputFmt_2(PORT_IMGO,SEN_PIX_BITDEPTH,mSensorInfo[i], &fmt))
				LOG_INF("get pix fmt error\n");
		    NSImageio::NSIspio::ISP_QuerySize(
		                            NSImageio::NSIspio::EPortIndex_IMGO,
		                            NSImageio::NSIspio::ISP_QUERY_X_PIX|
		                            NSImageio::NSIspio::ISP_QUERY_STRIDE_PIX|
		                            NSImageio::NSIspio::ISP_QUERY_STRIDE_BYTE,
		                            fmt,
		                            sensorSize[i].w,
		                            queryRst,
		                            e_PixMode[i]
		                            );
		    if(bEnZoom)
            {
	           _cropsize = __CROP_SIZE_2(__IMGO,fmt,sensorSize[i],e_PixMode[i]);
		    }
    		else
    		{
		        _cropsize = __CROP_SIZE(__IMGO,fmt,sensorSize[i],e_PixMode[i]);
	    	}
		    HwPortConfig_t full = {
		        PORT_IMGO,
		        fmt,
				_cropsize,//size
		        MRect(__CROP_Start(sensorSize[i],_cropsize,fmt),_cropsize),//crop
		        0, //if raw type != 1 -> pure-raw
		        { queryRst.stride_byte, 0, 0 }
		    };
		    lHwPortCfg[i].push_back(full);
	    }
	    if(enablePort & __RRZO_ENABLE){
	    	LOG_INF("senidx:0x%x:port RRZO\n",i);
	    	MSize _size;
			if( !getOutputFmt_2(PORT_RRZO,SEN_PIX_BITDEPTH,mSensorInfo[i], &fmt))
				LOG_INF("get pix fmt error\n");

			_size = __SCALE_SIZE(__RRZO,fmt,sensorSize[i],RRZ_SCALING_RATIO,e_PixMode[i]);
            LOG_INF("SCALE_SIZE: _size(%dx%d)\n", _size.w, _size.h);
		    NSImageio::NSIspio::ISP_QuerySize(
		                            NSImageio::NSIspio::EPortIndex_RRZO,
		                            NSImageio::NSIspio::ISP_QUERY_X_PIX|
		                            NSImageio::NSIspio::ISP_QUERY_STRIDE_PIX|
		                            NSImageio::NSIspio::ISP_QUERY_STRIDE_BYTE,
		                            fmt,
		                            _size.w,
		                            queryRst,
		                            e_PixMode[i]
		                            );
		    //crop size can't < outsize
		    //__CROP_SIZE(sensorSize) must > _size in rrzo
		    _cropsize = __CROP_SIZE(__RRZO,fmt,sensorSize[i],e_PixMode[i]);
			HwPortConfig_t resized = {
		        PORT_RRZO,
		        fmt,
		        _size,
		        MRect(__CROP_Start(sensorSize[i],_cropsize,fmt),_cropsize),
		        0, //if raw type != 1 -> pure-raw
		        { queryRst.stride_byte, 0, 0 }
		    };

            LOG_INF("senidx:0x%x(%dx%d),_cropsize(%dx%d),resized(%dx%d)\n", \
                i, sensorSize[i].w, sensorSize[i].h,\
                _cropsize.w, _cropsize.h, \
                resized.mSize.w, resized.mSize.h);
		    lHwPortCfg[i].push_back(resized);
	    }
	    LOG_INF("senidx:0x%x:hwport size:0x%x\n",i,lHwPortCfg[i].size());
    }
    ////////////////////////////////////////////////////////////////////////////
    //configpipe
    LOG_INF("start configpipe\n");
    vector<portInfo> vPortInfo[2];
    list<HwPortConfig_t>::const_iterator pPortCfg;
    for(int i=0;i<dualSensor;i++){
	    for( pPortCfg = lHwPortCfg[i].begin(); pPortCfg != lHwPortCfg[i].end(); pPortCfg++ )
	    {
	    	LOG_INF("senidx:0x%x id:0x%x, crop:%d,%d,%dx%d, size:%dx%d, fmt:0x%x, stride:%d, pureraw:%d\n",\
        	    	i,\
        	    	pPortCfg->mPortID.index,\
        	    	pPortCfg->mCrop.p.x,pPortCfg->mCrop.p.y,pPortCfg->mCrop.s.w,pPortCfg->mCrop.s.h,\
        	    	pPortCfg->mSize.w,pPortCfg->mSize.h,\
        	    	pPortCfg->mFmt,\
        	    	pPortCfg->mStrideInByte[0],\
        	    	pPortCfg->mPureRaw);
	        //
	        portInfo OutPort(
	                pPortCfg->mPortID,
	                pPortCfg->mFmt,
	                pPortCfg->mSize, //dst size
	                pPortCfg->mCrop, //crop
	                pPortCfg->mStrideInByte[0],
	                pPortCfg->mStrideInByte[1],
	                pPortCfg->mStrideInByte[2],
	                pPortCfg->mPureRaw, // pureraw
	                MTRUE               //packed
	                );
	        vPortInfo[i].push_back(OutPort);
	    }
	    //
	    QInitParam halCamIOinitParam(
	            0, // 2: sensor uses pattern
	            SEN_PIX_BITDEPTH,
	            vSensorCfg[i],
	            vPortInfo[i]);
	    //
	    //NG_TRACE_BEGIN("configP1");
	    if( !mpCamIO[i]->configPipe(halCamIOinitParam) ) {
	        LOG_INF("senidx:0x%x configPipe failed\n",i);
	    }
    }
	/////////////////////////////////////////////////////////////////////////
	//allocate buffer
	// 4 enque buf (3 push to drv before start, 1 for replace)
	// 3 dummy buf
	LOG_INF("start allocate buffer\n");

	IMEM_BUF_INFO p1Buf;
	IImageBuffer* pImgBuffer[_MAXTG_][__MAXDMAO][(Enque_buf+Dummy_buf+replace_buf)];

	for(int k=0;k<dualSensor;k++){
		for(int i=0;i<(Enque_buf+Dummy_buf+replace_buf);i++){
			if(enablePort & __RRZO_ENABLE){
				MSize _size((sensorSize[k].w/RRZ_SCALING_RATIO),(sensorSize[k].h/RRZ_SCALING_RATIO));
				if( !getOutputFmt_2(PORT_RRZO,SEN_PIX_BITDEPTH,mSensorInfo[k], &fmt))
					LOG_INF("get pix fmt error\n");
		    	NSImageio::NSIspio::ISP_QuerySize(
                            NSImageio::NSIspio::EPortIndex_RRZO,
                            NSImageio::NSIspio::ISP_QUERY_X_PIX|
                            NSImageio::NSIspio::ISP_QUERY_STRIDE_PIX|
                            NSImageio::NSIspio::ISP_QUERY_STRIDE_BYTE,
                            fmt,
                            _size.w,
                            queryRst,
                            e_PixMode[k]
                            );

				p1Buf.size = _size.h * queryRst.stride_byte;
		        LOG_INF("p1Buf.size/stride/h/fmt(%d/%d/%d/0x%x)\n", p1Buf.size, queryRst.stride_byte, _size.h, fmt);
				mpImemDrv->allocVirtBuf(&p1Buf);
				////imem buffer 2 image heap
				MUINT32 bufStridesInBytes[3] = {queryRst.stride_byte, 0, 0};
				MINT32 bufBoundaryInBytes[3] = {0, 0, 0};
				PortBufInfo_v1 portBufInfo = PortBufInfo_v1( p1Buf.memID,p1Buf.virtAddr,0,p1Buf.bufSecu, p1Buf.bufCohe);

				IImageBufferAllocator::ImgParam imgParam = IImageBufferAllocator::ImgParam((bitmap_FG(SEN_PIX_BITDEPTH)),
														_size, bufStridesInBytes, bufBoundaryInBytes, 1);
				sp<ImageBufferHeap> pHeap = ImageBufferHeap::create( LOG_TAG, imgParam,portBufInfo,MTRUE);
				pImgBuffer[k][__RRZO][i] = pHeap->createImageBuffer();
				pImgBuffer[k][__RRZO][i]->incStrong(pImgBuffer);
				pImgBuffer[k][__RRZO][i]->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
				LOG_INF("senidx:0x%x img buffer(%d): rrzo pa:0x%x\n",k,i,pImgBuffer[k][__RRZO][i]->getBufPA(0));
			}
			if(enablePort & __IMGO_ENABLE){
				if( !getOutputFmt_2(PORT_IMGO,SEN_PIX_BITDEPTH,mSensorInfo[k], &fmt))
					LOG_INF("get pix fmt error\n");
		    	NSImageio::NSIspio::ISP_QuerySize(
		                            NSImageio::NSIspio::EPortIndex_IMGO,
		                            NSImageio::NSIspio::ISP_QUERY_X_PIX|
		                            NSImageio::NSIspio::ISP_QUERY_STRIDE_PIX|
		                            NSImageio::NSIspio::ISP_QUERY_STRIDE_BYTE,
		                            fmt,
		                            sensorSize[k].w,
		                            queryRst,
		                            e_PixMode[k]
		                            );


				p1Buf.size = sensorSize[k].h* queryRst.stride_byte;
                LOG_INF("p1Buf.size/stride/h/fmt(%d/%d/0x%x)\n", p1Buf.size, queryRst.stride_byte, fmt);
				mpImemDrv->allocVirtBuf(&p1Buf);
				////imem buffer 2 image heap
				MUINT32 bufStridesInBytes[3] = {queryRst.stride_byte, 0, 0};
				MINT32 bufBoundaryInBytes[3] = {0, 0, 0};
				PortBufInfo_v1 portBufInfo = PortBufInfo_v1( p1Buf.memID,p1Buf.virtAddr,0,p1Buf.bufSecu, p1Buf.bufCohe);

				IImageBufferAllocator::ImgParam imgParam = IImageBufferAllocator::ImgParam((bitmap(SEN_PIX_BITDEPTH)),
														sensorSize[k], bufStridesInBytes, bufBoundaryInBytes, 1);
				sp<ImageBufferHeap> pHeap = ImageBufferHeap::create( LOG_TAG, imgParam,portBufInfo,MTRUE);
				pImgBuffer[k][__IMGO][i] = pHeap->createImageBuffer();
				pImgBuffer[k][__IMGO][i]->incStrong(pImgBuffer);
				pImgBuffer[k][__IMGO][i]->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
				LOG_INF("senidx:0x%x img buffer(%d): imgo pa:0x%x\n",k,i,pImgBuffer[k][__IMGO][i]->getBufPA(0));
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//push enque buf/dummy frame into drv before start. to set FBC number
	//enque buf
	//in this example, enque buf with mag : 1,2,3
	LOG_INF("start push enque buf/dummy frame into drv\n");
	QBufInfo buf;
	MSize _rrz ;
	for(int k=0;k<dualSensor;k++){
		for(int i=0;i<Enque_buf;i++){
			buf.mvOut.clear();
			if(enablePort & __RRZO_ENABLE){
				//note:crop size can't < outsize
				_rrz = __SCALE_SIZE(__RRZO,pImgBuffer[k][__RRZO][i]->getImgFormat(),sensorSize[k],RRZ_SCALING_RATIO,e_PixMode[k]);
				_cropsize = __CROP_SIZE(__RRZO,pImgBuffer[k][__RRZO][i]->getImgFormat(),sensorSize[k],e_PixMode[k]);
                printf("_rrz size : 0x%x 0x%x (0x%x 0x%x)(0x%x 0x%x) %d\n",_rrz.w,_rrz.h,sensorSize[k].w,sensorSize[k].h, _cropsize.w, _cropsize.h, pImgBuffer[k][__RRZO][i]->getImgFormat());
                NSCam::NSIoPipe::NSCamIOPipe::BufInfo _buf;
                if(bArbCrop && i==1){
                    _buf = NSCam::NSIoPipe::NSCamIOPipe::BufInfo(PORT_RRZO,pImgBuffer[k][__RRZO][i],_rrz,MRect(__CROP_Start_2(sensorSize[k],_cropsize,pImgBuffer[k][__RRZO][i]->getImgFormat(),1),_cropsize),i+1);
                }
                else{
    				_buf = NSCam::NSIoPipe::NSCamIOPipe::BufInfo(PORT_RRZO,pImgBuffer[k][__RRZO][i],_rrz,MRect(__CROP_Start(sensorSize[k],_cropsize,pImgBuffer[k][__RRZO][i]->getImgFormat()),_cropsize),i+1);
                }
				buf.mvOut.push_back(_buf);
			}
			if(enablePort & __IMGO_ENABLE){
                if(bEnZoom)
                {
                    if(i==1){
				        _cropsize = __CROP_SIZE(__IMGO,pImgBuffer[k][__IMGO][i]->getImgFormat(),sensorSize[k],e_PixMode[k]);
                    }
                    else{
                        _cropsize = __CROP_SIZE_2(__IMGO,pImgBuffer[k][__IMGO][i]->getImgFormat(),sensorSize[k],e_PixMode[k]);
                    }
                }
                else{
                    _cropsize = __CROP_SIZE(__IMGO,pImgBuffer[k][__IMGO][i]->getImgFormat(),sensorSize[k],e_PixMode[k]);
                }

                NSCam::NSIoPipe::NSCamIOPipe::BufInfo _buf;
                if(bArbCrop && i==1){
                    _buf = NSCam::NSIoPipe::NSCamIOPipe::BufInfo(PORT_IMGO,pImgBuffer[k][__IMGO][i],_cropsize,MRect(__CROP_Start_2(sensorSize[k],_cropsize,pImgBuffer[k][__IMGO][i]->getImgFormat(),1),_cropsize),i+1);
                }
                else{
				    _buf = NSCam::NSIoPipe::NSCamIOPipe::BufInfo(PORT_IMGO,pImgBuffer[k][__IMGO][i],_cropsize,MRect(__CROP_Start(sensorSize[k],_cropsize,pImgBuffer[k][__IMGO][i]->getImgFormat()),_cropsize),i+1);
                }
				buf.mvOut.push_back(_buf);
			}
			mpCamIO[k]->enque(buf);
		}
	}
	//in this example, dummy buf with mag : 4,5,6
	for(int k=0;k<dualSensor;k++){
		for(int i=Dummy_buf;i<(Enque_buf+Dummy_buf);i++){
			buf.mvOut.clear();
			if(enablePort & __RRZO_ENABLE){
				_rrz = __SCALE_SIZE(__RRZO,pImgBuffer[k][__RRZO][i]->getImgFormat(),sensorSize[k],RRZ_SCALING_RATIO,e_PixMode[k]);
				_cropsize = __CROP_SIZE(__RRZO,pImgBuffer[k][__RRZO][i]->getImgFormat(),sensorSize[k],e_PixMode[k]);
                NSCam::NSIoPipe::NSCamIOPipe::BufInfo _buf;
                if(bArbCrop && i%3==1){
    				_buf = NSCam::NSIoPipe::NSCamIOPipe::BufInfo(PORT_RRZO,pImgBuffer[k][__RRZO][i],_rrz,MRect(__CROP_Start_2(sensorSize[k],_cropsize,pImgBuffer[k][__RRZO][i]->getImgFormat(),1),_cropsize),i+1);
                }
                else{
                    _buf = NSCam::NSIoPipe::NSCamIOPipe::BufInfo(PORT_RRZO,pImgBuffer[k][__RRZO][i],_rrz,MRect(__CROP_Start(sensorSize[k],_cropsize,pImgBuffer[k][__RRZO][i]->getImgFormat()),_cropsize),i+1);
                }
				buf.mvOut.push_back(_buf);
			}
			if(enablePort & __IMGO_ENABLE){
                if(bEnZoom)
                {
                    if(i%3==1){
    				    _cropsize = __CROP_SIZE(__IMGO,pImgBuffer[k][__IMGO][i]->getImgFormat(),sensorSize[k],e_PixMode[k]);
                    }
                    else{
                        _cropsize = __CROP_SIZE_2(__IMGO,pImgBuffer[k][__IMGO][i]->getImgFormat(),sensorSize[k],e_PixMode[k]);
                    }
                }
                else{
                    _cropsize = __CROP_SIZE(__IMGO,pImgBuffer[k][__IMGO][i]->getImgFormat(),sensorSize[k],e_PixMode[k]);
                }
                NSCam::NSIoPipe::NSCamIOPipe::BufInfo _buf;
                if(bArbCrop && i%3==1){
				    _buf = NSCam::NSIoPipe::NSCamIOPipe::BufInfo(PORT_IMGO,pImgBuffer[k][__IMGO][i],_cropsize,MRect(__CROP_Start_2(sensorSize[k],_cropsize,pImgBuffer[k][__IMGO][i]->getImgFormat(),1),_cropsize),i+1);
                }
                else{
                    _buf = NSCam::NSIoPipe::NSCamIOPipe::BufInfo(PORT_IMGO,pImgBuffer[k][__IMGO][i],_cropsize,MRect(__CROP_Start(sensorSize[k],_cropsize,pImgBuffer[k][__IMGO][i]->getImgFormat()),_cropsize),i+1);
                }
				buf.mvOut.push_back(_buf);
			}
			mpCamIO[k]->DummyFrame(buf);
		}
	}

	/////////////////////////////////////////////////////////////
    //
    //pre-set tuning setting to verify p1 get tuning data
    //
    if(bEnTuning)
    {
        LOG_INF("settttttttttttttt p1 tuning \n");
        setP1Tuning(1); //magic number of pass1 ring buffer is 1,2,3
        setP1Tuning(2);
        setP1Tuning(3);
        setP1Tuning(7);//magic number of pass1 replace buffer is 7
    }
    /////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//start isp
	LOG_INF("!!!ISP START\n");
	for(int i=0;i<dualSensor;i++)
		mpCamIO[i]->start();

	/**
		note: need to start isp fist(can't use deque first,than use another to start isp, or deadlock will hanppen)
	*/
	//start deque/enque thread
	LOG_INF("start deque/denque thread\n");
	_test_camio_ * pCamio[_MAXTG_] = {NULL};
	for(int i=0;i<dualSensor;i++){
		pCamio[i] = _test_camio_::create();
		pCamio[i]->m_pNormalP_FrmB = mpCamIO[i];
		pCamio[i]->m_enablePort = enablePort;
		pCamio[i]->mPixMode = e_PixMode[i];
        pCamio[i]->m_Sensoridx = i;
		if(pCamio[i]->m_enablePort & __RRZO_ENABLE)
			pCamio[i]->m_prepalce_Buf[__RRZO] = pImgBuffer[i][__RRZO][(Enque_buf+Dummy_buf)];
		if(pCamio[i]->m_enablePort & __IMGO_ENABLE)
			pCamio[i]->m_prepalce_Buf[__IMGO] = pImgBuffer[i][__IMGO][(Enque_buf+Dummy_buf)];
		pCamio[i]->m_TgSize = sensorSize[i];
		pCamio[i]->startThread();
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//p1 stop
    for(int i=0;i<dualSensor;i++){
		pCamio[i]->stopThread();
		LOG_INF("!!!!!!!! [%d]thread stoped... !!!!!!!!\n", i);
		mpCamIO[i]->stop();
		LOG_INF("!!!!!!!! [%d]isp stoped... !!!!!!!!\n", i);


		mpCamIO[i]->uninit();
		mpCamIO[i]->destroyInstance(LOG_TAG);

		//poweroff sensor
		pSensorHalObj[i]->powerOff(LOG_TAG,1, &sensorArray[i]);
	}
    LOG_INF("!!!!!!!!done... !!!!!!!!\n");
    return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
int basic_flow_tuning(int num,int type)
{
    int ret=0;
    runningcase=num;
    bEnTuning=true;

    IHalSensorList* const pHalSensorList = IHalSensorList::get();
    pHalSensorList->searchSensors();
    int scenario=0;
    MUINT32 dualSensor=1; // 1 for single , 2 for dual sensor, if dualSensor == 1 , always use main sensor
    MUINT32 enablePort=0x0;
    IMemDrv* mpImemDrv=NULL;
    mpImemDrv=IMemDrv::createInstance();
    mpImemDrv->init();
    sem_init(&mbufferSem, 0, 0);
    sem_init(&mLeaveSem, 0, 0);
    bLeaveP2=false;
    switch(type){
    //       case 0: //prv_fps30_imgo
    //           scenario = 0;
    //           enablePort = 0x2;
    //           break;
       case 1: //prv_fps30_rrzo
           scenario = 0;
           enablePort = 0x1;
           break;
       case 2: //prv_fps30_imgo+rrzo
           scenario = 0;
           enablePort = 0x3;
           break;
    //      case 3: //prv_fps30_imgo_dualSensor
    //          scenario = 0;
    //          enablePort = 0x2;
    //          dualSensor = 2;
    //          break;
       case 4: //prv_fps30_rrzo_dualSensor
           scenario = 0;
           enablePort = 0x1;
           dualSensor = 2;
           break;
       case 5: //prv_fps30_rrzo+imgo_dualSensor
           scenario = 0;
           enablePort = 0x3;
           dualSensor = 2;
           break;
    //     case 6: //cap_fps30_imgo
    //         scenario = 1;
    //         enablePort = 0x2;
    //         break;
       case 7: //cap_fps30_rrzo
           scenario = 1;
           enablePort = 0x1;
           break;
       case 8: //cap_fps30_imgo+rrzo
           scenario = 1;
           enablePort = 0x3;
           break;
       default:
           LOG_ERR("unsupported testcaseNum0x%x\n",type);
           return 0;
           break;
    }
    MUINT32 sensorIdx = 0;    //0 for main.
    MUINT32 sensorFps = 30;    //30 for 30fps
    LOG_INF("sensorIdx:0x%x,senario:0x%x,sensorFps:0x%x,enablePort = 0x%x\n",sensorIdx,scenario,sensorFps,enablePort);

    /////////////////////////////////////////
    //sensor operation
    MUINT32    sensorArray[2];
    SensorStaticInfo mSensorInfo[2];
    if(dualSensor > 2)
       dualSensor = 2;

    //powerOn sensor
    IHalSensor* pSensorHalObj[_MAXTG_] = {NULL};
    for(int i=0;i<dualSensor;i++){
       pSensorHalObj[i] = pHalSensorList->createSensor(LOG_TAG, i);
       if(pSensorHalObj == NULL)
       {
          LOG_ERR("mpSensorHalObj is NULL");
       }
       //
       sensorArray[i] = i;
       pSensorHalObj[i]->powerOn(LOG_TAG, 1, &sensorArray[i]);
    }
    //

    //query sensor
    for(int i=0;i<dualSensor;i++){
       IHalSensorList* const pHalSensorList = IHalSensorList::get();
       pHalSensorList->querySensorStaticInfo(
               pHalSensorList->querySensorDevIdx(i),
               &mSensorInfo[i]);
    }
    //////////////////////////////////////////////////////////////////////
    //pass1 operation
    //create/init normalpipe_frmb
    INormalPipe_FrmB* mpCamIO[2] = {NULL,NULL};
    for(int i=0;i<dualSensor;i++){
       mpCamIO[i] = INormalPipe_FrmB::createInstance((MINT32)i, LOG_TAG,1);
       mpCamIO[i]->init();
    }

    //prepare sensor cfg
    MSize sensorSize[2];
    vector<IHalSensor::ConfigParam> vSensorCfg[2];
    for(int i=0;i<dualSensor;i++){
       getSensorSize_2(&sensorSize[i],mSensorInfo[i],scenario);
       IHalSensor::ConfigParam sensorCfg =
       {
           (MUINT)i,                       /* index            */
           sensorSize[i],                  /* crop             */
           scenario,                       /* scenarioId       */
           0,                              /* isBypassScenario */
           1,                              /* isContinuous     */
           MFALSE,                         /* iHDROn           */
           sensorFps,                      /* framerate        */
           0,                              /* two pixel on     */
           0,                              /* debugmode        */
       };
       vSensorCfg[i].push_back(sensorCfg);
       LOG_INF("senidx:0x%x, sensor %dx%d, sce %d, bypass %d, con %d, hdr %d, fps %d, twopxl %d\n",
                   i,
                   sensorCfg.crop.w,
                   sensorCfg.crop.h,
                   sensorCfg.scenarioId,
                   sensorCfg.isBypassScenario,
                   sensorCfg.isContinuous,
                   sensorCfg.HDRMode,
                   sensorCfg.framerate,
                   sensorCfg.twopixelOn);
    }
    ////////////////////////////////////////////////////////////////////////
    //isp dmao cfg
    LOG_INF("start isp dmao cfg\n");
    EImageFormat fmt;
    NSImageio::NSIspio::ISP_QUERY_RST queryRst;
    MUINT32 pixelMode[2] = {0};
    MSize _cropsize;
    NSImageio::NSIspio::E_ISP_PIXMODE e_PixMode[2];
    list<HwPortConfig_t> lHwPortCfg[2];
    for(int i=0;i<dualSensor;i++){
       getSensorPixelMode_2(&pixelMode[i],i,scenario,sensorFps);
       e_PixMode[i] =  ((pixelMode[i] == 0) ?  (NSImageio::NSIspio::ISP_QUERY_1_PIX_MODE) : (NSImageio::NSIspio::ISP_QUERY_2_PIX_MODE));
       if(enablePort & __IMGO_ENABLE){
           LOG_INF("senidx:0x%x:port IMGO\n",i);
           if( !getOutputFmt_2(PORT_IMGO,SEN_PIX_BITDEPTH,mSensorInfo[i], &fmt))
               LOG_INF("get pix fmt error\n");
           NSImageio::NSIspio::ISP_QuerySize(
                                   NSImageio::NSIspio::EPortIndex_IMGO,
                                   NSImageio::NSIspio::ISP_QUERY_X_PIX|
                                   NSImageio::NSIspio::ISP_QUERY_STRIDE_PIX|
                                   NSImageio::NSIspio::ISP_QUERY_STRIDE_BYTE,
                                   fmt,
                                   sensorSize[i].w,
                                   queryRst,
                                   e_PixMode[i]
                                   );

           _cropsize = __CROP_SIZE(__IMGO,fmt,sensorSize[i],e_PixMode[i]);

           HwPortConfig_t full = {
               PORT_IMGO,
               fmt,
               _cropsize,
               MRect(__CROP_Start(sensorSize[i],_cropsize,fmt),_cropsize),
               0, //if raw type != 1 -> pure-raw
               { queryRst.stride_byte, 0, 0 }
           };
           lHwPortCfg[i].push_back(full);
       }
       if(enablePort & __RRZO_ENABLE){
           LOG_INF("senidx:0x%x:port RRZO\n",i);
           MSize _size;
           if( !getOutputFmt_2(PORT_RRZO,SEN_PIX_BITDEPTH,mSensorInfo[i], &fmt))
               LOG_INF("get pix fmt error\n");

           _size = __SCALE_SIZE(__RRZO,fmt,sensorSize[i],1/*RRZ_SCALING_RATIO*/,e_PixMode[i]);
           LOG_INF("SCALE_SIZE: _size(%dx%d)\n", _size.w, _size.h);
           NSImageio::NSIspio::ISP_QuerySize(
                                   NSImageio::NSIspio::EPortIndex_RRZO,
                                   NSImageio::NSIspio::ISP_QUERY_X_PIX|
                                   NSImageio::NSIspio::ISP_QUERY_STRIDE_PIX|
                                   NSImageio::NSIspio::ISP_QUERY_STRIDE_BYTE,
                                   fmt,
                                   _size.w,
                                   queryRst,
                                   e_PixMode[i]
                                   );
           //crop size can't < outsize
           //__CROP_SIZE(sensorSize) must > _size in rrzo
           _cropsize = __CROP_SIZE_2(__RRZO,fmt,sensorSize[i],e_PixMode[i]);
           HwPortConfig_t resized = {
               PORT_RRZO,
               fmt,
               _size,
               MRect(__CROP_Start(sensorSize[i],_cropsize,fmt),_cropsize),
               0, //if raw type != 1 -> pure-raw
               { queryRst.stride_byte, 0, 0 }
           };

           LOG_INF("senidx:0x%x(%dx%d),_cropsize(%dx%d),resized(%dx%d)\n", \
               i, sensorSize[i].w, sensorSize[i].h,\
               _cropsize.w, _cropsize.h, \
               resized.mSize.w, resized.mSize.h);
           lHwPortCfg[i].push_back(resized);
       }
       LOG_INF("senidx:0x%x:hwport size:0x%x\n",i,lHwPortCfg[i].size());
    }
    ////////////////////////////////////////////////////////////////////////////
    //configpipe
    LOG_INF("start configpipe\n");
    vector<portInfo> vPortInfo[2];
    list<HwPortConfig_t>::const_iterator pPortCfg;
    for(int i=0;i<dualSensor;i++){
       for( pPortCfg = lHwPortCfg[i].begin(); pPortCfg != lHwPortCfg[i].end(); pPortCfg++ )
       {
           LOG_INF("senidx:0x%x id:0x%x, crop:%d,%d,%dx%d, size:%dx%d, fmt:0x%x, stride:%d, pureraw:%d\n",\
                   i,\
                   pPortCfg->mPortID.index,\
                   pPortCfg->mCrop.p.x,pPortCfg->mCrop.p.y,pPortCfg->mCrop.s.w,pPortCfg->mCrop.s.h,\
                   pPortCfg->mSize.w,pPortCfg->mSize.h,\
                   pPortCfg->mFmt,\
                   pPortCfg->mStrideInByte[0],\
                   pPortCfg->mPureRaw);
           //
           portInfo OutPort(
                   pPortCfg->mPortID,
                   pPortCfg->mFmt,
                   pPortCfg->mSize, //dst size
                   pPortCfg->mCrop, //crop
                   pPortCfg->mStrideInByte[0],
                   pPortCfg->mStrideInByte[1],
                   pPortCfg->mStrideInByte[2],
                   pPortCfg->mPureRaw, // pureraw
                   MTRUE               //packed
                   );
           vPortInfo[i].push_back(OutPort);
       }
       //
       QInitParam halCamIOinitParam(
               0, // 2: sensor uses pattern
               SEN_PIX_BITDEPTH,
               vSensorCfg[i],
               vPortInfo[i]);
       //
       //NG_TRACE_BEGIN("configP1");
       if( !mpCamIO[i]->configPipe(halCamIOinitParam) ) {
           LOG_INF("senidx:0x%x configPipe failed\n",i);
       }
    }

    /////////////////////////////////////////////////////////////
    //
    //pre-set tuning setting to verify p1 get tuning data
    LOG_INF("settttttttttttttt p1 tuning \n");
    setP1Tuning(1); //magic number of pass1 ring buffer is 1,2,3
    setP1Tuning(2);
    setP1Tuning(3);
    setP1Tuning(7);//magic number of pass1 replace buffer is 7
    /////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////
    //allocate buffer
    // 4 enque buf (3 push to drv before start, 1 for replace)
    // 3 dummy buf
    LOG_INF("start allocate buffer\n");

    IMEM_BUF_INFO p1Buf;
    IImageBuffer* pImgBuffer[_MAXTG_][__MAXDMAO][(Enque_buf+Dummy_buf+replace_buf)];

    for(int k=0;k<dualSensor;k++){
       for(int i=0;i<(Enque_buf+Dummy_buf+replace_buf);i++){
           if(enablePort & __RRZO_ENABLE){
               MSize _size((sensorSize[k].w/*/RRZ_SCALING_RATIO*/),(sensorSize[k].h/*/RRZ_SCALING_RATIO*/));
               if( !getOutputFmt_2(PORT_RRZO,SEN_PIX_BITDEPTH,mSensorInfo[k], &fmt))
                   LOG_INF("get pix fmt error\n");
               NSImageio::NSIspio::ISP_QuerySize(
                           NSImageio::NSIspio::EPortIndex_RRZO,
                           NSImageio::NSIspio::ISP_QUERY_X_PIX|
                           NSImageio::NSIspio::ISP_QUERY_STRIDE_PIX|
                           NSImageio::NSIspio::ISP_QUERY_STRIDE_BYTE,
                           fmt,
                           _size.w,
                           queryRst,
                           e_PixMode[k]
                           );

               p1Buf.size = _size.h * queryRst.stride_byte;
               LOG_INF("p1Buf.size/stride/h(%d/%d/%d)\n", p1Buf.size, queryRst.stride_byte, _size.h);
               mpImemDrv->allocVirtBuf(&p1Buf);
               ////imem buffer 2 image heap
               MUINT32 bufStridesInBytes[3] = {queryRst.stride_byte, 0, 0};
               MINT32 bufBoundaryInBytes[3] = {0, 0, 0};
               PortBufInfo_v1 portBufInfo = PortBufInfo_v1( p1Buf.memID,p1Buf.virtAddr,0,p1Buf.bufSecu, p1Buf.bufCohe);

               IImageBufferAllocator::ImgParam imgParam = IImageBufferAllocator::ImgParam((bitmap_FG(SEN_PIX_BITDEPTH)),
                                                       _size, bufStridesInBytes, bufBoundaryInBytes, 1);
               sp<ImageBufferHeap> pHeap = ImageBufferHeap::create( LOG_TAG, imgParam,portBufInfo,MTRUE);
               pImgBuffer[k][__RRZO][i] = pHeap->createImageBuffer();
               pImgBuffer[k][__RRZO][i]->incStrong(pImgBuffer);
               pImgBuffer[k][__RRZO][i]->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
               LOG_INF("senidx:0x%x img buffer(%d): rrzo pa:0x%x\n",k,i,pImgBuffer[k][__RRZO][i]->getBufPA(0));
           }
           if(enablePort & __IMGO_ENABLE){
               if( !getOutputFmt_2(PORT_IMGO,SEN_PIX_BITDEPTH,mSensorInfo[k], &fmt))
                   LOG_INF("get pix fmt error\n");
               NSImageio::NSIspio::ISP_QuerySize(
                                   NSImageio::NSIspio::EPortIndex_IMGO,
                                   NSImageio::NSIspio::ISP_QUERY_X_PIX|
                                   NSImageio::NSIspio::ISP_QUERY_STRIDE_PIX|
                                   NSImageio::NSIspio::ISP_QUERY_STRIDE_BYTE,
                                   fmt,
                                   sensorSize[k].w,
                                   queryRst,
                                   e_PixMode[k]
                                   );


               p1Buf.size = sensorSize[k].h* queryRst.stride_byte;
               mpImemDrv->allocVirtBuf(&p1Buf);
               ////imem buffer 2 image heap
               MUINT32 bufStridesInBytes[3] = {queryRst.stride_byte, 0, 0};
               MINT32 bufBoundaryInBytes[3] = {0, 0, 0};
               PortBufInfo_v1 portBufInfo = PortBufInfo_v1( p1Buf.memID,p1Buf.virtAddr,0,p1Buf.bufSecu, p1Buf.bufCohe);

               IImageBufferAllocator::ImgParam imgParam = IImageBufferAllocator::ImgParam((bitmap(SEN_PIX_BITDEPTH)),
                                                       sensorSize[k], bufStridesInBytes, bufBoundaryInBytes, 1);
               sp<ImageBufferHeap> pHeap = ImageBufferHeap::create( LOG_TAG, imgParam,portBufInfo,MTRUE);
               pImgBuffer[k][__IMGO][i] = pHeap->createImageBuffer();
               pImgBuffer[k][__IMGO][i]->incStrong(pImgBuffer);
               pImgBuffer[k][__IMGO][i]->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
               LOG_INF("senidx:0x%x img buffer(%d): imgo pa:0x%x\n",k,i,pImgBuffer[k][__IMGO][i]->getBufPA(0));
           }
       }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    //push enque buf/dummy frame into drv before start. to set FBC number
    //enque buf
    //in this example, enque buf with mag : 1,2,3
    LOG_INF("start push enque buf/dummy frame into drv\n");
    QBufInfo buf;
    MSize _rrz ;
    for(int k=0;k<dualSensor;k++){
       for(int i=0;i<Enque_buf;i++){
           buf.mvOut.clear();
           if(enablePort & __RRZO_ENABLE){
               //note:crop size can't < outsize
               //_rrz = __SCALE_SIZE(__RRZO,pImgBuffer[k][__RRZO][i]->getImgFormat(),sensorSize[k], ((i==1)?RRZ_SCALING_RATIO:1),e_PixMode[k]);
               //_cropsize = __CROP_SIZE(__RRZO,pImgBuffer[k][__RRZO][i]->getImgFormat(),sensorSize[k],e_PixMode[k]);
               if(i==1)//scale
               {
                    _rrz = __SCALE_SIZE(__RRZO,pImgBuffer[k][__RRZO][i]->getImgFormat(),sensorSize[k],RRZ_SCALING_RATIO,e_PixMode[k]);
                    _cropsize = __CROP_SIZE(__RRZO,pImgBuffer[k][__RRZO][i]->getImgFormat(),sensorSize[k],e_PixMode[k]);
               }
               else
               {
                    _rrz = __SCALE_SIZE(__RRZO,pImgBuffer[k][__RRZO][i]->getImgFormat(),sensorSize[k],1,e_PixMode[k]);
                    _cropsize = __CROP_SIZE_2(__RRZO,pImgBuffer[k][__RRZO][i]->getImgFormat(),sensorSize[k],e_PixMode[k]);
               }
               printf("_rrz size : 0x%x 0x%x (0x%x 0x%x)(0x%x 0x%x) %d\n",_rrz.w,_rrz.h,sensorSize[k].w,sensorSize[k].h, _cropsize.w, _cropsize.h, pImgBuffer[k][__RRZO][i]->getImgFormat());
               NSCam::NSIoPipe::NSCamIOPipe::BufInfo _buf = NSCam::NSIoPipe::NSCamIOPipe::BufInfo(PORT_RRZO,pImgBuffer[k][__RRZO][i],_rrz,MRect(__CROP_Start(sensorSize[k],_cropsize,pImgBuffer[k][__RRZO][i]->getImgFormat()),_cropsize),i+1);
               buf.mvOut.push_back(_buf);
           }
           if(enablePort & __IMGO_ENABLE){
               _cropsize = __CROP_SIZE(__IMGO,pImgBuffer[k][__IMGO][i]->getImgFormat(),sensorSize[k],e_PixMode[k]);
               NSCam::NSIoPipe::NSCamIOPipe::BufInfo _buf = NSCam::NSIoPipe::NSCamIOPipe::BufInfo(PORT_IMGO,pImgBuffer[k][__IMGO][i],_cropsize,MRect(__CROP_Start(sensorSize[k],_cropsize,pImgBuffer[k][__IMGO][i]->getImgFormat()),_cropsize),i+1);
               buf.mvOut.push_back(_buf);
           }
           mpCamIO[k]->enque(buf);
       }
    }
    //in this example, dummy buf with mag : 4,5,6
    for(int k=0;k<dualSensor;k++){
       for(int i=Dummy_buf;i<(Enque_buf+Dummy_buf);i++){
           buf.mvOut.clear();
           if(enablePort & __RRZO_ENABLE){
               _rrz = __SCALE_SIZE(__RRZO,pImgBuffer[k][__RRZO][i]->getImgFormat(),sensorSize[k],RRZ_SCALING_RATIO,e_PixMode[k]);
               _cropsize = __CROP_SIZE(__RRZO,pImgBuffer[k][__RRZO][i]->getImgFormat(),sensorSize[k],e_PixMode[k]);
               NSCam::NSIoPipe::NSCamIOPipe::BufInfo _buf = NSCam::NSIoPipe::NSCamIOPipe::BufInfo(PORT_RRZO,pImgBuffer[k][__RRZO][i],_rrz,MRect(__CROP_Start(sensorSize[k],_cropsize,pImgBuffer[k][__RRZO][i]->getImgFormat()),_cropsize),i+1);
               buf.mvOut.push_back(_buf);
           }
           if(enablePort & __IMGO_ENABLE){
               _cropsize = __CROP_SIZE(__IMGO,pImgBuffer[k][__IMGO][i]->getImgFormat(),sensorSize[k],e_PixMode[k]);
               NSCam::NSIoPipe::NSCamIOPipe::BufInfo _buf = NSCam::NSIoPipe::NSCamIOPipe::BufInfo(PORT_IMGO,pImgBuffer[k][__IMGO][i],_cropsize,MRect(__CROP_Start(sensorSize[k],_cropsize,pImgBuffer[k][__IMGO][i]->getImgFormat()),_cropsize),i+1);
               buf.mvOut.push_back(_buf);
           }
           mpCamIO[k]->DummyFrame(buf);
       }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //start isp
    LOG_INF("!!!ISP START\n");
    for(int i=0;i<dualSensor;i++)
       mpCamIO[i]->start();

    /**
       note: need to start isp fist(can't use deque first,than use another to start isp, or deadlock will hanppen)
    */
    //start deque/enque thread
    LOG_INF("start deque/denque thread\n");
    _test_camio_ * pCamio[_MAXTG_] = {NULL};
    for(int i=0;i<dualSensor;i++){
       pCamio[i] = _test_camio_::create();
       pCamio[i]->m_pNormalP_FrmB = mpCamIO[i];
       pCamio[i]->m_enablePort = enablePort;
       pCamio[i]->mPixMode = e_PixMode[i];
       pCamio[i]->m_Sensoridx = i;
       if(pCamio[i]->m_enablePort & __RRZO_ENABLE)
           pCamio[i]->m_prepalce_Buf[__RRZO] = pImgBuffer[i][__RRZO][(Enque_buf+Dummy_buf)];
       if(pCamio[i]->m_enablePort & __IMGO_ENABLE)
           pCamio[i]->m_prepalce_Buf[__IMGO] = pImgBuffer[i][__IMGO][(Enque_buf+Dummy_buf)];
       pCamio[i]->m_TgSize = sensorSize[i];
       pCamio[i]->startThread();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //p1 stop
    for(int i=0;i<dualSensor;i++){
       pCamio[i]->stopThread();
       LOG_INF("!!!!!!!! thread stoped... !!!!!!!!\n");
       mpCamIO[i]->stop();
       LOG_INF("!!!!!!!! isp stoped... !!!!!!!!\n");


       mpCamIO[i]->uninit();
       mpCamIO[i]->destroyInstance(LOG_TAG);

       //poweroff sensor
       pSensorHalObj[i]->powerOff(LOG_TAG,1, &sensorArray[i]);
    }
    LOG_INF("!!!!!!!!done... !!!!!!!!\n");
    return ret;

}

////////////////////////////////////////////////////////////////////////////////////////
int basic_flow_crop_resize(int num,int type)
{
   int ret=0;
   runningcase=num;
   bEnTuning=false;
   bEnZoom=true;

   IHalSensorList* const pHalSensorList = IHalSensorList::get();
   pHalSensorList->searchSensors();
   int scenario=0;
   MUINT32 dualSensor=1; // 1 for single , 2 for dual sensor, if dualSensor == 1 , always use main sensor
   MUINT32 enablePort=0x0;
   IMemDrv* mpImemDrv=NULL;
   mpImemDrv=IMemDrv::createInstance();
   mpImemDrv->init();
   sem_init(&mbufferSem, 0, 0);
   sem_init(&mLeaveSem, 0, 0);
   bLeaveP2=false;
   switch(type){
//       case 0: //prv_fps30_imgo
//           scenario = 0;
//           enablePort = 0x2;
//           break;
       case 1: //prv_fps30_rrzo
           scenario = 0;
           enablePort = 0x1;
           break;
       case 2: //prv_fps30_imgo+rrzo
           scenario = 0;
           enablePort = 0x3;
           break;
 //      case 3: //prv_fps30_imgo_dualSensor
 //          scenario = 0;
 //          enablePort = 0x2;
 //          dualSensor = 2;
 //          break;
       case 4: //prv_fps30_rrzo_dualSensor
           scenario = 0;
           enablePort = 0x1;
           dualSensor = 2;
           break;
       case 5: //prv_fps30_rrzo+imgo_dualSensor
           scenario = 0;
           enablePort = 0x3;
           dualSensor = 2;
           break;
  //     case 6: //cap_fps30_imgo
  //         scenario = 1;
  //         enablePort = 0x2;
  //         break;
       case 7: //cap_fps30_rrzo
           scenario = 1;
           enablePort = 0x1;
           break;
       case 8: //cap_fps30_imgo+rrzo
           scenario = 1;
           enablePort = 0x3;
           break;
       default:
           LOG_ERR("unsupported testcaseNum0x%x\n",type);
           return 0;
           break;
   }
   MUINT32 sensorIdx = 0;    //0 for main.
   MUINT32 sensorFps = 30;    //30 for 30fps
   LOG_INF("sensorIdx:0x%x,senario:0x%x,sensorFps:0x%x,enablePort = 0x%x\n",sensorIdx,scenario,sensorFps,enablePort);

   /////////////////////////////////////////
   //sensor operation
   MUINT32    sensorArray[2];
   SensorStaticInfo mSensorInfo[2];
   if(dualSensor > 2)
       dualSensor = 2;

   //powerOn sensor
   IHalSensor* pSensorHalObj[_MAXTG_] = {NULL};
   for(int i=0;i<dualSensor;i++){
       pSensorHalObj[i] = pHalSensorList->createSensor(LOG_TAG, i);
       if(pSensorHalObj == NULL)
       {
          LOG_ERR("mpSensorHalObj is NULL");
       }
       //
       sensorArray[i] = i;
       pSensorHalObj[i]->powerOn(LOG_TAG, 1, &sensorArray[i]);
   }
   //

   //query sensor
   for(int i=0;i<dualSensor;i++){
       IHalSensorList* const pHalSensorList = IHalSensorList::get();
       pHalSensorList->querySensorStaticInfo(
               pHalSensorList->querySensorDevIdx(i),
               &mSensorInfo[i]);
   }
   //////////////////////////////////////////////////////////////////////
   //pass1 operation
   //create/init normalpipe_frmb
   INormalPipe_FrmB* mpCamIO[2] = {NULL,NULL};
   for(int i=0;i<dualSensor;i++){
       mpCamIO[i] = INormalPipe_FrmB::createInstance((MINT32)i, LOG_TAG,1);
       mpCamIO[i]->init();
   }

   //prepare sensor cfg
   MSize sensorSize[2];
   vector<IHalSensor::ConfigParam> vSensorCfg[2];
   for(int i=0;i<dualSensor;i++){
       getSensorSize_2(&sensorSize[i],mSensorInfo[i],scenario);
       IHalSensor::ConfigParam sensorCfg =
       {
           (MUINT)i,                       /* index            */
           sensorSize[i],                  /* crop             */
           scenario,                       /* scenarioId       */
           0,                              /* isBypassScenario */
           1,                              /* isContinuous     */
           MFALSE,                         /* iHDROn           */
           sensorFps,                      /* framerate        */
           0,                              /* two pixel on     */
           0,                              /* debugmode        */
       };
       vSensorCfg[i].push_back(sensorCfg);
       LOG_INF("senidx:0x%x, sensor %dx%d, sce %d, bypass %d, con %d, hdr %d, fps %d, twopxl %d\n",
                   i,
                   sensorCfg.crop.w,
                   sensorCfg.crop.h,
                   sensorCfg.scenarioId,
                   sensorCfg.isBypassScenario,
                   sensorCfg.isContinuous,
                   sensorCfg.HDRMode,
                   sensorCfg.framerate,
                   sensorCfg.twopixelOn);
   }
   ////////////////////////////////////////////////////////////////////////
   //isp dmao cfg
   LOG_INF("start isp dmao cfg\n");
   EImageFormat fmt;
   NSImageio::NSIspio::ISP_QUERY_RST queryRst;
   MUINT32 pixelMode[2] = {0};
   MSize _cropsize;
   NSImageio::NSIspio::E_ISP_PIXMODE e_PixMode[2];
   list<HwPortConfig_t> lHwPortCfg[2];
   for(int i=0;i<dualSensor;i++){
       getSensorPixelMode_2(&pixelMode[i],i,scenario,sensorFps);
       e_PixMode[i] =  ((pixelMode[i] == 0) ?  (NSImageio::NSIspio::ISP_QUERY_1_PIX_MODE) : (NSImageio::NSIspio::ISP_QUERY_2_PIX_MODE));
       if(enablePort & __IMGO_ENABLE){
           LOG_INF("senidx:0x%x:port IMGO\n",i);
           if( !getOutputFmt_2(PORT_IMGO,SEN_PIX_BITDEPTH,mSensorInfo[i], &fmt))
               LOG_INF("get pix fmt error\n");
           NSImageio::NSIspio::ISP_QuerySize(
                                   NSImageio::NSIspio::EPortIndex_IMGO,
                                   NSImageio::NSIspio::ISP_QUERY_X_PIX|
                                   NSImageio::NSIspio::ISP_QUERY_STRIDE_PIX|
                                   NSImageio::NSIspio::ISP_QUERY_STRIDE_BYTE,
                                   fmt,
                                   sensorSize[i].w,
                                   queryRst,
                                   e_PixMode[i]
                                   );

           _cropsize = __CROP_SIZE_2(__IMGO,fmt,sensorSize[i],e_PixMode[i]);

           HwPortConfig_t full = {
               PORT_IMGO,
               fmt,
               _cropsize,
               MRect(__CROP_Start(sensorSize[i],_cropsize,fmt),_cropsize),
               0, //if raw type != 1 -> pure-raw
               { queryRst.stride_byte, 0, 0 }
           };
           lHwPortCfg[i].push_back(full);
       }
       if(enablePort & __RRZO_ENABLE){
           LOG_INF("senidx:0x%x:port RRZO\n",i);
           MSize _size;
           if( !getOutputFmt_2(PORT_RRZO,SEN_PIX_BITDEPTH,mSensorInfo[i], &fmt))
               LOG_INF("get pix fmt error\n");

           _size = __SCALE_SIZE(__RRZO,fmt,sensorSize[i],1/*RRZ_SCALING_RATIO*/,e_PixMode[i]);
           LOG_INF("SCALE_SIZE: _size(%dx%d)\n", _size.w, _size.h);
           NSImageio::NSIspio::ISP_QuerySize(
                                   NSImageio::NSIspio::EPortIndex_RRZO,
                                   NSImageio::NSIspio::ISP_QUERY_X_PIX|
                                   NSImageio::NSIspio::ISP_QUERY_STRIDE_PIX|
                                   NSImageio::NSIspio::ISP_QUERY_STRIDE_BYTE,
                                   fmt,
                                   _size.w,
                                   queryRst,
                                   e_PixMode[i]
                                   );
           //crop size can't < outsize
           //__CROP_SIZE(sensorSize) must > _size in rrzo
           _cropsize = __CROP_SIZE_2(__RRZO,fmt,sensorSize[i],e_PixMode[i]);
           HwPortConfig_t resized = {
               PORT_RRZO,
               fmt,
               _size,
               MRect(__CROP_Start(sensorSize[i],_cropsize,fmt),_cropsize),
               0, //if raw type != 1 -> pure-raw
               { queryRst.stride_byte, 0, 0 }
           };

           LOG_INF("senidx:0x%x(%dx%d),_cropsize(%dx%d),resized(%dx%d)\n", \
               i, sensorSize[i].w, sensorSize[i].h,\
               _cropsize.w, _cropsize.h, \
               resized.mSize.w, resized.mSize.h);
           lHwPortCfg[i].push_back(resized);
       }
       LOG_INF("senidx:0x%x:hwport size:0x%x\n",i,lHwPortCfg[i].size());
   }
   ////////////////////////////////////////////////////////////////////////////
   //configpipe
   LOG_INF("start configpipe\n");
   vector<portInfo> vPortInfo[2];
   list<HwPortConfig_t>::const_iterator pPortCfg;
   for(int i=0;i<dualSensor;i++){
       for( pPortCfg = lHwPortCfg[i].begin(); pPortCfg != lHwPortCfg[i].end(); pPortCfg++ )
       {
           LOG_INF("senidx:0x%x id:0x%x, crop:%d,%d,%dx%d, size:%dx%d, fmt:0x%x, stride:%d, pureraw:%d\n",\
                   i,\
                   pPortCfg->mPortID.index,\
                   pPortCfg->mCrop.p.x,pPortCfg->mCrop.p.y,pPortCfg->mCrop.s.w,pPortCfg->mCrop.s.h,\
                   pPortCfg->mSize.w,pPortCfg->mSize.h,\
                   pPortCfg->mFmt,\
                   pPortCfg->mStrideInByte[0],\
                   pPortCfg->mPureRaw);
           //
           portInfo OutPort(
                   pPortCfg->mPortID,
                   pPortCfg->mFmt,
                   pPortCfg->mSize, //dst size
                   pPortCfg->mCrop, //crop
                   pPortCfg->mStrideInByte[0],
                   pPortCfg->mStrideInByte[1],
                   pPortCfg->mStrideInByte[2],
                   pPortCfg->mPureRaw, // pureraw
                   MTRUE               //packed
                   );
           vPortInfo[i].push_back(OutPort);
       }
       //
       QInitParam halCamIOinitParam(
               0, // 2: sensor uses pattern
               SEN_PIX_BITDEPTH,
               vSensorCfg[i],
               vPortInfo[i]);
       //
       //NG_TRACE_BEGIN("configP1");
       if( !mpCamIO[i]->configPipe(halCamIOinitParam) ) {
           LOG_INF("senidx:0x%x configPipe failed\n",i);
       }
   }
   /////////////////////////////////////////////////////////////////////////
   //allocate buffer
   // 4 enque buf (3 push to drv before start, 1 for replace)
   // 3 dummy buf
   LOG_INF("start allocate buffer\n");

   IMEM_BUF_INFO p1Buf;
   IImageBuffer* pImgBuffer[_MAXTG_][__MAXDMAO][(Enque_buf+Dummy_buf+replace_buf)];

   for(int k=0;k<dualSensor;k++){
       for(int i=0;i<(Enque_buf+Dummy_buf+replace_buf);i++){
           if(enablePort & __RRZO_ENABLE){
               MSize _size((sensorSize[k].w/*/RRZ_SCALING_RATIO*/),(sensorSize[k].h/*/RRZ_SCALING_RATIO*/));
               if( !getOutputFmt_2(PORT_RRZO,SEN_PIX_BITDEPTH,mSensorInfo[k], &fmt))
                   LOG_INF("get pix fmt error\n");
               NSImageio::NSIspio::ISP_QuerySize(
                           NSImageio::NSIspio::EPortIndex_RRZO,
                           NSImageio::NSIspio::ISP_QUERY_X_PIX|
                           NSImageio::NSIspio::ISP_QUERY_STRIDE_PIX|
                           NSImageio::NSIspio::ISP_QUERY_STRIDE_BYTE,
                           fmt,
                           _size.w,
                           queryRst,
                           e_PixMode[k]
                           );

               p1Buf.size = _size.h * queryRst.stride_byte;
               LOG_INF("p1Buf.size/stride/h(%d/%d/%d)\n", p1Buf.size, queryRst.stride_byte, _size.h);
               mpImemDrv->allocVirtBuf(&p1Buf);
               ////imem buffer 2 image heap
               MUINT32 bufStridesInBytes[3] = {queryRst.stride_byte, 0, 0};
               MINT32 bufBoundaryInBytes[3] = {0, 0, 0};
               PortBufInfo_v1 portBufInfo = PortBufInfo_v1( p1Buf.memID,p1Buf.virtAddr,0,p1Buf.bufSecu, p1Buf.bufCohe);

               IImageBufferAllocator::ImgParam imgParam = IImageBufferAllocator::ImgParam((bitmap_FG(SEN_PIX_BITDEPTH)),
                                                       _size, bufStridesInBytes, bufBoundaryInBytes, 1);
               sp<ImageBufferHeap> pHeap = ImageBufferHeap::create( LOG_TAG, imgParam,portBufInfo,MTRUE);
               pImgBuffer[k][__RRZO][i] = pHeap->createImageBuffer();
               pImgBuffer[k][__RRZO][i]->incStrong(pImgBuffer);
               pImgBuffer[k][__RRZO][i]->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
               LOG_INF("senidx:0x%x img buffer(%d): rrzo pa:0x%x\n",k,i,pImgBuffer[k][__RRZO][i]->getBufPA(0));
           }
           if(enablePort & __IMGO_ENABLE){
               if( !getOutputFmt_2(PORT_IMGO,SEN_PIX_BITDEPTH,mSensorInfo[k], &fmt))
                   LOG_INF("get pix fmt error\n");
               NSImageio::NSIspio::ISP_QuerySize(
                                   NSImageio::NSIspio::EPortIndex_IMGO,
                                   NSImageio::NSIspio::ISP_QUERY_X_PIX|
                                   NSImageio::NSIspio::ISP_QUERY_STRIDE_PIX|
                                   NSImageio::NSIspio::ISP_QUERY_STRIDE_BYTE,
                                   fmt,
                                   sensorSize[k].w,
                                   queryRst,
                                   e_PixMode[k]
                                   );


               p1Buf.size = sensorSize[k].h* queryRst.stride_byte;
               mpImemDrv->allocVirtBuf(&p1Buf);
               ////imem buffer 2 image heap
               MUINT32 bufStridesInBytes[3] = {queryRst.stride_byte, 0, 0};
               MINT32 bufBoundaryInBytes[3] = {0, 0, 0};
               PortBufInfo_v1 portBufInfo = PortBufInfo_v1( p1Buf.memID,p1Buf.virtAddr,0,p1Buf.bufSecu, p1Buf.bufCohe);

               IImageBufferAllocator::ImgParam imgParam = IImageBufferAllocator::ImgParam((bitmap(SEN_PIX_BITDEPTH)),
                                                       sensorSize[k], bufStridesInBytes, bufBoundaryInBytes, 1);
               sp<ImageBufferHeap> pHeap = ImageBufferHeap::create( LOG_TAG, imgParam,portBufInfo,MTRUE);
               pImgBuffer[k][__IMGO][i] = pHeap->createImageBuffer();
               pImgBuffer[k][__IMGO][i]->incStrong(pImgBuffer);
               pImgBuffer[k][__IMGO][i]->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
               LOG_INF("senidx:0x%x img buffer(%d): imgo pa:0x%x\n",k,i,pImgBuffer[k][__IMGO][i]->getBufPA(0));
           }
       }
   }
   //////////////////////////////////////////////////////////////////////////////////////////////////////////
   //push enque buf/dummy frame into drv before start. to set FBC number
   //enque buf
   //in this example, enque buf with mag : 1,2,3
   LOG_INF("start push enque buf/dummy frame into drv\n");
   QBufInfo buf;
   MSize _rrz ;
   for(int k=0;k<dualSensor;k++){
       for(int i=0;i<Enque_buf;i++){
           buf.mvOut.clear();
           if(enablePort & __RRZO_ENABLE){
               //note:crop size can't < outsize
               if(i==1)//scale
               {
                    _rrz = __SCALE_SIZE(__RRZO,pImgBuffer[k][__RRZO][i]->getImgFormat(),sensorSize[k],RRZ_SCALING_RATIO,e_PixMode[k]);
                    _cropsize = __CROP_SIZE(__RRZO,pImgBuffer[k][__RRZO][i]->getImgFormat(),sensorSize[k],e_PixMode[k]);
               }
               else
               {
                    _rrz = __SCALE_SIZE(__RRZO,pImgBuffer[k][__RRZO][i]->getImgFormat(),sensorSize[k],1,e_PixMode[k]);
                    _cropsize = __CROP_SIZE_2(__RRZO,pImgBuffer[k][__RRZO][i]->getImgFormat(),sensorSize[k],e_PixMode[k]);
               }
               printf("_rrz size : 0x%x 0x%x (0x%x 0x%x)(0x%x 0x%x) %d\n",_rrz.w,_rrz.h,sensorSize[k].w,sensorSize[k].h, _cropsize.w, _cropsize.h, pImgBuffer[k][__RRZO][i]->getImgFormat());
               NSCam::NSIoPipe::NSCamIOPipe::BufInfo _buf = NSCam::NSIoPipe::NSCamIOPipe::BufInfo(PORT_RRZO,pImgBuffer[k][__RRZO][i],_rrz,MRect(__CROP_Start(sensorSize[k],_cropsize,pImgBuffer[k][__RRZO][i]->getImgFormat()),_cropsize),i+1);
               buf.mvOut.push_back(_buf);
           }
           if(enablePort & __IMGO_ENABLE){
               if(i==1)
               {
                   _cropsize = __CROP_SIZE(__IMGO,pImgBuffer[k][__IMGO][i]->getImgFormat(),sensorSize[k],e_PixMode[k]);
               }
               else
               {
                   _cropsize = __CROP_SIZE_2(__IMGO,pImgBuffer[k][__IMGO][i]->getImgFormat(),sensorSize[k],e_PixMode[k]);
               }
               NSCam::NSIoPipe::NSCamIOPipe::BufInfo _buf = NSCam::NSIoPipe::NSCamIOPipe::BufInfo(PORT_IMGO,pImgBuffer[k][__IMGO][i],_cropsize,MRect(__CROP_Start(sensorSize[k],_cropsize,pImgBuffer[k][__IMGO][i]->getImgFormat()),_cropsize),i+1);
               buf.mvOut.push_back(_buf);
           }
           mpCamIO[k]->enque(buf);
       }
   }
   //in this example, dummy buf with mag : 4,5,6
   for(int k=0;k<dualSensor;k++){
       for(int i=Dummy_buf;i<(Enque_buf+Dummy_buf);i++){
           buf.mvOut.clear();
           if(enablePort & __RRZO_ENABLE){
               _rrz = __SCALE_SIZE(__RRZO,pImgBuffer[k][__RRZO][i]->getImgFormat(),sensorSize[k],RRZ_SCALING_RATIO,e_PixMode[k]);
               _cropsize = __CROP_SIZE(__RRZO,pImgBuffer[k][__RRZO][i]->getImgFormat(),sensorSize[k],e_PixMode[k]);
               NSCam::NSIoPipe::NSCamIOPipe::BufInfo _buf = NSCam::NSIoPipe::NSCamIOPipe::BufInfo(PORT_RRZO,pImgBuffer[k][__RRZO][i],_rrz,MRect(__CROP_Start(sensorSize[k],_cropsize,pImgBuffer[k][__RRZO][i]->getImgFormat()),_cropsize),i+1);
               buf.mvOut.push_back(_buf);
           }
           if(enablePort & __IMGO_ENABLE){
               if(i%3==1)
               {
                   _cropsize = __CROP_SIZE(__IMGO,pImgBuffer[k][__IMGO][i]->getImgFormat(),sensorSize[k],e_PixMode[k]);
               }
               else
               {
                   _cropsize = __CROP_SIZE_2(__IMGO,pImgBuffer[k][__IMGO][i]->getImgFormat(),sensorSize[k],e_PixMode[k]);
               }
               NSCam::NSIoPipe::NSCamIOPipe::BufInfo _buf = NSCam::NSIoPipe::NSCamIOPipe::BufInfo(PORT_IMGO,pImgBuffer[k][__IMGO][i],_cropsize,MRect(__CROP_Start(sensorSize[k],_cropsize,pImgBuffer[k][__IMGO][i]->getImgFormat()),_cropsize),i+1);
               buf.mvOut.push_back(_buf);
           }
           mpCamIO[k]->DummyFrame(buf);
       }
   }

   //////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //start isp
   LOG_INF("!!!ISP START\n");
   for(int i=0;i<dualSensor;i++)
       mpCamIO[i]->start();

   /**
       note: need to start isp fist(can't use deque first,than use another to start isp, or deadlock will hanppen)
   */
   //start deque/enque thread
   LOG_INF("start deque/denque thread\n");
   _test_camio_ * pCamio[_MAXTG_] = {NULL};
   for(int i=0;i<dualSensor;i++){
       pCamio[i] = _test_camio_::create();
       pCamio[i]->m_pNormalP_FrmB = mpCamIO[i];
       pCamio[i]->m_enablePort = enablePort;
       pCamio[i]->mPixMode = e_PixMode[i];
       pCamio[i]->m_Sensoridx = i;
       if(pCamio[i]->m_enablePort & __RRZO_ENABLE)
           pCamio[i]->m_prepalce_Buf[__RRZO] = pImgBuffer[i][__RRZO][(Enque_buf+Dummy_buf)];
       if(pCamio[i]->m_enablePort & __IMGO_ENABLE)
           pCamio[i]->m_prepalce_Buf[__IMGO] = pImgBuffer[i][__IMGO][(Enque_buf+Dummy_buf)];
       pCamio[i]->m_TgSize = sensorSize[i];
       pCamio[i]->startThread();
   }

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //p1 stop
   for(int i=0;i<dualSensor;i++){
       pCamio[i]->stopThread();
       LOG_INF("!!!!!!!! thread stoped... !!!!!!!!\n");
       mpCamIO[i]->stop();
       LOG_INF("!!!!!!!! isp stoped... !!!!!!!!\n");


       mpCamIO[i]->uninit();
       mpCamIO[i]->destroyInstance(LOG_TAG);

       //poweroff sensor
       pSensorHalObj[i]->powerOff(LOG_TAG,1, &sensorArray[i]);
   }
   LOG_INF("!!!!!!!!done... !!!!!!!!\n");
   return ret;

}

