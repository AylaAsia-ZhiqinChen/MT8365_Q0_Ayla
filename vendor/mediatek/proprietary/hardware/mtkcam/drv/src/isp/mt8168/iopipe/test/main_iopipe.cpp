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


#define LOG_TAG "iopipetest"

#include <vector>
#include <list>
#include <stdio.h>
#include <sys/time.h>

#include <stdlib.h>
//
#include <errno.h>
#include <fcntl.h>

//#include <mtkcam/utils/std/Log.h>
#include <mtkcam/def/common.h>
#include <mtkcam/utils/std/common.h>
using namespace NSCam;
using namespace NSCam::Utils;
#include <utils/Mutex.h>
using namespace android;
#include <queue>
using namespace std;

//// gtest benchmark
//#include <benchmark/benchmark.h>


//
#include <mtkcam/drv/iopipe/PostProc/INormalStream.h>
#include "PortMap.h"
//#include <iopipe/PostProc/INormalStream.h>

using namespace NSIoPipe;
using namespace NSPostProc;


//
#include "ispio_utility.h"
#include "ispio_pipe_ports.h"
#include <mtkcam/utils/imgbuf/IImageBuffer.h>
#include <utils/StrongPointer.h>
#include <mtkcam/utils/imgbuf/ImageBufferHeap.h>
#include <mtkcam/utils/imgbuf/ISecureImageBufferHeap.h>
#include "imem_drv.h"
#include <semaphore.h>
#include <pthread.h>
#include <mtkcam/def/PriorityDefs.h>

#include <utils/threads.h>

//tuning
#include <drv/tuning_mgr.h>
#include <drv/isp_drv.h>

//for Thread
#include <sys/prctl.h>
using namespace android;
using namespace NSCam::Utils;
// For property_get().
#include <cutils/properties.h>

MBOOL g_basicMultiFrameCallback = MFALSE;

MVOID basicMultiFrameCallback(QParams& rParams)
{
	//printf("--- [basicMultiFrame callback func]\n");

	g_basicMultiFrameCallback = MTRUE;
}

void saveImage(char *file_name, IImageBuffer *buf)
{
    char folderpath[] = "/data/pass2";
    char filepath[256];

    if(!makePath(folderpath,0660))
    {
        printf("makePath [%s] fail",folderpath);
        return;
    }

    snprintf(filepath, sizeof(filepath), "%s/%s", folderpath, file_name);
    buf->saveToFile(filepath);
}

/******************************************************************************
* save the buffer to the file
*******************************************************************************/
static bool
p2saveBufToFile(char const*const fname, MUINT8 *const buf, MUINT32 const size)
{
    int nw, cnt = 0;
    uint32_t written = 0;

    //LOG_INF("(name, buf, size) = (%s, %x, %d)", fname, buf, size);
    //LOG_INF("opening file [%s]\n", fname);
    int fd = ::open(fname, O_RDWR | O_CREAT, S_IRWXU);
    if (fd < 0) {
        printf(": failed to create file [%s]: %s \n", fname, ::strerror(errno));
        return false;
    }

    //LOG_INF("writing %d bytes to file [%s]\n", size, fname);
    while (written < size) {
        nw = ::write(fd,
                     buf + written,
                     size - written);
        if (nw < 0) {
            printf(": failed to write to file [%s]: %s\n", fname, ::strerror(errno));
            break;
        }
        written += nw;
        cnt++;
    }
    //LOG_INF("done writing %d bytes to file [%s] in %d passes\n", size, fname, cnt);
    ::close(fd);
    return true;
}

#include "pic/imgi_640x480_bayer10.h"
#include "pic/imgi_1280x720_bayer10.h"
#include "pic/imgi_2560x1440_bayer10.h"
#include "pic/imgi_320x240_yuyv.h"
#include "pic/imgi_640x480_yuy2.h"
#include "pic/alpha1_320x240_bayer8.h"
#include "pic/alpha2_320x240_bayer8.h"
#include "pic/g_imgi_5280x960_yuy2.h"
#include "pic/g_vipi_5280x960_yuy2.h"
#include "pic/g_ufdi_5280x960_b8.h"
#include "pic/g_vip3i_5280x960_b8.h"
#include "pic/tuningbuf_p2a.h"
#include "pic/tuningbuf_vss.h"
#include "pic/tuningbuf_iprawtpipe.h"
/*******************************************************************************
*  Main Function
*
*  case 0 0: preview path, 1 in / 2 out (wdma/wroto) with same src crop and same dst size
*  case 0 1: preview path, 1 in / 2 out (wdma/wroto) with different src crop but same dst size
*  case 0 2: preview path, 1 in / 2 out (wdma/wroto) with different src crop and different dst size(rotate 90 for wroto)
*  case 0 3: preview path, 1 in / 3 out (wdma/wroto/img2o) with different src crop and different dst size
*  case 1 0: vfb path,p2a 1 in / 2 out (img3o/wdmao) with different src crop and different dst size
*                            p2b path, 4 in / 2 out (wdma/wroto)
*  case 2 0: mfb mixing path,4 in / 3 out (img3o/wdmao/wroto) with different src crop and different dst size
********************************************************************************/
int case0(int type);
int case0_yuvIn(int type, int loopNum);
int case0_Secure(int type, int sec_fmt);
int case1();
int case2();
int basicVss(int testType,int loopNum);
int basicSecureVss(int type,int loopNum);
MVOID basicSecureP2ACallback(QParams& rParams);
MBOOL g_basicSecureVssCallback = MFALSE;
int P2A_FG();
int Basic_ip_raw_with_tile(int type);
int Basic_ip_raw();

int test_iopipe(int argc, char** argv)
{
    int ret = 0;

    int testNum = atoi(argv[1]);	
	int testType = atoi(argv[2]);	 
	int loopNum = atoi(argv[3]);
    printf("testNum %d, testType %d - %d", testNum, testType, loopNum);
    switch(testNum)
    {
        case 0:
            ret=case0(testType);
            break;
        case 1:
            ret=case0_yuvIn(testType, loopNum);
            break;
        case 2:
            //ret=case2();
            break;
        case 3:
            ret=basicVss(testType,loopNum);
            break;
        case 5:
           // ret=P2A_FG();
            break;
        case 6:
            ret=Basic_ip_raw_with_tile(testType);
            break;
        case 7:
            ret = Basic_ip_raw();
            break;
        case 8:     // Secure
            ret = basicSecureVss(testType, loopNum);
            break;
        case 9:     // Secure
            ret = case0_Secure(testType, loopNum);
            break;
        default:
            break;
    }

    return ret;
}

/*********************************************************************************/
int case0(int type)
{
    int ret=0;
    MBOOL isV3 = MFALSE;    //temp disable tuning path cuz tuning function not ready

	ALOGE("[TTT]--- [IP Based basicP2A(%d)...enterrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr]\n", type);
	NSCam::NSIoPipe::NSPostProc::INormalStream* pStream;
	pStream= NSCam::NSIoPipe::NSPostProc::INormalStream::createInstance(0xFFFF);
	pStream->init("basicP2A");
	ALOGE("[TTT]--- [basicP2A(%d)...pStream init done]\n", type);
    IMemDrv* mpImemDrv=NULL;
    mpImemDrv=IMemDrv::createInstance();
    mpImemDrv->init();

	QParams enqueParams;
	FrameParams frameParams;
	FrameParams frameParams2;
	
	frameParams.mStreamTag = NSCam::NSIoPipe::NSPostProc::ENormalStreamTag_Normal;
	frameParams2.mStreamTag = NSCam::NSIoPipe::NSPostProc::ENormalStreamTag_Normal;

    //input image
    IMEM_BUF_INFO imgiBuf;
    MINT32 bufBoundaryInBytes[3] = {0, 0, 0};
    IImageBuffer* srcBuffer;
    MUINT32 bufStridesInBytes[3] = {1600, 0, 0};
    imgiBuf.size=sizeof(g_imgi_array_1280x720_b10);
    mpImemDrv->allocVirtBuf(&imgiBuf);
    memcpy( (MUINT8*)(imgiBuf.virtAddr), (MUINT8*)(g_imgi_array_1280x720_b10), imgiBuf.size);
    //imem buffer 2 image heap
        PortBufInfo_v1 portBufInfo = PortBufInfo_v1( imgiBuf.memID,imgiBuf.virtAddr,0,imgiBuf.bufSecu, imgiBuf.bufCohe);
        IImageBufferAllocator::ImgParam imgParam = IImageBufferAllocator::ImgParam((eImgFmt_BAYER10),MSize(1280, 720), bufStridesInBytes, bufBoundaryInBytes, 1);
        sp<ImageBufferHeap> pHeap = ImageBufferHeap::create( LOG_TAG, imgParam,portBufInfo,true);
    srcBuffer = pHeap->createImageBuffer();
        srcBuffer->incStrong(srcBuffer);
        srcBuffer->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
    Input src;
    src.mPortID=PORT_IMGI;
    src.mBuffer=srcBuffer;
	src.mPortID.group=0;
    frameParams.mvIn.push_back(src);
	frameParams2.mvIn.push_back(src);
	printf("--- [basicP2A(%d)...push src done]\n", type);

    //crop information
    MCrpRsInfo crop;
    crop.mGroupID=1;
    MCrpRsInfo crop2;
    crop2.mGroupID=2;
    MCrpRsInfo crop3;
    crop3.mGroupID=3;
    IImageBuffer* outBuffer=NULL;
#if 0
	//crop information	  
	MCrpRsInfo crop;	  
	crop.mFrameGroup=0;	 
	crop.mGroupID=1;	 
	MCrpRsInfo crop2;	  
	crop2.mFrameGroup=0;	  
	crop2.mGroupID=2;    
	MCrpRsInfo crop3;	
	crop3.mFrameGroup=0;	
	crop3.mGroupID=3;	 
	crop.mCropRect.p_fractional.x=0;	 
	crop.mCropRect.p_fractional.y=0;	 
	crop.mCropRect.p_integral.x=0;    
	crop.mCropRect.p_integral.y=0;	 
	crop.mCropRect.s.w=_imgi_w_;	 
	crop.mCropRect.s.h=_imgi_h_;	 
	crop.mResizeDst.w=_imgi_w_;	
	crop.mResizeDst.h=_imgi_h_;    
	crop2.mCropRect.p_fractional.x=0;	
	crop2.mCropRect.p_fractional.y=0;	 
	crop2.mCropRect.p_integral.x=0;	
	crop2.mCropRect.p_integral.y=0;    
	crop2.mCropRect.s.w=_imgi_w_;	
	crop2.mCropRect.s.h=_imgi_h_;	
	crop2.mResizeDst.w=_imgi_w_;	 
	crop2.mResizeDst.h=_imgi_h_;	 
	crop3.mCropRect.p_fractional.x=0;	  
	crop3.mCropRect.p_fractional.y=0;    
	crop3.mCropRect.p_integral.x=0;	  
	crop3.mCropRect.p_integral.y=0;	 
	crop3.mCropRect.s.w=_imgi_w_;	  
	crop3.mCropRect.s.h=_imgi_h_;    
	crop3.mResizeDst.w=_imgi_w_;    
	crop3.mResizeDst.h=_imgi_h_;    
	frameParams.mvCropRsInfo.push_back(crop);	
	frameParams.mvCropRsInfo.push_back(crop2);	  
	frameParams.mvCropRsInfo.push_back(crop3);
#endif
	printf("--- [sizeof(isp_reg_t)(%x)..(%x)]\n", sizeof(isp_reg_t), (sizeof(isp_reg_t) - 0x4000));

    isp_reg_t tuningDat;    
    memcpy( (MUINT8*)(&tuningDat.CAM_CTL_START), (MUINT8*)(p2_tuning_buffer), sizeof(p2_tuning_buffer)/*(sizeof(isp_reg_t) - 0x4000)*/);
	frameParams.mTuningData = (MVOID*)&tuningDat;
	frameParams2.mTuningData = (MVOID*)&tuningDat;
	g_basicMultiFrameCallback = MFALSE;
    enqueParams.mpfnCallback = basicMultiFrameCallback;
    for(int i=0;i<1;i++)
    {
        frameParams.mvOut.clear();
        frameParams.mvCropRsInfo.clear();
		frameParams2.mvOut.clear();
	    frameParams2.mvCropRsInfo.clear();

        crop.mCropRect.p_fractional.x=0;
        crop.mCropRect.p_fractional.y=0;
        crop.mCropRect.p_integral.x=0;
        crop.mCropRect.p_integral.y=0;
        crop.mCropRect.s.w=1280;
        crop.mCropRect.s.h=720;
        crop.mResizeDst.w=1280;
        crop.mResizeDst.h=720;
        int wdma_out_w=0,wdma_out_h=0;
        int wrot_out_w=0,wrot_out_h=0;
        switch(type)
        {
            case 0:
            default:
                //same crop for mdp 2 output
                crop2.mCropRect.p_fractional.x=0;
                crop2.mCropRect.p_fractional.y=0;
                crop2.mCropRect.p_integral.x=0;
                crop2.mCropRect.p_integral.y=0;
                crop2.mCropRect.s.w=1280;
                crop2.mCropRect.s.h=720;
                crop2.mResizeDst.w=1280;
                crop2.mResizeDst.h=720;
                crop3.mCropRect.p_fractional.x=0;
                crop3.mCropRect.p_fractional.y=0;
                crop3.mCropRect.p_integral.x=0;
                crop3.mCropRect.p_integral.y=0;
                crop3.mCropRect.s.w=1280;
                crop3.mCropRect.s.h=720;
                crop3.mResizeDst.w=1280;
                crop3.mResizeDst.h=720;
                wdma_out_w=1280;
                wdma_out_h=720;
                wrot_out_w=1280;
                wrot_out_h=720;
                break;
            case 1:
                //different crop/same dst size
                crop2.mCropRect.p_fractional.x=0;
                crop2.mCropRect.p_fractional.y=0;
                crop2.mCropRect.p_integral.x=0;
                crop2.mCropRect.p_integral.y=0;
                crop2.mCropRect.s.w=1000;
                crop2.mCropRect.s.h=600;
                crop2.mResizeDst.w=1280;
                crop2.mResizeDst.h=720;
                crop3.mCropRect.p_fractional.x=0;
                crop3.mCropRect.p_fractional.y=0;
                crop3.mCropRect.p_integral.x=280;
                crop3.mCropRect.p_integral.y=120;
                crop3.mCropRect.s.w=1000;
                crop3.mCropRect.s.h=600;
                crop3.mResizeDst.w=1280;
                crop3.mResizeDst.h=720;
                wdma_out_w=1280;
                wdma_out_h=720;
                wrot_out_w=1280;
                wrot_out_h=720;
                break;
            case 2:
                //different crop/different dst size (wroto rotate)
                crop2.mCropRect.p_fractional.x=0;
                crop2.mCropRect.p_fractional.y=0;
                crop2.mCropRect.p_integral.x=0;
                crop2.mCropRect.p_integral.y=0;
                crop2.mCropRect.s.w=700;
                crop2.mCropRect.s.h=400;
                crop2.mResizeDst.w=640;
                crop2.mResizeDst.h=480;
                crop3.mCropRect.p_fractional.x=0;
                crop3.mCropRect.p_fractional.y=0;
                crop3.mCropRect.p_integral.x=280;
                crop3.mCropRect.p_integral.y=120;
                crop3.mCropRect.s.w=1000;
                crop3.mCropRect.s.h=600;
                crop3.mResizeDst.w=960;
                crop3.mResizeDst.h=640;
                wdma_out_w=640;
                wdma_out_h=480;
                wrot_out_w=640;
                wrot_out_h=960;
                break;
            case 3:
                crop.mCropRect.p_fractional.x=0;
                crop.mCropRect.p_fractional.y=0;
                crop.mCropRect.p_integral.x=100;
                crop.mCropRect.p_integral.y=100;
                crop.mCropRect.s.w=700;
                crop.mCropRect.s.h=350;
                crop.mResizeDst.w=320;
                crop.mResizeDst.h=240;
                crop2.mCropRect.p_fractional.x=0;
                crop2.mCropRect.p_fractional.y=0;
                crop2.mCropRect.p_integral.x=0;
                crop2.mCropRect.p_integral.y=0;
                crop2.mCropRect.s.w=700;
                crop2.mCropRect.s.h=400;
                crop2.mResizeDst.w=640;
                crop2.mResizeDst.h=480;
                crop3.mCropRect.p_fractional.x=0;
                crop3.mCropRect.p_fractional.y=0;
                crop3.mCropRect.p_integral.x=280;
                crop3.mCropRect.p_integral.y=120;
                crop3.mCropRect.s.w=1000;
                crop3.mCropRect.s.h=600;
                crop3.mResizeDst.w=960;
                crop3.mResizeDst.h=640;
                wdma_out_w=640;
                wdma_out_h=480;
                wrot_out_w=640;
                wrot_out_h=960;
                break;
        }
        frameParams.mvCropRsInfo.push_back(crop);
        frameParams.mvCropRsInfo.push_back(crop2);
        frameParams.mvCropRsInfo.push_back(crop3);
		frameParams2.mvCropRsInfo.push_back(crop);
				frameParams2.mvCropRsInfo.push_back(crop2);
				frameParams2.mvCropRsInfo.push_back(crop3);

        //output buffer
        IMEM_BUF_INFO img2oBuf;
        if(type==3)
        {    //simulate fd buffer from img2o
            img2oBuf.size=320*240*2;
                mpImemDrv->allocVirtBuf(&img2oBuf);
            memset((MUINT8*)img2oBuf.virtAddr, 0x0, img2oBuf.size);
            MUINT32 bufStridesInBytes_1[3] = {320*2,0,0};
            PortBufInfo_v1 portBufInfo_1 = PortBufInfo_v1( img2oBuf.memID,img2oBuf.virtAddr,0,img2oBuf.bufSecu, img2oBuf.bufCohe);
                IImageBufferAllocator::ImgParam imgParam_1 = IImageBufferAllocator::ImgParam((eImgFmt_YUY2),
                                                                MSize(320,240),  bufStridesInBytes_1, bufBoundaryInBytes, 1);
            sp<ImageBufferHeap> pHeap_1 = ImageBufferHeap::create( LOG_TAG, imgParam_1,portBufInfo_1,true);
            outBuffer = pHeap_1->createImageBuffer();
                outBuffer->incStrong(outBuffer);
                outBuffer->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
            Output dst;
            dst.mPortID=PORT_IMG2O;
            dst.mBuffer=outBuffer;
            dst.mPortID.group=0;
            frameParams.mvOut.push_back(dst);
			frameParams2.mvOut.push_back(dst);
        }

        IMEM_BUF_INFO wdmaoBuf;
        IMEM_BUF_INFO wrotoBuf;
        wdmaoBuf.size=wdma_out_w*wdma_out_h*2;
            mpImemDrv->allocVirtBuf(&wdmaoBuf);
        memset((MUINT8*)wdmaoBuf.virtAddr, 0x0, wdmaoBuf.size);
        MINT32 bufStridesInBytes_3[3] = {wdma_out_w, wdma_out_w/2, wdma_out_w/2};
        PortBufInfo_v1 portBufInfo_3 = PortBufInfo_v1( wdmaoBuf.memID,wdmaoBuf.virtAddr,0,wdmaoBuf.bufSecu, wdmaoBuf.bufCohe);
            IImageBufferAllocator::ImgParam imgParam_3 = IImageBufferAllocator::ImgParam((eImgFmt_YV12),
                                                            MSize(wdma_out_w, wdma_out_h),  bufStridesInBytes_3, bufBoundaryInBytes, 3);
        sp<ImageBufferHeap> pHeap_3 = ImageBufferHeap::create( LOG_TAG, imgParam_3,portBufInfo_3,true);
        IImageBuffer* outBuffer2 = pHeap_3->createImageBuffer();
            outBuffer2->incStrong(outBuffer2);
            outBuffer2->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
        Output dst2;
        dst2.mPortID=PORT_WDMAO;
        dst2.mBuffer=outBuffer2;
        dst2.mPortID.group=0;
        frameParams.mvOut.push_back(dst2);
		frameParams2.mvOut.push_back(dst2);
        //
        wrotoBuf.size=wrot_out_w*wrot_out_h*2;
            mpImemDrv->allocVirtBuf(&wrotoBuf);
        memset((MUINT8*)wrotoBuf.virtAddr, 0x0, wrotoBuf.size);
        MINT32 bufStridesInBytes_4[3] = {wrot_out_w*2,0,0};
        PortBufInfo_v1 portBufInfo_4 = PortBufInfo_v1( wrotoBuf.memID,wrotoBuf.virtAddr,0,wrotoBuf.bufSecu, wrotoBuf.bufCohe);
            IImageBufferAllocator::ImgParam imgParam_4 = IImageBufferAllocator::ImgParam((eImgFmt_YUY2),
                                                            MSize(wrot_out_w, wrot_out_h),  bufStridesInBytes_4, bufBoundaryInBytes, 1);
        sp<ImageBufferHeap> pHeap_4 = ImageBufferHeap::create( LOG_TAG, imgParam_4,portBufInfo_4,true);
        IImageBuffer* outBuffer3 = pHeap_4->createImageBuffer();
            outBuffer3->incStrong(outBuffer3);
            outBuffer3->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
        Output dst3;
        dst3.mPortID=PORT_WROTO;
        dst3.mBuffer=outBuffer3;
        dst3.mPortID.group=0;
        if(type==2 || type==3)
        {
            dst3.mTransform=eTransform_ROT_90;
        }
        frameParams.mvOut.push_back(dst3);
		frameParams2.mvOut.push_back(dst3);
		enqueParams.mvFrameParams.push_back(frameParams);
		//enqueParams.mvFrameParams.push_back(frameParams2);

        //buffer operation
        mpImemDrv->cacheFlushAll();
        printf(": enque\n");
        ret=pStream->enque(enqueParams);
        if(!ret)
        {
            printf(" : ERR enque fail\n");
        }
        else
        {
            ALOGE("[TTT]: enque done\n");
        }

	 do{
            usleep(1000);
            if (MTRUE == g_basicMultiFrameCallback)
            {
                break;
            }
        }while(1);
        g_basicMultiFrameCallback = MFALSE;
        ALOGE("[TTT]: Frame done\n");
		/*
        QParams dequeParams;
        ret=pStream->deque(dequeParams);
        if(!ret)
        {
            printf(" : ERR deque fail\n");
        }
        else
        {
            printf(": deque done\n");
        }*/
        

        //dump image
        char filename[256];
        if(type==3)
        {
            sprintf(filename, "/data//P2UT_DiffViewAngle_case0_%d_%d_img2o_%dx%d.yuv",type,i, 320,240);
                p2saveBufToFile(filename, reinterpret_cast<MUINT8*>(img2oBuf.virtAddr), 320 *240 * 2);
        }
            sprintf(filename, "/data//P2UT_DiffViewAngle_case0_%d_%d_wdmao_%dx%d.yuv",type,i, wdma_out_w,wdma_out_h);
            p2saveBufToFile(filename, reinterpret_cast<MUINT8*>(wdmaoBuf.virtAddr), wdma_out_w *wdma_out_h * 2);
            char filename2[256];
            sprintf(filename2, "/data//P2UT_DiffViewAngle_case0_%d_%d_wroto_%dx%d.yuv",type,i, wrot_out_w,wrot_out_h);
            p2saveBufToFile(filename2, reinterpret_cast<MUINT8*>(wrotoBuf.virtAddr), wrot_out_w *wrot_out_h * 2);

            //
            if(type==3)
            {
                outBuffer->unlockBuf(LOG_TAG);
                mpImemDrv->freeVirtBuf(&img2oBuf);
            }
                outBuffer2->unlockBuf(LOG_TAG);
                outBuffer3->unlockBuf(LOG_TAG);
                mpImemDrv->freeVirtBuf(&wdmaoBuf);
                mpImemDrv->freeVirtBuf(&wrotoBuf);
            //
    }
    //
    pStream->uninit("basicP2A");   
	pStream->destroyInstance();

    mpImemDrv->uninit();
    mpImemDrv->destroyInstance();
   // mpIspDrv->uninit(LOG_TAG);
       // mpIspDrv->destroyInstance();
    return ret;
}

int case0_yuvIn(int type, int loopNum)
{
    int ret=0;
    MBOOL isV3 = MFALSE;    //temp disable tuning path cuz tuning function not ready

    printf("--- [IP Based basicP2A(%d)...enterrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr]\n", type);
    NSCam::NSIoPipe::NSPostProc::INormalStream* pStream;
    pStream= NSCam::NSIoPipe::NSPostProc::INormalStream::createInstance(0xFFFF);
    pStream->init("basicP2A_yuvIn");
    printf("--- [basicP2A_yuvIn(%d)...pStream init done]\n", type);
    IMemDrv* mpImemDrv=NULL;
    mpImemDrv=IMemDrv::createInstance();
    mpImemDrv->init();

    QParams enqueParams;
    FrameParams frameParams;
    FrameParams frameParams2;
    frameParams.mStreamTag = NSCam::NSIoPipe::NSPostProc::ENormalStreamTag_Normal;
    frameParams2.mStreamTag = NSCam::NSIoPipe::NSPostProc::ENormalStreamTag_Normal;

    //input image
    MUINT32 _imgi_w_=640, _imgi_h_=480;
    IMEM_BUF_INFO imgiBuf;
    MINT32 bufBoundaryInBytes[3] = {0, 0, 0};
    IImageBuffer* srcBuffer;
    MUINT32 bufStridesInBytes[3] = {_imgi_w_*2, 0, 0};
    imgiBuf.size=sizeof(g_imgi_array_640x480_yuy2);
    mpImemDrv->allocVirtBuf(&imgiBuf);
    memcpy( (MUINT8*)(imgiBuf.virtAddr), (MUINT8*)(g_imgi_array_640x480_yuy2), imgiBuf.size);
    //imem buffer 2 image heap
    PortBufInfo_v1 portBufInfo = PortBufInfo_v1( imgiBuf.memID,imgiBuf.virtAddr,0,imgiBuf.bufSecu, imgiBuf.bufCohe);
    IImageBufferAllocator::ImgParam imgParam = IImageBufferAllocator::ImgParam((eImgFmt_YUY2),MSize(_imgi_w_, _imgi_h_), bufStridesInBytes, bufBoundaryInBytes, 1);
    sp<ImageBufferHeap> pHeap = ImageBufferHeap::create( LOG_TAG, imgParam,portBufInfo,true);
    srcBuffer = pHeap->createImageBuffer();
    srcBuffer->incStrong(srcBuffer);
    srcBuffer->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
    Input src;
    src.mPortID=PORT_IMGI;
    src.mBuffer=srcBuffer;
    src.mPortID.group=0;
    frameParams.mvIn.push_back(src);
    frameParams2.mvIn.push_back(src);
    printf("--- [basicP2A(%d)...push src done]\n", type);


    MUINT32 _output_w_=640, _output_h_=480;
    //crop information
    MCrpRsInfo crop;
    crop.mGroupID=1;
    MCrpRsInfo crop2;
    crop2.mGroupID=2;
    MCrpRsInfo crop3;
    crop3.mGroupID=3;
    IImageBuffer* outBuffer=NULL;
    printf("--- [sizeof(isp_reg_t)(%x)..(%x)]\n", sizeof(isp_reg_t), (sizeof(isp_reg_t) - 0x4000));

    isp_reg_t tuningDat;    
    memset((MVOID*)&tuningDat, 0, sizeof(isp_reg_t));
    memcpy( (MUINT8*)(&tuningDat.CAM_CTL_START), (MUINT8*)(p2_tuning_buffer), sizeof(p2_tuning_buffer)/*(sizeof(isp_reg_t) - 0x4000)*/);
    frameParams.mTuningData = (MVOID*)&tuningDat;
    frameParams2.mTuningData = (MVOID*)&tuningDat;
    g_basicMultiFrameCallback = MFALSE;
    enqueParams.mpfnCallback = basicMultiFrameCallback;
    {
        frameParams.mvOut.clear();
        frameParams.mvCropRsInfo.clear();
        frameParams2.mvOut.clear();
        frameParams2.mvCropRsInfo.clear();

        crop.mCropRect.p_fractional.x=0;
        crop.mCropRect.p_fractional.y=0;
        crop.mCropRect.p_integral.x=100;
        crop.mCropRect.p_integral.y=100;
        crop.mCropRect.s.w=600;
        crop.mCropRect.s.h=350;
        crop.mResizeDst.w=320;
        crop.mResizeDst.h=240;
        int wdma_out_w=0,wdma_out_h=0;
        int wrot_out_w=0,wrot_out_h=0;

        //same crop for mdp 2 output
        crop2.mCropRect.p_fractional.x=0;
        crop2.mCropRect.p_fractional.y=0;
        crop2.mCropRect.p_integral.x=0;
        crop2.mCropRect.p_integral.y=0;
        crop2.mCropRect.s.w=_imgi_w_;
        crop2.mCropRect.s.h=_imgi_h_;
        crop2.mResizeDst.w=_output_w_;
        crop2.mResizeDst.h=_output_h_;
        crop3.mCropRect.p_fractional.x=0;
        crop3.mCropRect.p_fractional.y=0;
        crop3.mCropRect.p_integral.x=0;
        crop3.mCropRect.p_integral.y=0;
        crop3.mCropRect.s.w=_imgi_w_;
        crop3.mCropRect.s.h=_imgi_h_;
        crop3.mResizeDst.w=_output_w_;
        crop3.mResizeDst.h=_output_h_;
        wdma_out_w=_output_w_;
        wdma_out_h=_output_h_;
        wrot_out_w=_output_w_;
        wrot_out_h=_output_h_;

        frameParams.mvCropRsInfo.push_back(crop);
        frameParams.mvCropRsInfo.push_back(crop2);
        frameParams.mvCropRsInfo.push_back(crop3);
        frameParams2.mvCropRsInfo.push_back(crop);
        frameParams2.mvCropRsInfo.push_back(crop2);
        frameParams2.mvCropRsInfo.push_back(crop3);

        //output buffer
        IMEM_BUF_INFO img2oBuf;
        if(type==3 || type == 4)
        {    //simulate fd buffer from img2o
            img2oBuf.size=320*240*2;
                mpImemDrv->allocVirtBuf(&img2oBuf);
            memset((MUINT8*)img2oBuf.virtAddr, 0x0, img2oBuf.size);
            MUINT32 bufStridesInBytes_1[3] = {320*2,0,0};
            PortBufInfo_v1 portBufInfo_1 = PortBufInfo_v1( img2oBuf.memID,img2oBuf.virtAddr,0,img2oBuf.bufSecu, img2oBuf.bufCohe);
                IImageBufferAllocator::ImgParam imgParam_1 = IImageBufferAllocator::ImgParam((eImgFmt_YUY2),
                                                                MSize(320,240),  bufStridesInBytes_1, bufBoundaryInBytes, 1);
            sp<ImageBufferHeap> pHeap_1 = ImageBufferHeap::create( LOG_TAG, imgParam_1,portBufInfo_1,true);
            outBuffer = pHeap_1->createImageBuffer();
                outBuffer->incStrong(outBuffer);
                outBuffer->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
            Output dst;
            dst.mPortID=PORT_IMG2O;
            dst.mBuffer=outBuffer;
            dst.mPortID.group=0;
            frameParams.mvOut.push_back(dst);
            frameParams2.mvOut.push_back(dst);
        }

        IMEM_BUF_INFO wdmaoBuf;
        IMEM_BUF_INFO wrotoBuf;
        wdmaoBuf.size=wdma_out_w*wdma_out_h*2;
            mpImemDrv->allocVirtBuf(&wdmaoBuf);
        memset((MUINT8*)wdmaoBuf.virtAddr, 0x0, wdmaoBuf.size);
        MINT32 bufStridesInBytes_3[3] = {wdma_out_w, wdma_out_w/2, wdma_out_w/2};
        PortBufInfo_v1 portBufInfo_3 = PortBufInfo_v1( wdmaoBuf.memID,wdmaoBuf.virtAddr,0,wdmaoBuf.bufSecu, wdmaoBuf.bufCohe);
            IImageBufferAllocator::ImgParam imgParam_3 = IImageBufferAllocator::ImgParam((eImgFmt_YUY2),
                                                            MSize(wdma_out_w, wdma_out_h),  bufStridesInBytes_3, bufBoundaryInBytes, 3);
        sp<ImageBufferHeap> pHeap_3 = ImageBufferHeap::create( LOG_TAG, imgParam_3,portBufInfo_3,true);
        IImageBuffer* outBuffer2 = pHeap_3->createImageBuffer();
            outBuffer2->incStrong(outBuffer2);
            outBuffer2->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
        Output dst2;
        dst2.mPortID=PORT_WDMAO;
        dst2.mBuffer=outBuffer2;
        dst2.mPortID.group=0;
        if(type!=4) {
            frameParams.mvOut.push_back(dst2);
            frameParams2.mvOut.push_back(dst2);
        }
        //
        wrotoBuf.size=wrot_out_w*wrot_out_h*2;
            mpImemDrv->allocVirtBuf(&wrotoBuf);
        memset((MUINT8*)wrotoBuf.virtAddr, 0x0, wrotoBuf.size);
        MINT32 bufStridesInBytes_4[3] = {wrot_out_w,wrot_out_w/2,wrot_out_w/2};
        PortBufInfo_v1 portBufInfo_4 = PortBufInfo_v1( wrotoBuf.memID,wrotoBuf.virtAddr,0,wrotoBuf.bufSecu, wrotoBuf.bufCohe);
            IImageBufferAllocator::ImgParam imgParam_4 = IImageBufferAllocator::ImgParam((eImgFmt_YV12),
                                                            MSize(wrot_out_w, wrot_out_h),  bufStridesInBytes_4, bufBoundaryInBytes, 3);
        sp<ImageBufferHeap> pHeap_4 = ImageBufferHeap::create( LOG_TAG, imgParam_4,portBufInfo_4,true);
        IImageBuffer* outBuffer3 = pHeap_4->createImageBuffer();
            outBuffer3->incStrong(outBuffer3);
            outBuffer3->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
        Output dst3;
        dst3.mPortID=PORT_WROTO;
        dst3.mBuffer=outBuffer3;
        dst3.mPortID.group=0;
        if(type==2 || type==3)
        {
            dst3.mTransform=eTransform_ROT_90;
        }
        if(type!=4) {
            frameParams.mvOut.push_back(dst3);
            frameParams2.mvOut.push_back(dst3);
        }                
        enqueParams.mvFrameParams.push_back(frameParams);
        //enqueParams.mvFrameParams.push_back(frameParams2);

        //buffer operation
        mpImemDrv->cacheFlushAll();
        printf(": enque\n");      

        for(int i=0; i < loopNum; i++)
        {
            //enque
            ret = pStream->enque(enqueParams);
            if(!ret)
            {
                printf("---ERRRRRRRRR [(%d_%d)..enque fail\n]", type, i);
            }
            else
            {
                printf("---[(%d_%d)..enque done\n]",type, i);
                while(!g_basicMultiFrameCallback)
                usleep(100000);
                g_basicMultiFrameCallback = MFALSE;
                printf("--- [(%d_%d)...deque done\n]", type, i);
            }
        }

        //dump image
        char filename[256];
        if(type==3 || type == 4)
        {
            sprintf(filename, "/data//P2UT_DiffViewAngle_case0_%d_%d_img2o_%dx%d.yuv",type,0, 320,240);
                p2saveBufToFile(filename, reinterpret_cast<MUINT8*>(img2oBuf.virtAddr), 320 *240 * 2);
        }
        MUINT8* pBuf = reinterpret_cast<MUINT8*>(wdmaoBuf.virtAddr);
        printf("wdmaoBuf: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
                        *pBuf, *(pBuf+1), *(pBuf+2), *(pBuf+3), *(pBuf+4), *(pBuf+5), *(pBuf+6), *(pBuf+7),
                        *(pBuf+8), *(pBuf+9), *(pBuf+10), *(pBuf+11), *(pBuf+12), *(pBuf+13), *(pBuf+14), *(pBuf+15));           

        sprintf(filename, "/data//P2UT_DiffViewAngle_case0_%d_%d_wdmao_%dx%d.yuv",type,0, wdma_out_w,wdma_out_h);
        p2saveBufToFile(filename, reinterpret_cast<MUINT8*>(wdmaoBuf.virtAddr), wdma_out_w *wdma_out_h * 2);
        char filename2[256];
        sprintf(filename2, "/data//P2UT_DiffViewAngle_case0_%d_%d_wroto_%dx%d.yuv",type,0, wrot_out_w,wrot_out_h);
        p2saveBufToFile(filename2, reinterpret_cast<MUINT8*>(wrotoBuf.virtAddr), wrot_out_w *wrot_out_h * 2);

        //
        if(type==3)
        {
            outBuffer->unlockBuf(LOG_TAG);
            mpImemDrv->freeVirtBuf(&img2oBuf);
        }
        outBuffer2->unlockBuf(LOG_TAG);
        outBuffer3->unlockBuf(LOG_TAG);
        mpImemDrv->freeVirtBuf(&wdmaoBuf);
        mpImemDrv->freeVirtBuf(&wrotoBuf);
            //
    }
    //
    pStream->uninit("basicP2A");   
    pStream->destroyInstance();

    mpImemDrv->uninit();
    mpImemDrv->destroyInstance();
   // mpIspDrv->uninit(LOG_TAG);
       // mpIspDrv->destroyInstance();
    return ret;
}

int case0_Secure(int type, int sec_fmt)
{
	int ret = 0;
	MBOOL isV3 = MFALSE;    //temp disable tuning path cuz tuning function not ready

	printf("--- [IP Based basicSecureP2A(%d-%d)...enterrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr]\n", type, sec_fmt);
	NSCam::NSIoPipe::NSPostProc::INormalStream* pStream;
	pStream = NSCam::NSIoPipe::NSPostProc::INormalStream::createInstance(0xFFFF);
	enum EDIPSecureEnum SecureTag = EDIPSecure_SECURE;

	pStream->init("basicSecureP2A", SecureTag);
	printf("--- [basicSecureP2A(%d)...pStream init done]\n", type);
	IMemDrv* mpImemDrv = NULL;
	mpImemDrv = IMemDrv::createInstance();
	mpImemDrv->init();

	QParams enqueParams;
	FrameParams frameParams;
	FrameParams frameParams2;

	frameParams.mStreamTag = NSCam::NSIoPipe::NSPostProc::ENormalStreamTag_Normal;
	frameParams.mSecureFra = ((SecureTag > EDIPSecure_NONE) && (SecureTag < EDIPSecure_MAX));
	frameParams2.mStreamTag = NSCam::NSIoPipe::NSPostProc::ENormalStreamTag_Normal;
	frameParams2.mSecureFra = ((SecureTag > EDIPSecure_NONE) && (SecureTag < EDIPSecure_MAX));

	//input image
	MUINT32 _imgi_w_ = 1280, _imgi_h_ = 720;
	IImageBuffer* srcBuffer;
	MINT32 bufBoundaryInBytes[3] = { 0, 0, 0 };
	MUINT32 bufStridesInBytes[3] = { 1600, 0, 0 };

	//imem buffer 2 image heap
	IImageBufferAllocator::ImgParam imgParam = IImageBufferAllocator::ImgParam((eImgFmt_BAYER10), MSize(_imgi_w_, _imgi_h_), bufStridesInBytes, bufBoundaryInBytes, 1);
	sp<ISecureImageBufferHeap> pSecHeap = ISecureImageBufferHeap::create("basicSecureP2A", imgParam,
		ISecureImageBufferHeap::AllocExtraParam(0, 1, 0, MFALSE, SecType::mem_secure));
	if (pSecHeap == NULL) {
		printf("[basicSecureP2A] Stuff ISecureImageBufferHeap create fail\n");
		return 0;
	}

	srcBuffer = pSecHeap->createImageBuffer();
	srcBuffer->incStrong(srcBuffer);
	srcBuffer->lockBuf(LOG_TAG, eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
	Input src;
	src.mPortID = PORT_IMGI;
	src.mBuffer = srcBuffer;
	src.mPortID.group = 0;
	src.mSecureTag = EDIPSecure_SECURE;
	src.mSecHandle = 0;
	frameParams.mvIn.push_back(src);
	frameParams2.mvIn.push_back(src);
	printf("--- [basicSecureP2A(%d)...push src done]\n", type);

	//crop information
	MCrpRsInfo crop;
	crop.mGroupID = 1;
	MCrpRsInfo crop2;
	crop2.mGroupID = 2;
	MCrpRsInfo crop3;
	crop3.mGroupID = 3;
	IImageBuffer * outBuffer = NULL;
#if 0
	//crop information
	MCrpRsInfo crop;
	crop.mFrameGroup = 0;
	crop.mGroupID = 1;
	MCrpRsInfo crop2;
	crop2.mFrameGroup = 0;
	crop2.mGroupID = 2;
	MCrpRsInfo crop3;
	crop3.mFrameGroup = 0;
	crop3.mGroupID = 3;
	crop.mCropRect.p_fractional.x = 0;
	crop.mCropRect.p_fractional.y = 0;
	crop.mCropRect.p_integral.x = 0;
	crop.mCropRect.p_integral.y = 0;
	crop.mCropRect.s.w = _imgi_w_;
	crop.mCropRect.s.h = _imgi_h_;
	crop.mResizeDst.w = _imgi_w_;
	crop.mResizeDst.h = _imgi_h_;
	crop2.mCropRect.p_fractional.x = 0;
	crop2.mCropRect.p_fractional.y = 0;
	crop2.mCropRect.p_integral.x = 0;
	crop2.mCropRect.p_integral.y = 0;
	crop2.mCropRect.s.w = _imgi_w_;
	crop2.mCropRect.s.h = _imgi_h_;
	crop2.mResizeDst.w = _imgi_w_;
	crop2.mResizeDst.h = _imgi_h_;
	crop3.mCropRect.p_fractional.x = 0;
	crop3.mCropRect.p_fractional.y = 0;
	crop3.mCropRect.p_integral.x = 0;
	crop3.mCropRect.p_integral.y = 0;
	crop3.mCropRect.s.w = _imgi_w_;
	crop3.mCropRect.s.h = _imgi_h_;
	crop3.mResizeDst.w = _imgi_w_;
	crop3.mResizeDst.h = _imgi_h_;
	frameParams.mvCropRsInfo.push_back(crop);
	frameParams.mvCropRsInfo.push_back(crop2);
	frameParams.mvCropRsInfo.push_back(crop3);
#endif
	printf("--- [sizeof(isp_reg_t)(%x)..(%x)]\n", sizeof(isp_reg_t), (sizeof(isp_reg_t) - 0x4000));

	isp_reg_t tuningDat;
	memcpy((MUINT8*)(&tuningDat.CAM_CTL_START), (MUINT8*)(p2_tuning_buffer), sizeof(p2_tuning_buffer)/*(sizeof(isp_reg_t) - 0x4000)*/);
	frameParams.mTuningData = (MVOID*)& tuningDat;
	frameParams2.mTuningData = (MVOID*)& tuningDat;
	g_basicMultiFrameCallback = MFALSE;
	enqueParams.mpfnCallback = basicMultiFrameCallback;
	for (int i = 0; i < 1; i++)
	{
		frameParams.mvOut.clear();
		frameParams.mvCropRsInfo.clear();
		frameParams2.mvOut.clear();
		frameParams2.mvCropRsInfo.clear();

		crop.mCropRect.p_fractional.x = 0;
		crop.mCropRect.p_fractional.y = 0;
		crop.mCropRect.p_integral.x = 0;
		crop.mCropRect.p_integral.y = 0;
		crop.mCropRect.s.w = 1280;
		crop.mCropRect.s.h = 720;
		crop.mResizeDst.w = 1280;
		crop.mResizeDst.h = 720;
		int wdma_out_w = 0, wdma_out_h = 0;
		int wrot_out_w = 0, wrot_out_h = 0;
		switch (type)
		{
		case 0:
		default:
			//same crop for mdp 2 output
			crop2.mCropRect.p_fractional.x = 0;
			crop2.mCropRect.p_fractional.y = 0;
			crop2.mCropRect.p_integral.x = 0;
			crop2.mCropRect.p_integral.y = 0;
			crop2.mCropRect.s.w = 1280;
			crop2.mCropRect.s.h = 720;
			crop2.mResizeDst.w = 1280;
			crop2.mResizeDst.h = 720;
			crop3.mCropRect.p_fractional.x = 0;
			crop3.mCropRect.p_fractional.y = 0;
			crop3.mCropRect.p_integral.x = 0;
			crop3.mCropRect.p_integral.y = 0;
			crop3.mCropRect.s.w = 1280;
			crop3.mCropRect.s.h = 720;
			crop3.mResizeDst.w = 1280;
			crop3.mResizeDst.h = 720;
			wdma_out_w = 1280;
			wdma_out_h = 720;
			wrot_out_w = 1280;
			wrot_out_h = 720;
			break;
		case 1:
			//different crop/same dst size
			crop2.mCropRect.p_fractional.x = 0;
			crop2.mCropRect.p_fractional.y = 0;
			crop2.mCropRect.p_integral.x = 0;
			crop2.mCropRect.p_integral.y = 0;
			crop2.mCropRect.s.w = 1000;
			crop2.mCropRect.s.h = 600;
			crop2.mResizeDst.w = 1280;
			crop2.mResizeDst.h = 720;
			crop3.mCropRect.p_fractional.x = 0;
			crop3.mCropRect.p_fractional.y = 0;
			crop3.mCropRect.p_integral.x = 280;
			crop3.mCropRect.p_integral.y = 120;
			crop3.mCropRect.s.w = 1000;
			crop3.mCropRect.s.h = 600;
			crop3.mResizeDst.w = 1280;
			crop3.mResizeDst.h = 720;
			wdma_out_w = 1280;
			wdma_out_h = 720;
			wrot_out_w = 1280;
			wrot_out_h = 720;
			break;
		case 2:
			//different crop/different dst size (wroto rotate)
			crop2.mCropRect.p_fractional.x = 0;
			crop2.mCropRect.p_fractional.y = 0;
			crop2.mCropRect.p_integral.x = 0;
			crop2.mCropRect.p_integral.y = 0;
			crop2.mCropRect.s.w = 700;
			crop2.mCropRect.s.h = 400;
			crop2.mResizeDst.w = 640;
			crop2.mResizeDst.h = 480;
			crop3.mCropRect.p_fractional.x = 0;
			crop3.mCropRect.p_fractional.y = 0;
			crop3.mCropRect.p_integral.x = 280;
			crop3.mCropRect.p_integral.y = 120;
			crop3.mCropRect.s.w = 1000;
			crop3.mCropRect.s.h = 600;
			crop3.mResizeDst.w = 960;
			crop3.mResizeDst.h = 640;
			wdma_out_w = 640;
			wdma_out_h = 480;
			wrot_out_w = 640;
			wrot_out_h = 960;
			break;
		case 3:
			crop.mCropRect.p_fractional.x=0;
			crop.mCropRect.p_fractional.y=0;
			crop.mCropRect.p_integral.x=100;
			crop.mCropRect.p_integral.y=100;
			crop.mCropRect.s.w=700;
			crop.mCropRect.s.h=350;
			crop.mResizeDst.w=320;
			crop.mResizeDst.h=240;
			crop2.mCropRect.p_fractional.x=0;
			crop2.mCropRect.p_fractional.y=0;
			crop2.mCropRect.p_integral.x=0;
			crop2.mCropRect.p_integral.y=0;
			crop2.mCropRect.s.w=700;
			crop2.mCropRect.s.h=400;
			crop2.mResizeDst.w=640;
			crop2.mResizeDst.h=480;
			crop3.mCropRect.p_fractional.x=0;
			crop3.mCropRect.p_fractional.y=0;
			crop3.mCropRect.p_integral.x=280;
			crop3.mCropRect.p_integral.y=120;
			crop3.mCropRect.s.w=1000;
			crop3.mCropRect.s.h=600;
			crop3.mResizeDst.w=960;
			crop3.mResizeDst.h=640;
			wdma_out_w=640;
			wdma_out_h=480;
			wrot_out_w=640;
			wrot_out_h=960;
			break;
		}
		frameParams.mvCropRsInfo.push_back(crop);
		frameParams.mvCropRsInfo.push_back(crop2);
		frameParams.mvCropRsInfo.push_back(crop3);
		frameParams2.mvCropRsInfo.push_back(crop);
		frameParams2.mvCropRsInfo.push_back(crop2);
		frameParams2.mvCropRsInfo.push_back(crop3);

		//output buffer
		if (type == 3)
		{    //simulate fd buffer from img2o
			MUINT32 bufStridesInBytes_1[3] = { 320 * 2,0,0 };
			IImageBufferAllocator::ImgParam imgParam_1 = IImageBufferAllocator::ImgParam((eImgFmt_YUY2),
				MSize(320, 240), bufStridesInBytes_1, bufBoundaryInBytes, 1);
			sp<ISecureImageBufferHeap> pHeap_1 = ISecureImageBufferHeap::create("basicSecureP2A", imgParam_1,
				ISecureImageBufferHeap::AllocExtraParam(0, 1, 0, MFALSE, SecType::mem_secure));
			if (pHeap_1 == NULL) {
				printf("[basicSecureP2A] Stuff ImageBufferHeap create fail\n");
				return 0;
			}

			outBuffer = pHeap_1->createImageBuffer();
			outBuffer->incStrong(outBuffer);
			outBuffer->lockBuf(LOG_TAG, eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
			Output dst;
			dst.mPortID = PORT_IMG2O;
			dst.mBuffer = outBuffer;
			dst.mPortID.group = 0;
			dst.mSecureTag = EDIPSecure_SECURE;
			frameParams.mvOut.push_back(dst);
			frameParams2.mvOut.push_back(dst);
		}

		sp<ISecureImageBufferHeap> pHeap_3;

		switch (sec_fmt) {
			case 0:	/* YUY2 - 1 plane */
			default:
			{
				MINT32 bufStridesInBytes_3[3] = {wdma_out_w * 2, 0, 0};
				IImageBufferAllocator::ImgParam imgParam_3 = IImageBufferAllocator::ImgParam((eImgFmt_YUY2),
					MSize(wdma_out_w, wdma_out_h), bufStridesInBytes_3, bufBoundaryInBytes, 1);
				pHeap_3 = ISecureImageBufferHeap::create("basicSecureP2A", imgParam_3,
					ISecureImageBufferHeap::AllocExtraParam(0, 1, 0, MFALSE, SecType::mem_secure));
			}
			break;
			case 1:	/* YV12 - 3 planes, contig. secure buffer */
			{
				MINT32 bufStridesInBytes_3[3] = {wdma_out_w, wdma_out_w / 2, wdma_out_w / 2};
				IImageBufferAllocator::ImgParam imgParam_3 = IImageBufferAllocator::ImgParam((eImgFmt_YV12),
					MSize(wdma_out_w, wdma_out_h), bufStridesInBytes_3, bufBoundaryInBytes, 3);
				pHeap_3 = ISecureImageBufferHeap::create("basicSecureP2A", imgParam_3,
					ISecureImageBufferHeap::AllocExtraParam(0, 1, 0, MTRUE/*contig.*/, SecType::mem_secure));
			}
			break;
			case 2:	/* YV12 - 3 planes, non-contig. secure buffers */
			{
				MINT32 bufStridesInBytes_3[3] = {wdma_out_w, wdma_out_w / 2, wdma_out_w / 2};
				IImageBufferAllocator::ImgParam imgParam_3 = IImageBufferAllocator::ImgParam((eImgFmt_YV12),
					MSize(wdma_out_w, wdma_out_h), bufStridesInBytes_3, bufBoundaryInBytes, 3);
				pHeap_3 = ISecureImageBufferHeap::create("basicSecureP2A", imgParam_3,
					ISecureImageBufferHeap::AllocExtraParam(0, 1, 0, MFALSE, SecType::mem_secure));
			}
			break;
		}

		IImageBuffer* outBuffer2 = pHeap_3->createImageBuffer();
		outBuffer2->incStrong(outBuffer2);
		outBuffer2->lockBuf(LOG_TAG, eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
		Output dst2;
		dst2.mPortID = PORT_WDMAO;
		dst2.mBuffer = outBuffer2;
		dst2.mPortID.group = 0;
		dst2.mSecureTag = EDIPSecure_SECURE;
		frameParams.mvOut.push_back(dst2);
		frameParams2.mvOut.push_back(dst2);
		//
		//
		printf("sec_fmt(%d): wdam outBuffer2[0]:PA:%p , VA:%p\n", sec_fmt, outBuffer2->getBufPA(0), outBuffer2->getBufVA(0));
		printf("sec_fmt(%d): wdam outBuffer2[1]:PA:%p , VA:%p\n", sec_fmt, outBuffer2->getBufPA(1), outBuffer2->getBufVA(1));
		printf("sec_fmt(%d): wdam outBuffer2[2]:PA:%p , VA:%p\n", sec_fmt, outBuffer2->getBufPA(2), outBuffer2->getBufVA(2));
		//
		MINT32 bufStridesInBytes_4[3] = { wrot_out_w * 2,0,0 };
		IImageBufferAllocator::ImgParam imgParam_4 = IImageBufferAllocator::ImgParam((eImgFmt_YUY2),
			MSize(wrot_out_w, wrot_out_h), bufStridesInBytes_4, bufBoundaryInBytes, 1);
		sp<ISecureImageBufferHeap> pHeap_4 = ISecureImageBufferHeap::create("basicSecureP2A", imgParam_4,
			ISecureImageBufferHeap::AllocExtraParam(0, 1, 0, MFALSE, SecType::mem_secure));
		IImageBuffer * outBuffer3 = pHeap_4->createImageBuffer();
		outBuffer3->incStrong(outBuffer3);
		outBuffer3->lockBuf(LOG_TAG, eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
		Output dst3;
		dst3.mPortID = PORT_WROTO;
		dst3.mBuffer = outBuffer3;
		dst3.mPortID.group = 0;
		dst3.mSecureTag = EDIPSecure_SECURE;
		if (type == 2 || type == 3)
		{
			dst3.mTransform = eTransform_ROT_90;
		}
		frameParams.mvOut.push_back(dst3);
		frameParams2.mvOut.push_back(dst3);
		enqueParams.mvFrameParams.push_back(frameParams);
		//enqueParams.mvFrameParams.push_back(frameParams2);

		//buffer operation
		//mpImemDrv->cacheFlushAll();
		printf(": enque->");
		ret = pStream->enque(enqueParams);
		if (!ret)
		{
			printf(" : ERR enque fail\n");
		}
		else
		{
			printf("enque done->");
			do {
				usleep(100000);
				if (MTRUE == g_basicMultiFrameCallback)
				{
					printf("Got DeQ Callback\n");
					break;
				}
			} while (1);
			g_basicMultiFrameCallback = MFALSE;
		}

		if (type == 3)
		{
			outBuffer->unlockBuf(LOG_TAG);
		}
		outBuffer2->unlockBuf(LOG_TAG);
		outBuffer3->unlockBuf(LOG_TAG);
	}

	pStream->uninit("basicSecureP2A");
	pStream->destroyInstance();
	mpImemDrv->uninit();
	mpImemDrv->destroyInstance();
	return ret;
}



/*********************************************************************************/
/*
int case1()
{
    int ret=0;
    MBOOL isV3 = MFALSE;    //temp disable tuning path cuz tuning function not ready
    NSCam::NSIoPipe::NSPostProc::IFeatureStream* pP2AStream;
    pP2AStream= NSCam::NSIoPipe::NSPostProc::IFeatureStream::createInstance(LOG_TAG, NSCam::NSIoPipe::NSPostProc::EFeatureStreamTag_vFB_Stream,0xFFFF, isV3);
    pP2AStream->init();
    NSCam::NSIoPipe::NSPostProc::IFeatureStream* pP2BStream;
    pP2BStream= NSCam::NSIoPipe::NSPostProc::IFeatureStream::createInstance(LOG_TAG, NSCam::NSIoPipe::NSPostProc::EFeatureStreamTag_vFB_FB,0xFFFF, isV3);
    pP2BStream->init();
    IMemDrv* mpImemDrv=NULL;
    mpImemDrv=IMemDrv::createInstance();
    mpImemDrv->init();
    IspDrv* mpIspDrv=NULL;
        mpIspDrv=IspDrv::createInstance();
        mpIspDrv->init(LOG_TAG);
    //

    //p2a data
    QParams p2aenqueParams;
    p2aenqueParams.mvPrivaData.push_back(NULL);
    p2aenqueParams.mvMagicNo.push_back(0);
    //input image
    IMEM_BUF_INFO imgiBuf;
    MINT32 bufBoundaryInBytes[3] = {0, 0, 0};
    IImageBuffer* srcBuffer;
    MUINT32 bufStridesInBytes[3] = {3200, 0, 0};
        imgiBuf.size=sizeof(g_imgi_array_2560x1440_b10);
    mpImemDrv->allocVirtBuf(&imgiBuf);
        memcpy( (MUINT8*)(imgiBuf.virtAddr), (MUINT8*)(g_imgi_array_2560x1440_b10), imgiBuf.size);
         //imem buffer 2 image heap
        PortBufInfo_v1 portBufInfo = PortBufInfo_v1( imgiBuf.memID,imgiBuf.virtAddr,0,imgiBuf.bufSecu, imgiBuf.bufCohe);
        IImageBufferAllocator::ImgParam imgParam = IImageBufferAllocator::ImgParam((eImgFmt_BAYER10),MSize(2560, 1440), bufStridesInBytes, bufBoundaryInBytes, 1);
        sp<ImageBufferHeap> pHeap = ImageBufferHeap::create( LOG_TAG, imgParam,portBufInfo,true);
    srcBuffer = pHeap->createImageBuffer();
        srcBuffer->incStrong(srcBuffer);
        srcBuffer->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
    Input src;
    src.mPortID=IMGI;
    src.mBuffer=srcBuffer;
    p2aenqueParams.mvIn.push_back(src);
    //crop information
    MCrpRsInfo crop;
    crop.mGroupID=1;
    MCrpRsInfo crop2;
    crop2.mGroupID=2;
    MCrpRsInfo crop3;
    crop3.mGroupID=3;

    //p2b data
    QParams p2benqueParams;
    p2benqueParams.mvPrivaData.push_back(NULL);
    p2benqueParams.mvMagicNo.push_back(0);
    //input image
    IMEM_BUF_INFO p2b_imgiBuf;
    IImageBuffer* p2b_srcBuffer;
    MUINT32 p2b_bufStridesInBytes[3] = {640, 0, 0};
        p2b_imgiBuf.size=sizeof(g_imgi_320x240_yuyv);
    mpImemDrv->allocVirtBuf(&p2b_imgiBuf);
        memcpy( (MUINT8*)(p2b_imgiBuf.virtAddr), (MUINT8*)(g_imgi_320x240_yuyv), p2b_imgiBuf.size);
         //imem buffer 2 image heap
        PortBufInfo_v1 p2b_portBufInfo = PortBufInfo_v1( p2b_imgiBuf.memID,p2b_imgiBuf.virtAddr,0,p2b_imgiBuf.bufSecu, p2b_imgiBuf.bufCohe);
        IImageBufferAllocator::ImgParam p2b_imgParam = IImageBufferAllocator::ImgParam((eImgFmt_YUY2),MSize(320, 240), p2b_bufStridesInBytes, bufBoundaryInBytes, 1);
        sp<ImageBufferHeap> p2b_pHeap = ImageBufferHeap::create( LOG_TAG, p2b_imgParam,p2b_portBufInfo,true);
    p2b_srcBuffer = p2b_pHeap->createImageBuffer();
        p2b_srcBuffer->incStrong(p2b_srcBuffer);
        p2b_srcBuffer->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
    Input p2b_src;
    p2b_src.mPortID=IMGI;
    p2b_src.mBuffer=p2b_srcBuffer;
    p2benqueParams.mvIn.push_back(p2b_src);
    //alpha maps
    IMEM_BUF_INFO p2b_ufdiBuf;
    IImageBuffer* p2b_alpha1Buffer;
    MUINT32 p2b_alphabufStridesInBytes[3] = {320, 0, 0};
        p2b_ufdiBuf.size=sizeof(g_alpha1_320x240_b8);
    mpImemDrv->allocVirtBuf(&p2b_ufdiBuf);
        memcpy( (MUINT8*)(p2b_ufdiBuf.virtAddr), (MUINT8*)(g_alpha1_320x240_b8), p2b_ufdiBuf.size);
         //imem buffer 2 image heap
        PortBufInfo_v1 p2b_alpha1_portBufInfo = PortBufInfo_v1( p2b_ufdiBuf.memID,p2b_ufdiBuf.virtAddr,0,p2b_ufdiBuf.bufSecu, p2b_ufdiBuf.bufCohe);
        IImageBufferAllocator::ImgParam p2b_alpha1_imgParam = IImageBufferAllocator::ImgParam((eImgFmt_BAYER8),MSize(320, 240), p2b_alphabufStridesInBytes, bufBoundaryInBytes, 1);
        sp<ImageBufferHeap> p2b_alpha1_pHeap = ImageBufferHeap::create( LOG_TAG, p2b_alpha1_imgParam,p2b_alpha1_portBufInfo,true);
    p2b_alpha1Buffer = p2b_alpha1_pHeap->createImageBuffer();
        p2b_alpha1Buffer->incStrong(p2b_alpha1Buffer);
        p2b_alpha1Buffer->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
    Input p2b_alpha1;
    p2b_alpha1.mPortID=UFDI;
    p2b_alpha1.mBuffer=p2b_alpha1Buffer;
    p2benqueParams.mvIn.push_back(p2b_alpha1);
    //
    //alpha maps
    IMEM_BUF_INFO p2b_lceiBuf;
    IImageBuffer* p2b_alpha2Buffer;
        p2b_lceiBuf.size=sizeof(g_alpha2_320x240_b8);
    mpImemDrv->allocVirtBuf(&p2b_lceiBuf);
        memcpy( (MUINT8*)(p2b_lceiBuf.virtAddr), (MUINT8*)(g_alpha2_320x240_b8), p2b_lceiBuf.size);
         //imem buffer 2 image heap
        PortBufInfo_v1 p2b_alpha2_portBufInfo = PortBufInfo_v1( p2b_lceiBuf.memID,p2b_lceiBuf.virtAddr,0,p2b_lceiBuf.bufSecu, p2b_lceiBuf.bufCohe);
        IImageBufferAllocator::ImgParam p2b_alpha2_imgParam = IImageBufferAllocator::ImgParam((eImgFmt_BAYER8),MSize(320, 240), p2b_alphabufStridesInBytes, bufBoundaryInBytes, 1);
        sp<ImageBufferHeap> p2b_alpha2_pHeap = ImageBufferHeap::create( LOG_TAG, p2b_alpha2_imgParam,p2b_alpha2_portBufInfo,true);
    p2b_alpha2Buffer = p2b_alpha2_pHeap->createImageBuffer();
        p2b_alpha2Buffer->incStrong(p2b_alpha2Buffer);
        p2b_alpha2Buffer->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
    Input p2b_alpha2;
    p2b_alpha2.mPortID=LCEI;
    p2b_alpha2.mBuffer=p2b_alpha2Buffer;
    p2benqueParams.mvIn.push_back(p2b_alpha2);
    //crop information
    MCrpRsInfo p2b_crop;
    p2b_crop.mGroupID=1;
    MCrpRsInfo p2b_crop2;
    p2b_crop2.mGroupID=2;
    MCrpRsInfo p2b_crop3;
    p2b_crop3.mGroupID=3;

    //output buffer
    for(int i=0;i<5;i++)
    {
        //clear
        p2aenqueParams.mvOut.clear();
        p2aenqueParams.mvCropRsInfo.clear();

        ////////////////////////////////////////////////////////
        //p2a
        ////////////////////////////////////////////////////////
        //crop
        crop.mCropRect.p_fractional.x=0;
        crop.mCropRect.p_fractional.y=0;
        crop.mCropRect.p_integral.x=0;
        crop.mCropRect.p_integral.y=0;
        crop.mCropRect.s.w=2560;
        crop.mCropRect.s.h=1440;
        crop.mResizeDst.w=1920;
        crop.mResizeDst.h=1080;
        int wdma_out_w=0,wdma_out_h=0;
        int wrot_out_w=0,wrot_out_h=0;
        //different crop/same dst size
        if(i%2==0)
        {
            crop2.mCropRect.p_fractional.x=0;
            crop2.mCropRect.p_fractional.y=0;
            crop2.mCropRect.p_integral.x=0;
            crop2.mCropRect.p_integral.y=0;
            crop2.mCropRect.s.w=1800;
            crop2.mCropRect.s.h=800;
            crop2.mResizeDst.w=1280;
            crop2.mResizeDst.h=720;
            crop3.mCropRect.p_fractional.x=0;
            crop3.mCropRect.p_fractional.y=0;
            crop3.mCropRect.p_integral.x=120;
            crop3.mCropRect.p_integral.y=280;
            crop3.mCropRect.s.w=1800;
            crop3.mCropRect.s.h=800;
            crop3.mResizeDst.w=1920;
            crop3.mResizeDst.h=1080;
        }
        else
        {
            crop2.mCropRect.p_fractional.x=0;
            crop2.mCropRect.p_fractional.y=0;
            crop2.mCropRect.p_integral.x=120;
            crop2.mCropRect.p_integral.y=280;
            crop2.mCropRect.s.w=1800;
            crop2.mCropRect.s.h=800;
            crop2.mResizeDst.w=1280;
            crop2.mResizeDst.h=720;
            crop3.mCropRect.p_fractional.x=0;
            crop3.mCropRect.p_fractional.y=0;
            crop3.mCropRect.p_integral.x=0;
            crop3.mCropRect.p_integral.y=0;
            crop3.mCropRect.s.w=1800;
            crop3.mCropRect.s.h=800;
            crop3.mResizeDst.w=1920;
            crop3.mResizeDst.h=1080;
        }
        wdma_out_w=1280;
        wdma_out_h=720;
        wrot_out_w=1920;
        wrot_out_h=1080;
        p2aenqueParams.mvCropRsInfo.push_back(crop);
        p2aenqueParams.mvCropRsInfo.push_back(crop2);
        p2aenqueParams.mvCropRsInfo.push_back(crop3);

        //full size img3o
        IMEM_BUF_INFO img3oBuf;
        img3oBuf.size=1920*1080*2;
            mpImemDrv->allocVirtBuf(&img3oBuf);
        memset((MUINT8*)img3oBuf.virtAddr, 0xffffffff, img3oBuf.size);
        MUINT32 bufStridesInBytes_1[3] = {1920,960,960};
        PortBufInfo_v1 portBufInfo_1 = PortBufInfo_v1( img3oBuf.memID,img3oBuf.virtAddr,0,img3oBuf.bufSecu, img3oBuf.bufCohe);
            IImageBufferAllocator::ImgParam imgParam_1 = IImageBufferAllocator::ImgParam((eImgFmt_YV12),
                                                            MSize(1920,1080),  bufStridesInBytes_1, bufBoundaryInBytes, 3);
        sp<ImageBufferHeap> pHeap_1 = ImageBufferHeap::create( LOG_TAG, imgParam_1,portBufInfo_1,true);
        IImageBuffer* outBuffer = pHeap_1->createImageBuffer();
            outBuffer->incStrong(outBuffer);
            outBuffer->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
        Output dst;
        dst.mPortID=IMG3O;
        dst.mBuffer=outBuffer;
        dst.mPortID.group=0;
        p2aenqueParams.mvOut.push_back(dst);
        //
        IMEM_BUF_INFO wdmaoBuf;
        IMEM_BUF_INFO wrotoBuf;
        wdmaoBuf.size=wdma_out_w*wdma_out_h*2;
            mpImemDrv->allocVirtBuf(&wdmaoBuf);
        memset((MUINT8*)wdmaoBuf.virtAddr, 0xffffffff, wdmaoBuf.size);
        MINT32 bufStridesInBytes_3[3] = {wdma_out_w, wdma_out_w/2, wdma_out_w/2};
        PortBufInfo_v1 portBufInfo_3 = PortBufInfo_v1( wdmaoBuf.memID,wdmaoBuf.virtAddr,0,wdmaoBuf.bufSecu, wdmaoBuf.bufCohe);
            IImageBufferAllocator::ImgParam imgParam_3 = IImageBufferAllocator::ImgParam((eImgFmt_YV12),
                                                            MSize(wdma_out_w, wdma_out_h),  bufStridesInBytes_3, bufBoundaryInBytes, 3);
        sp<ImageBufferHeap> pHeap_3 = ImageBufferHeap::create( LOG_TAG, imgParam_3,portBufInfo_3,true);
        IImageBuffer* outBuffer2 = pHeap_3->createImageBuffer();
            outBuffer2->incStrong(outBuffer2);
            outBuffer2->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
        Output dst2;
        dst2.mPortID=WDMAO;
        dst2.mBuffer=outBuffer2;
        dst2.mPortID.group=0;
        p2aenqueParams.mvOut.push_back(dst2);
        //
        wrotoBuf.size=wrot_out_w*wrot_out_h*2;
            mpImemDrv->allocVirtBuf(&wrotoBuf);
        memset((MUINT8*)wrotoBuf.virtAddr, 0xffffffff, wrotoBuf.size);
        MINT32 bufStridesInBytes_4[3] = {wrot_out_w*2,0,0};
        PortBufInfo_v1 portBufInfo_4 = PortBufInfo_v1( wrotoBuf.memID,wrotoBuf.virtAddr,0,wrotoBuf.bufSecu, wrotoBuf.bufCohe);
            IImageBufferAllocator::ImgParam imgParam_4 = IImageBufferAllocator::ImgParam((eImgFmt_YUY2),
                                                            MSize(wrot_out_w, wrot_out_h),  bufStridesInBytes_4, bufBoundaryInBytes, 1);
        sp<ImageBufferHeap> pHeap_4 = ImageBufferHeap::create( LOG_TAG, imgParam_4,portBufInfo_4,true);
        IImageBuffer* outBuffer3 = pHeap_4->createImageBuffer();
            outBuffer3->incStrong(outBuffer3);
            outBuffer3->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
        Output dst3;
        dst3.mPortID=WROTO;
        dst3.mBuffer=outBuffer3;
        dst3.mPortID.group=0;
        p2aenqueParams.mvOut.push_back(dst3);

        //buffer operation
        mpImemDrv->cacheFlushAll();
        printf("(%d) : p2a enque\n",i);
        ret=pP2AStream->enque(p2aenqueParams);
        if(!ret)
        {
            printf("(%d) : p2a ERR enque fail\n",i);
        }
        else
        {
            printf("(%d) : p2a enque done\n",i);
        }
        QParams p2adequeParams;
        ret=pP2AStream->deque(p2adequeParams);
        if(!ret)
        {
            printf("(%d) : p2a ERR deque fail\n",i);
        }
        else
        {
            printf("(%d) : p2a deque done\n",i);
        }
     

        //dump image
        char filename[256];
        sprintf(filename, "/sdcard/Pictures/P2UT_DiffViewAngle_case1_p2a_%d_img3o_%dx%d.yuv",i, 1920,1080);
        saveBufToFile(filename, reinterpret_cast<MUINT8*>(p2adequeParams.mvOut[0].mBuffer->getBufVA(0)), 1920 *1080 * 2);
            sprintf(filename, "/sdcard/Pictures/P2UT_DiffViewAngle_case1_p2a_%d_wdmao_%dx%d.yuv",i, wdma_out_w,wdma_out_h);
            saveBufToFile(filename, reinterpret_cast<MUINT8*>(p2adequeParams.mvOut[1].mBuffer->getBufVA(0)), wdma_out_w *wdma_out_h * 2);
            sprintf(filename, "/sdcard/Pictures/P2UT_DiffViewAngle_case1_p2a_%d_wroto_%dx%d.yuv",i, wrot_out_w,wrot_out_h);
            saveBufToFile(filename, reinterpret_cast<MUINT8*>(p2adequeParams.mvOut[2].mBuffer->getBufVA(0)), wrot_out_w *wrot_out_h * 2);

        printf("(%d) : --- p2a done ---\n", i);
        ////////////////////////////////////////////////////////
        //p2b
        //vipi: 1920x1080
        //imgi: 320x240, ufdi: 320x240, lcei: 320x240
        ////////////////////////////////////////////////////////

        //clear
        p2benqueParams.mvOut.clear();
        p2benqueParams.mvCropRsInfo.clear();

        //img3o from p2a as vipi for p2b
        Input vipisrc;
                vipisrc.mPortID=VIPI;
                vipisrc.mBuffer=p2adequeParams.mvOut[0].mBuffer;
            vipisrc.mPortID.group=0;
                p2benqueParams.mvIn.push_back(vipisrc);

        //crop
        p2b_crop.mCropRect.p_fractional.x=0;
        p2b_crop.mCropRect.p_fractional.y=0;
        p2b_crop.mCropRect.p_integral.x=0;
        p2b_crop.mCropRect.p_integral.y=0;
        p2b_crop.mCropRect.s.w=320;
        p2b_crop.mCropRect.s.h=240;
        p2b_crop.mResizeDst.w=1920;
        p2b_crop.mResizeDst.h=1080;
        wdma_out_w=0;
        wdma_out_h=0;
        wrot_out_w=0;
        wrot_out_h=0;
        if(i%2==0)
        {
            //different crop/same dst size
            p2b_crop2.mCropRect.p_fractional.x=0;
            p2b_crop2.mCropRect.p_fractional.y=0;
            p2b_crop2.mCropRect.p_integral.x=0;
            p2b_crop2.mCropRect.p_integral.y=0;
            p2b_crop2.mCropRect.s.w=1800;
            p2b_crop2.mCropRect.s.h=800;
            p2b_crop2.mResizeDst.w=1280;
            p2b_crop2.mResizeDst.h=720;
            p2b_crop3.mCropRect.p_fractional.x=0;
            p2b_crop3.mCropRect.p_fractional.y=0;
            p2b_crop3.mCropRect.p_integral.x=120;
            p2b_crop3.mCropRect.p_integral.y=280;
            p2b_crop3.mCropRect.s.w=1800;
            p2b_crop3.mCropRect.s.h=800;
            p2b_crop3.mResizeDst.w=1920;
            p2b_crop3.mResizeDst.h=1080;
            wdma_out_w=1280;
            wdma_out_h=720;
            wrot_out_w=1920;
            wrot_out_h=1080;
        }
        else
        {
            //different crop/same dst size
            p2b_crop2.mCropRect.p_fractional.x=0;
            p2b_crop2.mCropRect.p_fractional.y=0;
            p2b_crop2.mCropRect.p_integral.x=120;
            p2b_crop2.mCropRect.p_integral.y=280;
            p2b_crop2.mCropRect.s.w=1800;
            p2b_crop2.mCropRect.s.h=800;
            p2b_crop2.mResizeDst.w=1280;
            p2b_crop2.mResizeDst.h=720;
            p2b_crop3.mCropRect.p_fractional.x=0;
            p2b_crop3.mCropRect.p_fractional.y=0;
            p2b_crop3.mCropRect.p_integral.x=0;
            p2b_crop3.mCropRect.p_integral.y=0;
            p2b_crop3.mCropRect.s.w=1800;
            p2b_crop3.mCropRect.s.h=800;
            p2b_crop3.mResizeDst.w=960;
            p2b_crop3.mResizeDst.h=640;
            wdma_out_w=1280;
            wdma_out_h=720;
            wrot_out_w=960;
            wrot_out_h=640;
        }
        p2benqueParams.mvCropRsInfo.push_back(p2b_crop);
        p2benqueParams.mvCropRsInfo.push_back(p2b_crop2);
        p2benqueParams.mvCropRsInfo.push_back(p2b_crop3);

        //output
        IMEM_BUF_INFO p2b_wdmaoBuf;
        IMEM_BUF_INFO p2b_wrotoBuf;
        p2b_wdmaoBuf.size=wdma_out_w*wdma_out_h*2;
            mpImemDrv->allocVirtBuf(&p2b_wdmaoBuf);
        memset((MUINT8*)p2b_wdmaoBuf.virtAddr, 0xffffffff, p2b_wdmaoBuf.size);
        MINT32 p2b_bufStridesInBytes_3[3] = {wdma_out_w, wdma_out_w/2, wdma_out_w/2};
        PortBufInfo_v1 p2b_portBufInfo_3 = PortBufInfo_v1( p2b_wdmaoBuf.memID,p2b_wdmaoBuf.virtAddr,0,p2b_wdmaoBuf.bufSecu, p2b_wdmaoBuf.bufCohe);
            IImageBufferAllocator::ImgParam p2b_imgParam_3 = IImageBufferAllocator::ImgParam((eImgFmt_YV12),
                                                            MSize(wdma_out_w, wdma_out_h),  p2b_bufStridesInBytes_3, bufBoundaryInBytes, 3);
        sp<ImageBufferHeap> p2b_pHeap_3 = ImageBufferHeap::create( LOG_TAG, p2b_imgParam_3,p2b_portBufInfo_3,true);
        IImageBuffer* p2b_outBuffer2 = p2b_pHeap_3->createImageBuffer();
            p2b_outBuffer2->incStrong(p2b_outBuffer2);
            p2b_outBuffer2->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
        Output p2b_dst;
        p2b_dst.mPortID=WDMAO;
        p2b_dst.mBuffer=p2b_outBuffer2;
        p2b_dst.mPortID.group=0;
        p2benqueParams.mvOut.push_back(p2b_dst);
        //
        p2b_wrotoBuf.size=wrot_out_w*wrot_out_h*2;
            mpImemDrv->allocVirtBuf(&p2b_wrotoBuf);
        memset((MUINT8*)p2b_wrotoBuf.virtAddr, 0xffffffff, p2b_wrotoBuf.size);
        MINT32 p2b_bufStridesInBytes_4[3] = {wrot_out_w*2,0,0};
        PortBufInfo_v1 p2b_portBufInfo_4 = PortBufInfo_v1( p2b_wrotoBuf.memID,p2b_wrotoBuf.virtAddr,0,p2b_wrotoBuf.bufSecu, p2b_wrotoBuf.bufCohe);
            IImageBufferAllocator::ImgParam p2b_imgParam_4 = IImageBufferAllocator::ImgParam((eImgFmt_YUY2),
                                                            MSize(wrot_out_w, wrot_out_h),  p2b_bufStridesInBytes_4, bufBoundaryInBytes, 1);
        sp<ImageBufferHeap> p2b_pHeap_4 = ImageBufferHeap::create( LOG_TAG, p2b_imgParam_4,p2b_portBufInfo_4,true);
        IImageBuffer* p2b_outBuffer3 = p2b_pHeap_4->createImageBuffer();
            p2b_outBuffer3->incStrong(p2b_outBuffer3);
            p2b_outBuffer3->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
        Output p2b_dst2;
        p2b_dst2.mPortID=WROTO;
        p2b_dst2.mBuffer=p2b_outBuffer3;
        p2b_dst2.mPortID.group=0;
        p2benqueParams.mvOut.push_back(p2b_dst2);

        //buffer operation
        mpImemDrv->cacheFlushAll();
        printf("(%d) : p2b enque\n",i);
        ret=pP2BStream->enque(p2benqueParams);
        if(!ret)
        {
            printf("(%d) : p2b ERR enque fail\n",i);
        }
        else
        {
            printf("(%d) : p2b enque done\n",i);
        }
        QParams p2bdequeParams;
        ret=pP2BStream->deque(p2bdequeParams);
        if(!ret)
        {
            printf("(%d) : p2b ERR deque fail\n",i);
        }
        else
        {
            printf("(%d) : p2b deque done\n",i);
        }
        //dump image
            sprintf(filename, "/sdcard/Pictures/P2UT_DiffViewAngle_case1_p2b_%d_wdmao_%dx%d.yuv",i, wdma_out_w,wdma_out_h);
            saveBufToFile(filename, reinterpret_cast<MUINT8*>(p2bdequeParams.mvOut[0].mBuffer->getBufVA(0)), wdma_out_w *wdma_out_h * 2);
            sprintf(filename, "/sdcard/Pictures/P2UT_DiffViewAngle_case1_p2b_%d_wroto_%dx%d.yuv",i, wrot_out_w,wrot_out_h);
            saveBufToFile(filename, reinterpret_cast<MUINT8*>(p2bdequeParams.mvOut[1].mBuffer->getBufVA(0)), wrot_out_w *wrot_out_h * 2);

        printf("(%d) : --- p2b done ---\n", i);

        //unmap and free buffer
        //p2a
                outBuffer->unlockBuf(LOG_TAG);
                outBuffer2->unlockBuf(LOG_TAG);
                outBuffer3->unlockBuf(LOG_TAG);
                mpImemDrv->freeVirtBuf(&img3oBuf);
                mpImemDrv->freeVirtBuf(&wdmaoBuf);
                mpImemDrv->freeVirtBuf(&wrotoBuf);
                //p2b
                p2b_outBuffer2->unlockBuf(LOG_TAG);
                p2b_outBuffer3->unlockBuf(LOG_TAG);
                mpImemDrv->freeVirtBuf(&p2b_wdmaoBuf);
                mpImemDrv->freeVirtBuf(&p2b_wrotoBuf);
    }
    //
    pP2AStream->uninit();
    pP2AStream->destroyInstance(LOG_TAG);
    pP2BStream->uninit();
    pP2BStream->destroyInstance(LOG_TAG);
    mpImemDrv->uninit();
    mpImemDrv->destroyInstance();
    mpIspDrv->uninit(LOG_TAG);
        mpIspDrv->destroyInstance();
    return ret;
}
*/
/*********************************************************************************/
				/*
int case2()
{
    int ret=0;
    MBOOL isV3 = MFALSE;    //temp disable tuning path cuz tuning function not ready
    NSCam::NSIoPipe::NSPostProc::IFeatureStream* pMFBMixStream;
    pMFBMixStream= NSCam::NSIoPipe::NSPostProc::IFeatureStream::createInstance(LOG_TAG, NSCam::NSIoPipe::NSPostProc::EFeatureStreamTag_MFB_Mix,0xFFFF, isV3);
    pMFBMixStream->init();
    IMemDrv* mpImemDrv=NULL;
    mpImemDrv=IMemDrv::createInstance();
    mpImemDrv->init();
    IspDrv* mpIspDrv=NULL;
        mpIspDrv=IspDrv::createInstance();
        mpIspDrv->init(LOG_TAG);
    //

    //data
    QParams enqueParams;
    enqueParams.mvPrivaData.push_back(NULL);
    enqueParams.mvMagicNo.push_back(0);
    //input image
    IMEM_BUF_INFO imgiBuf;
    MINT32 bufBoundaryInBytes[3] = {0, 0, 0};
    IImageBuffer* srcBuffer;
    MUINT32 bufStridesInBytes[3] = {10560, 0, 0};
        imgiBuf.size=sizeof(g_imgi_5280x960_yuy2);
    mpImemDrv->allocVirtBuf(&imgiBuf);
        memcpy( (MUINT8*)(imgiBuf.virtAddr), (MUINT8*)(g_imgi_5280x960_yuy2), imgiBuf.size);
         //imem buffer 2 image heap
        PortBufInfo_v1 portBufInfo = PortBufInfo_v1( imgiBuf.memID,imgiBuf.virtAddr,0,imgiBuf.bufSecu, imgiBuf.bufCohe);
        IImageBufferAllocator::ImgParam imgParam = IImageBufferAllocator::ImgParam((eImgFmt_YUY2),MSize(5280, 960), bufStridesInBytes, bufBoundaryInBytes, 1);
        sp<ImageBufferHeap> pHeap = ImageBufferHeap::create( LOG_TAG, imgParam,portBufInfo,true);
    srcBuffer = pHeap->createImageBuffer();
        srcBuffer->incStrong(srcBuffer);
        srcBuffer->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
    Input src;
    src.mPortID=IMGI;
    src.mBuffer=srcBuffer;
    enqueParams.mvIn.push_back(src);

    IMEM_BUF_INFO vipiBuf;
    IImageBuffer* vipi_srcBuffer;
        vipiBuf.size=sizeof(g_vipi_5280x960_yuy2);
    mpImemDrv->allocVirtBuf(&vipiBuf);
        memcpy( (MUINT8*)(vipiBuf.virtAddr), (MUINT8*)(g_vipi_5280x960_yuy2), vipiBuf.size);
         //imem buffer 2 image heap
        PortBufInfo_v1 vipi_portBufInfo = PortBufInfo_v1( vipiBuf.memID,vipiBuf.virtAddr,0,vipiBuf.bufSecu, vipiBuf.bufCohe);
        IImageBufferAllocator::ImgParam vipi_imgParam = IImageBufferAllocator::ImgParam((eImgFmt_YUY2),MSize(5280, 960), bufStridesInBytes, bufBoundaryInBytes, 1);
        sp<ImageBufferHeap> vipi_pHeap = ImageBufferHeap::create( LOG_TAG, vipi_imgParam,vipi_portBufInfo,true);
    vipi_srcBuffer = vipi_pHeap->createImageBuffer();
        vipi_srcBuffer->incStrong(vipi_srcBuffer);
        vipi_srcBuffer->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
    Input vipi_src;
    vipi_src.mPortID=VIPI;
    vipi_src.mBuffer=vipi_srcBuffer;
    enqueParams.mvIn.push_back(vipi_src);
    //alpha maps
    IMEM_BUF_INFO vip3iBuf;
    IImageBuffer* vip3i_alpha1Buffer;
    MUINT32 vip3i_alphabufStridesInBytes[3] = {5280, 0, 0};
        vip3iBuf.size=sizeof(g_vip3i_5280x960_b8);
    mpImemDrv->allocVirtBuf(&vip3iBuf);
        memcpy( (MUINT8*)(vip3iBuf.virtAddr), (MUINT8*)(g_vip3i_5280x960_b8), vip3iBuf.size);
         //imem buffer 2 image heap
        PortBufInfo_v1 vip3i_alpha1_portBufInfo = PortBufInfo_v1( vip3iBuf.memID,vip3iBuf.virtAddr,0,vip3iBuf.bufSecu, vip3iBuf.bufCohe);
        IImageBufferAllocator::ImgParam vip3i_alpha1_imgParam = IImageBufferAllocator::ImgParam((eImgFmt_BAYER8),MSize(5280, 960), vip3i_alphabufStridesInBytes, bufBoundaryInBytes, 1);
        sp<ImageBufferHeap> vip3i_alpha1_pHeap = ImageBufferHeap::create( LOG_TAG, vip3i_alpha1_imgParam,vip3i_alpha1_portBufInfo,true);
    vip3i_alpha1Buffer = vip3i_alpha1_pHeap->createImageBuffer();
        vip3i_alpha1Buffer->incStrong(vip3i_alpha1Buffer);
        vip3i_alpha1Buffer->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
    Input vip3i_alpha1;
    vip3i_alpha1.mPortID=VIP3I;
    vip3i_alpha1.mBuffer=vip3i_alpha1Buffer;
    enqueParams.mvIn.push_back(vip3i_alpha1);

    IMEM_BUF_INFO ufdiBuf;
    IImageBuffer* ufdi_alpha1Buffer;
        ufdiBuf.size=sizeof(g_ufdi_5280x960_b8);
    mpImemDrv->allocVirtBuf(&ufdiBuf);
        memcpy( (MUINT8*)(ufdiBuf.virtAddr), (MUINT8*)(g_ufdi_5280x960_b8), ufdiBuf.size);
         //imem buffer 2 image heap
        PortBufInfo_v1 ufdi_alpha1_portBufInfo = PortBufInfo_v1( ufdiBuf.memID,ufdiBuf.virtAddr,0,ufdiBuf.bufSecu, ufdiBuf.bufCohe);
        IImageBufferAllocator::ImgParam ufdi_alpha1_imgParam = IImageBufferAllocator::ImgParam((eImgFmt_BAYER8),MSize(5280, 960), vip3i_alphabufStridesInBytes, bufBoundaryInBytes, 1);
        sp<ImageBufferHeap> ufdi_alpha1_pHeap = ImageBufferHeap::create( LOG_TAG, ufdi_alpha1_imgParam,ufdi_alpha1_portBufInfo,true);
    ufdi_alpha1Buffer = ufdi_alpha1_pHeap->createImageBuffer();
        ufdi_alpha1Buffer->incStrong(ufdi_alpha1Buffer);
        ufdi_alpha1Buffer->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
    Input ufdi_alpha1;
    ufdi_alpha1.mPortID=UFDI;
    ufdi_alpha1.mBuffer=ufdi_alpha1Buffer;
    enqueParams.mvIn.push_back(ufdi_alpha1);

    //crop information
    MCrpRsInfo crop;
    crop.mGroupID=1;
    MCrpRsInfo crop2;
    crop2.mGroupID=2;
    MCrpRsInfo crop3;
    crop3.mGroupID=3;


    //output buffer
    for(int i=0;i<5;i++)
    {
        //clear
        enqueParams.mvOut.clear();
        enqueParams.mvCropRsInfo.clear();

        //crop
        crop.mCropRect.p_fractional.x=0;
        crop.mCropRect.p_fractional.y=0;
        crop.mCropRect.p_integral.x=0;
        crop.mCropRect.p_integral.y=0;
        crop.mCropRect.s.w=5280;
        crop.mCropRect.s.h=960;
        crop.mResizeDst.w=5280;
        crop.mResizeDst.h=960;
        int wdma_out_w=0,wdma_out_h=0;
        int wrot_out_w=0,wrot_out_h=0;
        //different crop/same dst size
        if(i%2==0)
        {
            crop2.mCropRect.p_fractional.x=0;
            crop2.mCropRect.p_fractional.y=0;
            crop2.mCropRect.p_integral.x=0;
            crop2.mCropRect.p_integral.y=0;
            crop2.mCropRect.s.w=5280;
            crop2.mCropRect.s.h=960;
            crop2.mResizeDst.w=1280;
            crop2.mResizeDst.h=720;
            crop3.mCropRect.p_fractional.x=0;
            crop3.mCropRect.p_fractional.y=0;
            crop3.mCropRect.p_integral.x=280;
            crop3.mCropRect.p_integral.y=160;
            crop3.mCropRect.s.w=5000;
            crop3.mCropRect.s.h=800;
            crop3.mResizeDst.w=1920;
            crop3.mResizeDst.h=1080;
            wdma_out_w=1280;
            wdma_out_h=720;
            wrot_out_w=1920;
            wrot_out_h=1080;
        }
        else
        {
            crop2.mCropRect.p_fractional.x=0;
            crop2.mCropRect.p_fractional.y=0;
            crop2.mCropRect.p_integral.x=0;
            crop2.mCropRect.p_integral.y=0;
            crop2.mCropRect.s.w=5000;
            crop2.mCropRect.s.h=800;
            crop2.mResizeDst.w=1280;
            crop2.mResizeDst.h=720;
            crop3.mCropRect.p_fractional.x=0;
            crop3.mCropRect.p_fractional.y=0;
            crop3.mCropRect.p_integral.x=280;
            crop3.mCropRect.p_integral.y=160;
            crop3.mCropRect.s.w=5000;
            crop3.mCropRect.s.h=800;
            crop3.mResizeDst.w=960;
            crop3.mResizeDst.h=640;
            wdma_out_w=1280;
            wdma_out_h=720;
            wrot_out_w=960;
            wrot_out_h=640;
        }

        enqueParams.mvCropRsInfo.push_back(crop);
        enqueParams.mvCropRsInfo.push_back(crop2);
        enqueParams.mvCropRsInfo.push_back(crop3);

        //full size img3o
        IMEM_BUF_INFO img3oBuf;
        img3oBuf.size=5280*960*2;
            mpImemDrv->allocVirtBuf(&img3oBuf);
        memset((MUINT8*)img3oBuf.virtAddr, 0xffffffff, img3oBuf.size);
        MUINT32 bufStridesInBytes_1[3] = {5280,2640,2640};
        PortBufInfo_v1 portBufInfo_1 = PortBufInfo_v1( img3oBuf.memID,img3oBuf.virtAddr,0,img3oBuf.bufSecu, img3oBuf.bufCohe);
            IImageBufferAllocator::ImgParam imgParam_1 = IImageBufferAllocator::ImgParam((eImgFmt_YV12),
                                                            MSize(5280,960),  bufStridesInBytes_1, bufBoundaryInBytes, 3);
        sp<ImageBufferHeap> pHeap_1 = ImageBufferHeap::create( LOG_TAG, imgParam_1,portBufInfo_1,true);
        IImageBuffer* outBuffer = pHeap_1->createImageBuffer();
            outBuffer->incStrong(outBuffer);
            outBuffer->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
        Output dst;
        dst.mPortID=IMG3O;
        dst.mBuffer=outBuffer;
        dst.mPortID.group=0;
        enqueParams.mvOut.push_back(dst);
        //
        IMEM_BUF_INFO wdmaoBuf;
        IMEM_BUF_INFO wrotoBuf;
        wdmaoBuf.size=wdma_out_w*wdma_out_h*2;
            mpImemDrv->allocVirtBuf(&wdmaoBuf);
        memset((MUINT8*)wdmaoBuf.virtAddr, 0xffffffff, wdmaoBuf.size);
        MINT32 bufStridesInBytes_3[3] = {wdma_out_w, wdma_out_w/2, wdma_out_w/2};
        PortBufInfo_v1 portBufInfo_3 = PortBufInfo_v1( wdmaoBuf.memID,wdmaoBuf.virtAddr,0,wdmaoBuf.bufSecu, wdmaoBuf.bufCohe);
            IImageBufferAllocator::ImgParam imgParam_3 = IImageBufferAllocator::ImgParam((eImgFmt_YV12),
                                                            MSize(wdma_out_w, wdma_out_h),  bufStridesInBytes_3, bufBoundaryInBytes, 3);
        sp<ImageBufferHeap> pHeap_3 = ImageBufferHeap::create( LOG_TAG, imgParam_3,portBufInfo_3,true);
        IImageBuffer* outBuffer2 = pHeap_3->createImageBuffer();
            outBuffer2->incStrong(outBuffer2);
            outBuffer2->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
        Output dst2;
        dst2.mPortID=WDMAO;
        dst2.mBuffer=outBuffer2;
        dst2.mPortID.group=0;
        enqueParams.mvOut.push_back(dst2);
        //
        wrotoBuf.size=wrot_out_w*wrot_out_h*2;
            mpImemDrv->allocVirtBuf(&wrotoBuf);
        memset((MUINT8*)wrotoBuf.virtAddr, 0xffffffff, wrotoBuf.size);
        MINT32 bufStridesInBytes_4[3] = {wrot_out_w*2,0,0};
        PortBufInfo_v1 portBufInfo_4 = PortBufInfo_v1( wrotoBuf.memID,wrotoBuf.virtAddr,0,wrotoBuf.bufSecu, wrotoBuf.bufCohe);
            IImageBufferAllocator::ImgParam imgParam_4 = IImageBufferAllocator::ImgParam((eImgFmt_YUY2),
                                                            MSize(wrot_out_w, wrot_out_h),  bufStridesInBytes_4, bufBoundaryInBytes, 1);
        sp<ImageBufferHeap> pHeap_4 = ImageBufferHeap::create( LOG_TAG, imgParam_4,portBufInfo_4,true);
        IImageBuffer* outBuffer3 = pHeap_4->createImageBuffer();
            outBuffer3->incStrong(outBuffer3);
            outBuffer3->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
        Output dst3;
        dst3.mPortID=WROTO;
        dst3.mBuffer=outBuffer3;
        dst3.mPortID.group=0;
        enqueParams.mvOut.push_back(dst3);

        //buffer operation
        mpImemDrv->cacheFlushAll();
        printf("(%d) : enque\n",i);
        ret=pMFBMixStream->enque(enqueParams);
        if(!ret)
        {
            printf("(%d) : ERR enque fail\n",i);
        }
        else
        {
            printf("(%d) : enque done\n",i);
        }
        QParams dequeParams;
        ret=pMFBMixStream->deque(dequeParams);
        if(!ret)
        {
            printf("(%d) :  ERR deque fail\n",i);
        }
        else
        {
            printf("(%d) :  deque done\n",i);
        }

        //dump image
        char filename[256];
        sprintf(filename, "/sdcard/Pictures/P2UT_DiffViewAngle_case2_%d_img3o_%dx%d.yuv",i, 5280,960);
        saveBufToFile(filename, reinterpret_cast<MUINT8*>(dequeParams.mvOut[0].mBuffer->getBufVA(0)), 5280 *960 * 2);
            sprintf(filename, "/sdcard/Pictures/P2UT_DiffViewAngle_case2_%d_wdmao_%dx%d.yuv",i, wdma_out_w,wdma_out_h);
            saveBufToFile(filename, reinterpret_cast<MUINT8*>(dequeParams.mvOut[1].mBuffer->getBufVA(0)), wdma_out_w *wdma_out_h * 2);
            sprintf(filename, "/sdcard/Pictures/P2UT_DiffViewAngle_case2_%d_wroto_%dx%d.yuv",i, wrot_out_w,wrot_out_h);
            saveBufToFile(filename, reinterpret_cast<MUINT8*>(dequeParams.mvOut[2].mBuffer->getBufVA(0)), wrot_out_w *wrot_out_h * 2);

        printf("(%d) : --- p2a done ---\n", i);


        //unmap and free buffer
                outBuffer->unlockBuf(LOG_TAG);
                outBuffer2->unlockBuf(LOG_TAG);
                outBuffer3->unlockBuf(LOG_TAG);
                mpImemDrv->freeVirtBuf(&img3oBuf);
                mpImemDrv->freeVirtBuf(&wdmaoBuf);
                mpImemDrv->freeVirtBuf(&wrotoBuf);
    }
    //
    pMFBMixStream->uninit();
    pMFBMixStream->destroyInstance(LOG_TAG);
    mpImemDrv->uninit();
    mpImemDrv->destroyInstance();
    mpIspDrv->uninit(LOG_TAG);
        mpIspDrv->destroyInstance();
    return ret;
}
*/
/*********************************************************************************/

#include "pic/imgi_3264x1836_bayer10.h"

int basicVss(int type,int loopNum)
{
    
    int ret=0;
    //MBOOL isV3 = MFALSE;    //temp disable tuning path cuz tuning function not ready

    printf("--- [IP Based basicVss(%d,%d)...enterrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr]\n", type, loopNum);

    NSCam::NSIoPipe::NSPostProc::INormalStream* pCCStream;
    pCCStream= NSCam::NSIoPipe::NSPostProc::INormalStream::createInstance(0xFFFF);
    pCCStream->init("basicVss");
    printf("--- [basicVss(%d)...pStream init done]\n", type);
    IMemDrv* mpImemDrv=NULL;
    mpImemDrv=IMemDrv::createInstance();
    mpImemDrv->init();

    //
    QParams enqueParams;
    FrameParams frameParams;

    frameParams.mStreamTag = NSCam::NSIoPipe::NSPostProc::ENormalStreamTag_Vss;
    
    //input image
    MUINT32 _imgi_w_=3264, _imgi_h_=1836;
    IMEM_BUF_INFO buf_imgi;
    buf_imgi.size=sizeof(g_imgi_array_3264x1836_b10);
    mpImemDrv->allocVirtBuf(&buf_imgi);
    memcpy( (MUINT8*)(buf_imgi.virtAddr), (MUINT8*)(g_imgi_array_3264x1836_b10), buf_imgi.size);
    //imem buffer 2 image heap
    printf("--- [basicVss(%d)...flag -1 ]\n", type);
    IImageBuffer* srcBuffer;
    MINT32 bufBoundaryInBytes[3] = {0, 0, 0};
    MUINT32 bufStridesInBytes[3] = {(_imgi_w_*10/8) , 0, 0};
    PortBufInfo_v1 portBufInfo = PortBufInfo_v1( buf_imgi.memID,buf_imgi.virtAddr,0,buf_imgi.bufSecu, buf_imgi.bufCohe);
    IImageBufferAllocator::ImgParam imgParam = IImageBufferAllocator::ImgParam((eImgFmt_BAYER10),MSize(_imgi_w_, _imgi_h_), bufStridesInBytes, bufBoundaryInBytes, 1);
    sp<ImageBufferHeap> pHeap;
    pHeap = ImageBufferHeap::create( "basicVss", imgParam,portBufInfo,true);
    srcBuffer = pHeap->createImageBuffer();
    srcBuffer->incStrong(srcBuffer);
    srcBuffer->lockBuf("basicVss",eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
    printf("--- [basicVss(%d)...flag -8]\n", type);
    Input src;
    src.mPortID=PORT_IMGI;
    src.mBuffer=srcBuffer;
    src.mPortID.group=0;
    frameParams.mvIn.push_back(src);
    printf("--- [basicVss(%d)...push src done]\n", type);

   //crop information
    MCrpRsInfo crop;
    crop.mFrameGroup=0;
    crop.mGroupID=1;
    MCrpRsInfo crop2;
    crop2.mFrameGroup=0;
    crop2.mGroupID=2;
    MCrpRsInfo crop3;
    crop3.mFrameGroup=0;
    crop3.mGroupID=3;
    crop.mCropRect.p_fractional.x=0;
    crop.mCropRect.p_fractional.y=0;
    crop.mCropRect.p_integral.x=0;
    crop.mCropRect.p_integral.y=0;
    crop.mCropRect.s.w=_imgi_w_;
    crop.mCropRect.s.h=_imgi_h_;
    crop.mResizeDst.w=_imgi_w_;
    crop.mResizeDst.h=_imgi_h_;
    crop2.mCropRect.p_fractional.x=0;
    crop2.mCropRect.p_fractional.y=0;
    crop2.mCropRect.p_integral.x=0;
    crop2.mCropRect.p_integral.y=0;
    crop2.mCropRect.s.w=_imgi_w_;
    crop2.mCropRect.s.h=_imgi_h_;
    crop2.mResizeDst.w=_imgi_w_;
    crop2.mResizeDst.h=_imgi_h_;
    crop3.mCropRect.p_fractional.x=0;
    crop3.mCropRect.p_fractional.y=0;
    crop3.mCropRect.p_integral.x=0;
    crop3.mCropRect.p_integral.y=0;
    crop3.mCropRect.s.w=_imgi_w_;
    crop3.mCropRect.s.h=_imgi_h_;
    crop3.mResizeDst.w=_imgi_w_;
    crop3.mResizeDst.h=_imgi_h_;
    frameParams.mvCropRsInfo.push_back(crop);
    frameParams.mvCropRsInfo.push_back(crop2);
    frameParams.mvCropRsInfo.push_back(crop3);
    printf("--- [basicVss(%d)...push crop information done\n]", type);
    printf("--- [sizeof(isp_reg_t)(%x)..(%x)]\n", sizeof(isp_reg_t), (sizeof(isp_reg_t) - 0x4000));

    //output dma
    IMEM_BUF_INFO buf_wdmao;
    buf_wdmao.size=_imgi_w_*_imgi_h_*2;
    mpImemDrv->allocVirtBuf(&buf_wdmao);
    memset((MUINT8*)buf_wdmao.virtAddr, 0xffffffff, buf_wdmao.size);
    IImageBuffer* outBuffer=NULL;
    MUINT32 bufStridesInBytes_1[3] = {_imgi_w_*2,0,0};
    PortBufInfo_v1 portBufInfo_1 = PortBufInfo_v1( buf_wdmao.memID,buf_wdmao.virtAddr,0,buf_wdmao.bufSecu, buf_wdmao.bufCohe);
    IImageBufferAllocator::ImgParam imgParam_1 = IImageBufferAllocator::ImgParam((eImgFmt_YUY2),
                                            MSize(_imgi_w_,_imgi_h_),  bufStridesInBytes_1, bufBoundaryInBytes, 1);
    sp<ImageBufferHeap> pHeap_1 = ImageBufferHeap::create( "basicVss", imgParam_1,portBufInfo_1,true);
    outBuffer = pHeap_1->createImageBuffer();
    outBuffer->incStrong(outBuffer);
    outBuffer->lockBuf("basicVss",eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
    Output dst;
    dst.mPortID=PORT_WDMAO;
    dst.mBuffer=outBuffer;
    dst.mPortID.group=0;
    frameParams.mvOut.push_back(dst);

    IMEM_BUF_INFO buf_wroto;
    buf_wroto.size=_imgi_w_*_imgi_h_*2;
    mpImemDrv->allocVirtBuf(&buf_wroto);
    memset((MUINT8*)buf_wroto.virtAddr, 0xffffffff, buf_wroto.size);
    IImageBuffer* outBuffer_2=NULL;
    MUINT32 bufStridesInBytes_2[3] = {_imgi_w_*2,0,0};
    PortBufInfo_v1 portBufInfo_2 = PortBufInfo_v1( buf_wroto.memID,buf_wroto.virtAddr,0,buf_wroto.bufSecu, buf_wroto.bufCohe);
    IImageBufferAllocator::ImgParam imgParam_2 = IImageBufferAllocator::ImgParam((eImgFmt_YUY2),MSize(_imgi_w_,_imgi_h_),  bufStridesInBytes_2, bufBoundaryInBytes, 1);
    sp<ImageBufferHeap> pHeap_2 = ImageBufferHeap::create( "basicVss", imgParam_2,portBufInfo_2,true);
    outBuffer_2 = pHeap_2->createImageBuffer();
    outBuffer_2->incStrong(outBuffer_2);
    outBuffer_2->lockBuf("basicVss",eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
    Output dst_2;
    dst_2.mPortID=PORT_WROTO;
    dst_2.mBuffer=outBuffer_2;
    dst_2.mPortID.group=0;
    frameParams.mvOut.push_back(dst_2);

    isp_reg_t tuningDat;
    memcpy( (MUINT8*)(&tuningDat.CAM_CTL_START), (MUINT8*)(p2_vss_tuning_buffer), sizeof(p2_vss_tuning_buffer)/*(sizeof(isp_reg_t) - 0x4000)*/);
	frameParams.mTuningData = (MVOID*)&tuningDat;

    //frameParams.mTuningData = NULL;
    enqueParams.mvFrameParams.push_back(frameParams);

    printf("--- [basicVss(%d)...push dst done\n]", type);


    for(int i=0;i<loopNum;i++)
    {

        memset((MUINT8*)(frameParams.mvOut[0].mBuffer->getBufVA(0)), 0xffffffff, buf_wdmao.size);
        memset((MUINT8*)(frameParams.mvOut[1].mBuffer->getBufVA(0)), 0xffffffff, buf_wroto.size);

        //buffer operation
        mpImemDrv->cacheFlushAll();
        printf("--- [basicVss(%d_%d)...flush done\n]", type, i);


        //enque
        ret=pCCStream->enque(enqueParams);
        if(!ret)
        {
            printf("---ERRRRRRRRR [basicVss(%d_%d)..enque fail\n]", type, i);
        }
        else
        {
            printf("---[basicVss(%d_%d)..enque done\n]",type, i);
        }

        //temp use while to observe in CVD
        //printf("--- [basicVss(%d)...enter while...........\n]", type);
       //while(1);


        //deque
        //wait a momet in fpga
        //usleep(5000000);
        QParams dequeParams;
        ret=pCCStream->deque(dequeParams);
        if(!ret)
        {
            printf("---ERRRRRRRRR [basicVss(%d_%d)..deque fail\n]",type, i);
        }
        else
        {
            printf("---[basicVss(%d_%d)..deque done\n]", type, i);
        }


        //dump image
        char filename[256];
        sprintf(filename, "/data/P2UT_basicVss_case0_%d_%d_wdmao_%dx%d.yuv",type,i,  _imgi_w_,_imgi_h_);
        saveBufToFile(filename, reinterpret_cast<MUINT8*>(buf_wdmao.virtAddr), _imgi_w_ *_imgi_h_ * 2);
        char filename2[256];
        sprintf(filename2, "/data/P2UT_basicVss_case0_%d_%d_wroto_%dx%d.yuv",type,i,  _imgi_w_,_imgi_h_);
        saveBufToFile(filename2, reinterpret_cast<MUINT8*>(buf_wroto.virtAddr), _imgi_w_ *_imgi_h_ * 2);
            

        printf("--- [basicVss(%d_%d)...save file done\n]", type,i);
    }

    //free
    srcBuffer->unlockBuf("basicVss");
    mpImemDrv->freeVirtBuf(&buf_imgi);
    outBuffer->unlockBuf("basicVss");
    mpImemDrv->freeVirtBuf(&buf_wdmao);
    outBuffer_2->unlockBuf("basicVss");
    mpImemDrv->freeVirtBuf(&buf_wroto);
    printf("--- [basicVss(%d)...free memory done\n]", type);

    //
    pCCStream->uninit("basicVss");
    pCCStream->destroyInstance();
    mpImemDrv->uninit();
    mpImemDrv->destroyInstance();
    printf("--- [basicVss(%d)...pStream uninit done\n]", type);

    return ret;
}

// Secure camera UT
MVOID basicSecureVssCallback(QParams& rParams)
{
	//printf("--- [basicSecureP2A callback func]\n");

	g_basicSecureVssCallback = MTRUE;
}

int basicSecureVss(int type,int loopNum)
{
    int ret=0;
    printf("--- [IP Based basicSecureVss(%d,%d)...enterrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr]\n", type, loopNum);

    NSCam::NSIoPipe::NSPostProc::INormalStream* pCCStream;
    pCCStream= NSCam::NSIoPipe::NSPostProc::INormalStream::createInstance(0xFFFF);

    enum EDIPSecureEnum SecureTag = EDIPSecure_SECURE;

    pCCStream->init("basicSecureVss", SecureTag);
    printf("--- [basicSecureVss(%d)...pStream init done]\n", type);
    IMemDrv* mpImemDrv=NULL;
    mpImemDrv=IMemDrv::createInstance();
    mpImemDrv->init();

    //
    QParams enqueParams;
    FrameParams frameParams;

    frameParams.mStreamTag = NSCam::NSIoPipe::NSPostProc::ENormalStreamTag_Vss;
    frameParams.mSecureFra = ((SecureTag > EDIPSecure_NONE) && (SecureTag < EDIPSecure_MAX));

    // Allocate input secure image buffer
    //MUINT32 _imgi_w_=3264, _imgi_h_=1836;
    MUINT32 _imgi_w_=640, _imgi_h_=480;
    IImageBuffer* srcBuffer;
    MINT32 bufBoundaryInBytes[3] = {0, 0, 0};
    MUINT32 bufStridesInBytes[3] = {(_imgi_w_*10/8) , 0, 0};

    IImageBufferAllocator::ImgParam imgParam = IImageBufferAllocator::ImgParam((eImgFmt_BAYER10),MSize(_imgi_w_, _imgi_h_), bufStridesInBytes, bufBoundaryInBytes, 1);
    sp<ISecureImageBufferHeap> pSecHeap = NULL;
    pSecHeap = ISecureImageBufferHeap::create( "basicSecureVss", imgParam,
        ISecureImageBufferHeap::AllocExtraParam(0, 1, 0, MFALSE, SecType::mem_secure));
    if (pSecHeap == NULL) {
        printf("[basicSecureVss] Stuff ISecureImageBufferHeap create fail\n");
        return 0;
    }

    srcBuffer = pSecHeap->createImageBuffer();
    srcBuffer->incStrong(srcBuffer);
    srcBuffer->lockBuf("basicSecureVss",eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
    printf("--- [basicSecureVss(%d)...flag -8]\n", type);
    printf("ISecureImageBufferHeap: srcBuffer->getBufPA: 0x%x\n", srcBuffer->getBufPA(0));
    Input src;
    src.mPortID=PORT_IMGI;
    src.mBuffer=srcBuffer;
    src.mPortID.group=0;
    src.mSecureTag = EDIPSecure_SECURE;
    src.mSecHandle = 0;
    frameParams.mvIn.push_back(src);
    printf("--- [basicSecureVss(%d)...push src done]\n", type);

   //crop information
    MCrpRsInfo crop;
    crop.mFrameGroup=0;
    crop.mGroupID=1;
    MCrpRsInfo crop2;
    crop2.mFrameGroup=0;
    crop2.mGroupID=2;
    MCrpRsInfo crop3;
    crop3.mFrameGroup=0;
    crop3.mGroupID=3;
    crop.mCropRect.p_fractional.x=0;
    crop.mCropRect.p_fractional.y=0;
    crop.mCropRect.p_integral.x=0;
    crop.mCropRect.p_integral.y=0;
    crop.mCropRect.s.w=_imgi_w_;
    crop.mCropRect.s.h=_imgi_h_;
    crop.mResizeDst.w=_imgi_w_;
    crop.mResizeDst.h=_imgi_h_;
    crop2.mCropRect.p_fractional.x=0;
    crop2.mCropRect.p_fractional.y=0;
    crop2.mCropRect.p_integral.x=0;
    crop2.mCropRect.p_integral.y=0;
    crop2.mCropRect.s.w=_imgi_w_;
    crop2.mCropRect.s.h=_imgi_h_;
    crop2.mResizeDst.w=_imgi_w_;
    crop2.mResizeDst.h=_imgi_h_;
    crop3.mCropRect.p_fractional.x=0;
    crop3.mCropRect.p_fractional.y=0;
    crop3.mCropRect.p_integral.x=0;
    crop3.mCropRect.p_integral.y=0;
    crop3.mCropRect.s.w=_imgi_w_;
    crop3.mCropRect.s.h=_imgi_h_;
    crop3.mResizeDst.w=_imgi_w_;
    crop3.mResizeDst.h=_imgi_h_;
    frameParams.mvCropRsInfo.push_back(crop);
    frameParams.mvCropRsInfo.push_back(crop2);
    frameParams.mvCropRsInfo.push_back(crop3);
    printf("--- [basicSecureVss(%d)...push crop information done\n]", type);
    printf("--- [sizeof(isp_reg_t)(%x)..(%x)]\n", sizeof(isp_reg_t), (sizeof(isp_reg_t) - 0x4000));

    // Allocate 1st. output secure image buffer
    IImageBuffer* outBuffer=NULL;
    MUINT32 bufStridesInBytes_1[3] = {_imgi_w_*2, 0, 0};
    IImageBufferAllocator::ImgParam imgParam_1 = IImageBufferAllocator::ImgParam((eImgFmt_YUY2),
                                            MSize(_imgi_w_,_imgi_h_), bufStridesInBytes_1, bufBoundaryInBytes, 1);
    sp<ISecureImageBufferHeap> pHeap_1;
    switch (type) {
        /*
            type 0: isp direct link, "wdma"(secure),      wrot(secure)
            type 1: isp direct link, "wdma"(secure) ,     wrot(non-secure)
            type 2: isp direct link, "wdma"(non-secure) , wrot(secure)
        */
        case 2:
            pHeap_1 = ISecureImageBufferHeap::create( "basicSecureP2A", imgParam_1,
            ISecureImageBufferHeap::AllocExtraParam(0, 0, 0, MFALSE, SecType::mem_normal));
            break;

        case 0:
        case 1:
        default:
            pHeap_1 = ISecureImageBufferHeap::create( "basicSecureP2A", imgParam_1,
            ISecureImageBufferHeap::AllocExtraParam(0, 1, 0, MFALSE, SecType::mem_secure));
        break;
    };

    if (pHeap_1 == NULL) {
        printf("[basicSecureP2A] Stuff ImageBufferHeap create fail\n");
        return 0;
    }

    outBuffer = pHeap_1->createImageBuffer();
    outBuffer->incStrong(outBuffer);
    outBuffer->lockBuf("basicSecureVss",eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
    Output dst;
    dst.mBuffer=outBuffer;
    dst.mPortID.group=0;
    switch (type) {
        /*
            type 0: isp direct link, "wdma"(secure),      wrot(secure)
            type 1: isp direct link, "wdma"(secure) ,     wrot(non-secure)
            type 2: isp direct link, "wdma"(non-secure) , wrot(secure)
        */
        case 2:
            dst.mPortID=PORT_WDMAO;
            dst.mSecureTag = EDIPSecure_NONE;
            break;

        case 0:
        case 1:
        default:
            dst.mPortID=PORT_WDMAO;
            dst.mSecureTag = EDIPSecure_SECURE;
        break;
    };
    frameParams.mvOut.push_back(dst);

    // Allocate 2nd. output secure image buffer
    IImageBuffer* outBuffer_2 = NULL;
    MUINT32 bufStridesInBytes_2[3] = {_imgi_w_*2, 0, 0};
    IImageBufferAllocator::ImgParam imgParam_2 = IImageBufferAllocator::ImgParam((eImgFmt_YUY2),MSize(_imgi_w_,_imgi_h_),  bufStridesInBytes_2, bufBoundaryInBytes, 1);
    sp<ISecureImageBufferHeap> pHeap_2;
    switch (type) {
    /*
        type 0: isp direct link, wdma(secure),      "wrot"(secure)
        type 1: isp direct link, wdma(secure) ,     "wrot"(non-secure)
        type 2: isp direct link, wdma(non-secure) , "wrot"(secure)
    */
    case 1:
        pHeap_2 = ISecureImageBufferHeap::create( "basicSecureP2A", imgParam_2,
        ISecureImageBufferHeap::AllocExtraParam(0, 0, 0, MFALSE, SecType::mem_normal));
        break;

    case 0:
    case 2:
    default:
        pHeap_2 = ISecureImageBufferHeap::create( "basicSecureP2A", imgParam_2,
        ISecureImageBufferHeap::AllocExtraParam(0, 1, 0, MFALSE, SecType::mem_secure));
    break;
    };

    if (pHeap_2 == NULL) {
        printf("[basicSecureP2A] Stuff ImageBufferHeap create fail\n");
        return 0;
    }
    outBuffer_2 = pHeap_2->createImageBuffer();
    outBuffer_2->incStrong(outBuffer_2);
    outBuffer_2->lockBuf("basicSecureVss",eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
    Output dst_2;
    dst_2.mBuffer=outBuffer_2;
    dst_2.mPortID.group=0;
    switch (type) {
        /*
            type 0: isp direct link, "wdma"(secure),      wrot(secure)
            type 1: isp direct link, "wdma"(secure) ,     wrot(non-secure)
            type 2: isp direct link, "wdma"(non-secure) , wrot(secure)
        */
        case 1:
            dst_2.mPortID=PORT_WROTO;
            dst_2.mSecureTag = EDIPSecure_NONE;
            break;

        case 0:
        case 2:
        default:
            dst_2.mPortID=PORT_WROTO;
            dst_2.mSecureTag = EDIPSecure_SECURE;
        break;
    };
    frameParams.mvOut.push_back(dst_2);

    isp_reg_t tuningDat;
    memcpy( (MUINT8*)(&tuningDat.CAM_CTL_START), (MUINT8*)(p2_vss_tuning_buffer), sizeof(p2_vss_tuning_buffer)/*(sizeof(isp_reg_t) - 0x4000)*/);
	frameParams.mTuningData = (MVOID*)&tuningDat;

    //frameParams.mTuningData = NULL;
    enqueParams.mvFrameParams.push_back(frameParams);

    printf("--- [basicSecureVss(%d)...push dst done\n]", type);

    g_basicSecureVssCallback = MFALSE;
    enqueParams.mpfnCallback = basicSecureVssCallback;
    mpImemDrv->initSecureM4U();

	int waitcnt = 30;
    for(int i=0; i < loopNum; i++)
    {
        //enque
        ret = pCCStream->enque(enqueParams);
        if(!ret)
        {
            printf("---ERRRRRRRRR [basicSecureVss(%d_%d)..enque fail\n]", type, i);
        }
        else
        {
            printf("---[basicSecureVss(%d_%d)..enque done\n]",type, i);
            while(!g_basicSecureVssCallback && waitcnt--)
				usleep(100000);

	        g_basicSecureVssCallback = MFALSE;
	        printf("--- [basicSecureVss(%d_%d)...deque done\n]", type, i);
        }
    }

    //free
    srcBuffer->unlockBuf("basicSecureVss");
    outBuffer->unlockBuf("basicSecureVss");
    outBuffer_2->unlockBuf("basicSecureVss");
    printf("--- [basicSecureVss(%d)...free memory done\n]", type);

    //
    pCCStream->uninit("basicSecureVss");
    pCCStream->destroyInstance();
    mpImemDrv->uninit();
    mpImemDrv->destroyInstance();
    printf("--- [basicSecureVss(%d)...pCCStream uninit done\n]", type);

    return ret;
}

/*********************************************************************************/
/*
#include "pic/imgi_640_480_10.h"

int P2A_FG()
{
    
    int ret=0;
    MBOOL isV3 = MFALSE;    //temp disable tuning path cuz tuning function not ready

    NSCam::NSIoPipe::NSPostProc::INormalStream* pCCStream;
    pCCStream= NSCam::NSIoPipe::NSPostProc::INormalStream::createInstance(LOG_TAG, NSCam::NSIoPipe::NSPostProc::ENormalStreamTag_Prv,0xFFFF, isV3);
    pCCStream->init();
    IMemDrv* mpImemDrv=NULL;
    mpImemDrv=IMemDrv::createInstance();
    mpImemDrv->init();
    IspDrv* mpIspDrv=NULL;
        mpIspDrv=IspDrv::createInstance();
        mpIspDrv->init(LOG_TAG);

    //
    QParams enqueParams;
    enqueParams.mvPrivaData.push_back(NULL);
    enqueParams.mvMagicNo.push_back(0);
    
    //input image
    MUINT32 _imgi_w_=640, _imgi_h_=480;
    IMEM_BUF_INFO buf_imgi;
    buf_imgi.size=sizeof(p2a_fg_g_imgi_array_640_480_10);
    mpImemDrv->allocVirtBuf(&buf_imgi);
    memcpy( (MUINT8*)(buf_imgi.virtAddr), (MUINT8*)(p2a_fg_g_imgi_array_640_480_10), buf_imgi.size);
    //imem buffer 2 image heap
    printf("--- [P2A_FG...flag -1 ]\n");
    IImageBuffer* srcBuffer;
    MINT32 bufBoundaryInBytes[3] = {0, 0, 0};
    MUINT32 bufStridesInBytes[3] = {(_imgi_w_*10/8)* 3 / 2 , 0, 0};
    PortBufInfo_v1 portBufInfo = PortBufInfo_v1( buf_imgi.memID,buf_imgi.virtAddr,0,buf_imgi.bufSecu, buf_imgi.bufCohe);
    IImageBufferAllocator::ImgParam imgParam = IImageBufferAllocator::ImgParam((eImgFmt_FG_BAYER10),MSize(_imgi_w_, _imgi_h_), bufStridesInBytes, bufBoundaryInBytes, 1);
    sp<ImageBufferHeap> pHeap;
    pHeap = ImageBufferHeap::create( LOG_TAG, imgParam,portBufInfo,true);
    srcBuffer = pHeap->createImageBuffer();
    srcBuffer->incStrong(srcBuffer);
    srcBuffer->lockBuf("P2A_FG",eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
    printf("--- [P2A_FG...flag -8]\n");
    Input src;
    src.mPortID=IMGI;
    src.mBuffer=srcBuffer;
    enqueParams.mvIn.push_back(src);
    printf("--- [P2A_FG...push src done]\n");

   //crop information    
    MCrpRsInfo crop;
    crop.mGroupID=1;
    MCrpRsInfo crop2;
    crop2.mGroupID=2;
    MCrpRsInfo crop3;
    crop3.mGroupID=3;
    crop.mCropRect.p_fractional.x=0;
    crop.mCropRect.p_fractional.y=0;
    crop.mCropRect.p_integral.x=0;
    crop.mCropRect.p_integral.y=0;
    crop.mCropRect.s.w=_imgi_w_;
    crop.mCropRect.s.h=_imgi_h_;
    crop.mResizeDst.w=_imgi_w_;
    crop.mResizeDst.h=_imgi_h_;
    crop2.mCropRect.p_fractional.x=0;
    crop2.mCropRect.p_fractional.y=0;
    crop2.mCropRect.p_integral.x=0;
    crop2.mCropRect.p_integral.y=0;
    crop2.mCropRect.s.w=_imgi_w_;
    crop2.mCropRect.s.h=_imgi_h_;
    crop2.mResizeDst.w=_imgi_w_;
    crop2.mResizeDst.h=_imgi_h_;
    crop3.mCropRect.p_fractional.x=0;
    crop3.mCropRect.p_fractional.y=0;
    crop3.mCropRect.p_integral.x=0;
    crop3.mCropRect.p_integral.y=0;
    crop3.mCropRect.s.w=_imgi_w_;
    crop3.mCropRect.s.h=_imgi_h_;
    crop3.mResizeDst.w=_imgi_w_;
    crop3.mResizeDst.h=_imgi_h_;
    enqueParams.mvCropRsInfo.push_back(crop);
    enqueParams.mvCropRsInfo.push_back(crop2);
    enqueParams.mvCropRsInfo.push_back(crop3);

    printf("--- [P2A_FG..push crop information done\n]");

    //output dma
    IMEM_BUF_INFO img2oBuf;
    IImageBuffer* outBuffer=NULL;
   //simulate fd buffer from img2o
    img2oBuf.size=_imgi_w_*_imgi_h_*2;
    mpImemDrv->allocVirtBuf(&img2oBuf);
    memset((MUINT8*)img2oBuf.virtAddr, 0x0, img2oBuf.size);
    MUINT32 bufStridesInBytes_1[3] = {_imgi_w_*2,0,0};
    PortBufInfo_v1 portBufInfo_1 = PortBufInfo_v1( img2oBuf.memID,img2oBuf.virtAddr,0,img2oBuf.bufSecu, img2oBuf.bufCohe);
    IImageBufferAllocator::ImgParam imgParam_1 = IImageBufferAllocator::ImgParam((eImgFmt_YUY2),
                                                  MSize(_imgi_w_,_imgi_h_),  bufStridesInBytes_1, bufBoundaryInBytes, 1);
    sp<ImageBufferHeap> pHeap_1 = ImageBufferHeap::create( LOG_TAG, imgParam_1,portBufInfo_1,true);
    outBuffer = pHeap_1->createImageBuffer();
    outBuffer->incStrong(outBuffer);
    outBuffer->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);

    Output dst;
    dst.mPortID=IMG2O;
    dst.mBuffer=outBuffer;
    dst.mPortID.group=0;
    enqueParams.mvOut.push_back(dst);


    printf("--- [P2A_FG...push dst done\n]");


   // for(int i=0;i<testNum;i++)
    {


        //buffer operation
        mpImemDrv->cacheFlushAll();
        printf("--- [P2A_FG...flush done\n]");


        //enque
        ret=pCCStream->enque(enqueParams);
        if(!ret)
        {
            printf("---ERRRRRRRRR [P2A_FG..enque fail\n]");
        }
        else
        {
            printf("---[P2A_FG..enque done\n]");
        }

        QParams dequeParams;
        ret=pCCStream->deque(dequeParams);
        if(!ret)
        {
            printf("---ERRRRRRRRR [P2A_FG..deque fail\n]");
        }
        else
        {
            printf("---[P2A_FG..deque done\n]");
        }


        //dump image
        char filename[256];
        sprintf(filename, "/vendor/bin/P2UT_P2A_FG_case_wdmao_%dx%d.yuv",  _imgi_w_,_imgi_h_);
        saveBufToFile(filename, reinterpret_cast<MUINT8*>(img2oBuf.virtAddr), _imgi_w_ *_imgi_h_ * 2);
 
            
        printf("--- [P2A_FG...save file done\n]");
    }

    //free
    srcBuffer->unlockBuf("LOG_TAG");
    mpImemDrv->freeVirtBuf(&buf_imgi);
    outBuffer->unlockBuf("LOG_TAG");
    mpImemDrv->freeVirtBuf(&img2oBuf);
    printf("--- [P2A_FG...free memory done\n]");

    //
    pCCStream->uninit();
    pCCStream->destroyInstance(LOG_TAG);
    mpImemDrv->uninit();
    mpImemDrv->destroyInstance();
    mpIspDrv->uninit(LOG_TAG);
    mpIspDrv->destroyInstance();
    printf("--- [P2A_FG...pStream uninit done\n]");

    return ret;
}
*/

/*******************************************************************************
* [image width limitation w/o tpipe is 768]
* type0 : [ip-raw path][path test] imgi in/wdmao & wroto out, tile mode, 3264x1836 (input format: bayer 10)
* type1 : [ip-raw path][path test] imgi in/wdmao & wroto out, tile mode, 640x480 (input format: yuy2)
********************************************************************************/
int Basic_ip_raw_with_tile(int type)
{
    int ret=0;
    printf("--- [ip_raw_with_tile(%d)...enterrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr]\n", type);
    //MBOOL isV3 = MFALSE;    //temp disable tuning path cuz tuning function not ready
    #define P2_TUNINGQUE_NUM    10
    MUINT32 size;
    void *pTuningQueBuf[P2_TUNINGQUE_NUM];
    isp_reg_t *pIspPhyReg[P2_TUNINGQUE_NUM];
    //MRect p1SrcCrop;
    //MSize p1Dst;
    //MRect p1DstCrop;

    //sem_init(&mSem, 0, 0);
    //sem_init(&mSem_ThreadEnd, 0, 0);

    /*
    switch(type)
    {
        case 0:
            isV3 = MFALSE;
        break;
        case 1:
            isV3 = MTRUE;
        break;
    }
    */
    NSCam::NSIoPipe::NSPostProc::INormalStream* pCCStream;
    pCCStream= NSCam::NSIoPipe::NSPostProc::INormalStream::createInstance(0xFFFF);
    pCCStream->init("ipraw");
    printf("--- [ip_raw_with_tile(%d)...pStream init done]\n", type);
    IMemDrv* mpImemDrv=NULL;
    mpImemDrv=IMemDrv::createInstance();
    mpImemDrv->init();
    IspDrv* mpIspDrv=NULL;
    mpIspDrv=IspDrv::createInstance();
    mpIspDrv->init("ipraw");

    QParams enqueParams;
    FrameParams frameParams;

    //frame tag
    frameParams.mStreamTag = NSCam::NSIoPipe::NSPostProc::ENormalStreamTag_IP_Tpipe;

    /*
    if(isV3)
    {
        //p1 private data
        p1SrcCrop.s.w = 640;
        p1SrcCrop.s.h = 480;
        p1Dst.w = 640;
        p1Dst.h = 480;
        p1DstCrop.p.x = 0;
        p1DstCrop.p.y = 0;
        enqueParams.mvP1SrcCrop.push_back(p1SrcCrop);
        enqueParams.mvP1Dst.push_back(p1Dst);
        enqueParams.mvP1DstCrop.push_back(p1DstCrop);
    }
    */

    ///////
    //create input
    IMEM_BUF_INFO imgiBuf;

    MINT32 bufBoundaryInBytes[3] = {0, 0, 0};
    IImageBuffer* srcBuffer;
    MUINT32 _imgi_w_,_imgi_h_;

    //imgi in/wdma & wroto out
    _imgi_w_=3264;
    _imgi_h_=1836;
    MUINT32 bufStridesInBytes[3] = {(_imgi_w_*10/8) , 0, 0};
    imgiBuf.size=sizeof(g_imgi_array_3264x1836_b10);
    mpImemDrv->allocVirtBuf(&imgiBuf);
    memcpy( (MUINT8*)(imgiBuf.virtAddr), (MUINT8*)(g_imgi_array_3264x1836_b10), imgiBuf.size);
    printf("--- [ip_raw_with_tile(%d)...flag -1 ]\n", type);
    //imem buffer 2 image heap
    PortBufInfo_v1 portBufInfo = PortBufInfo_v1( imgiBuf.memID,imgiBuf.virtAddr,0,imgiBuf.bufSecu, imgiBuf.bufCohe);
    IImageBufferAllocator::ImgParam imgParam = IImageBufferAllocator::ImgParam((eImgFmt_BAYER10),MSize(_imgi_w_, _imgi_h_), bufStridesInBytes, bufBoundaryInBytes, 1);
    sp<ImageBufferHeap> pHeap = ImageBufferHeap::create( "ipraw", imgParam,portBufInfo,true);
    srcBuffer = pHeap->createImageBuffer();
    srcBuffer->incStrong(srcBuffer);
    srcBuffer->lockBuf("ipraw",eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
    printf("--- [ip_raw_with_tile(%d)...flag -8]\n", type);

    //
    Input src;
    src.mPortID=PORT_IMGI;
    src.mBuffer=srcBuffer;
    src.mPortID.group=0;
    frameParams.mvIn.push_back(src);
    printf("--- [ip_raw_with_tile(%d)...push src done]\n", type);

    //crop information
    MCrpRsInfo crop;
    crop.mFrameGroup=0;
    crop.mGroupID=1;
    MCrpRsInfo crop2;
    crop2.mFrameGroup=0;
    crop2.mGroupID=2;
    MCrpRsInfo crop3;
    crop3.mFrameGroup=0;
    crop3.mGroupID=3;
    crop.mCropRect.p_fractional.x=0;
    crop.mCropRect.p_fractional.y=0;
    crop.mCropRect.p_integral.x=0;
    crop.mCropRect.p_integral.y=0;
    crop.mCropRect.s.w=_imgi_w_;
    crop.mCropRect.s.h=_imgi_h_;
    crop.mResizeDst.w=_imgi_w_;
    crop.mResizeDst.h=_imgi_h_;
    crop2.mCropRect.p_fractional.x=0;
    crop2.mCropRect.p_fractional.y=0;
    crop2.mCropRect.p_integral.x=0;
    crop2.mCropRect.p_integral.y=0;
    crop2.mCropRect.s.w=_imgi_w_;
    crop2.mCropRect.s.h=_imgi_h_;
    crop2.mResizeDst.w=_imgi_w_;
    crop2.mResizeDst.h=_imgi_h_;
    crop3.mCropRect.p_fractional.x=0;
    crop3.mCropRect.p_fractional.y=0;
    crop3.mCropRect.p_integral.x=0;
    crop3.mCropRect.p_integral.y=0;
    crop3.mCropRect.s.w=_imgi_w_;
    crop3.mCropRect.s.h=_imgi_h_;
    crop3.mResizeDst.w=_imgi_w_;
    crop3.mResizeDst.h=_imgi_h_;
    frameParams.mvCropRsInfo.push_back(crop);
    frameParams.mvCropRsInfo.push_back(crop2);
    frameParams.mvCropRsInfo.push_back(crop3);
    printf("--- [ip_raw_with_tile(%d)...push crop information done\n]", type);
    printf("--- [sizeof(isp_reg_t)(%x)..(%x)]\n", sizeof(isp_reg_t), (sizeof(isp_reg_t) - 0x4000));

    /////////
    //output buffer
    IMEM_BUF_INFO wdmaoBuf;
    IMEM_BUF_INFO wrotoBuf;

    wdmaoBuf.size=_imgi_w_*_imgi_h_*2; //output pack 10
    mpImemDrv->allocVirtBuf(&wdmaoBuf);
    memset((MUINT8*)wdmaoBuf.virtAddr, 0xffffffff, wdmaoBuf.size);
    IImageBuffer* outBuffer=NULL;
    MUINT32 bufStridesInBytes_1[3] = {_imgi_w_*2, 0, 0};
    PortBufInfo_v1 portBufInfo_1 = PortBufInfo_v1( wdmaoBuf.memID,wdmaoBuf.virtAddr,0,wdmaoBuf.bufSecu, wdmaoBuf.bufCohe);
    IImageBufferAllocator::ImgParam imgParam_1 = IImageBufferAllocator::ImgParam((eImgFmt_YUY2),MSize(_imgi_w_, _imgi_h_),  bufStridesInBytes_1, bufBoundaryInBytes, 1);
    sp<ImageBufferHeap> pHeap_1 = ImageBufferHeap::create( "ipraw", imgParam_1,portBufInfo_1,true);
    outBuffer = pHeap_1->createImageBuffer();
    outBuffer->incStrong(outBuffer);
    outBuffer->lockBuf("ipraw",eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
    Output dst;
    dst.mPortID=PORT_WDMAO;
    dst.mBuffer=outBuffer;
    dst.mPortID.group=0;
    frameParams.mvOut.push_back(dst);

    wrotoBuf.size=_imgi_w_*_imgi_h_*2;
    mpImemDrv->allocVirtBuf(&wrotoBuf);
    memset((MUINT8*)wrotoBuf.virtAddr, 0xffffffff, wrotoBuf.size);
    IImageBuffer* outBuffer_2=NULL;
    MUINT32 bufStridesInBytes_2[3] = {_imgi_w_*2,0,0};
    PortBufInfo_v1 portBufInfo_2 = PortBufInfo_v1( wrotoBuf.memID,wrotoBuf.virtAddr,0,wrotoBuf.bufSecu, wrotoBuf.bufCohe);
    IImageBufferAllocator::ImgParam imgParam_2 = IImageBufferAllocator::ImgParam((eImgFmt_YUY2),MSize(_imgi_w_,_imgi_h_),  bufStridesInBytes_2, bufBoundaryInBytes, 1);
    sp<ImageBufferHeap> pHeap_2 = ImageBufferHeap::create( "ipraw", imgParam_2,portBufInfo_2,true);
    outBuffer_2 = pHeap_2->createImageBuffer();
    outBuffer_2->incStrong(outBuffer_2);
    outBuffer_2->lockBuf("ipraw",eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
    Output dst_2;
    dst_2.mPortID=PORT_WROTO;
    dst_2.mBuffer=outBuffer_2;
    dst_2.mPortID.group=0;
    frameParams.mvOut.push_back(dst_2);

    printf("--- [ip_raw_with_tile(%d)...push dst done\n]", type);


    /////
    //temp to add, (original done in pass1)
    MUINT32 reg_val=0x0;
    //ISP_WRITE_REG(mpIspDrv,CAM_CTL_SEL, 0, ISP_DRV_USER_ISPF);
    ISP_WRITE_REG(mpIspDrv,CAM_CTL_EN_P1, 0x40000000, ISP_DRV_USER_ISPF);
    reg_val = ISP_READ_REG_NOPROTECT(mpIspDrv,CAM_CTL_EN_P2);
    ISP_WRITE_REG(mpIspDrv,CAM_CTL_EN_P2, reg_val&0x04000000, ISP_DRV_USER_ISPF);//FMT_EN don't touch
    ISP_WRITE_REG(mpIspDrv,CAM_CTL_EN_P2_DMA, 0, ISP_DRV_USER_ISPF);
    ISP_WRITE_REG(mpIspDrv,CAM_CTL_EN_P1_DMA, 0, ISP_DRV_USER_ISPF);
    //disable GGM
    ISP_WRITE_REG(mpIspDrv,CAM_GGM_CTRL, 0, ISP_DRV_USER_ISPF);
    //reset GDMA relative setting
    //ISP_WRITE_REG(mpIspDrv,CAM_CTL_SEL, 0, ISP_DRV_USER_ISPF);
    //interrupt merge
    //ISP_WRITE_BITS(mpIspDrv,CAM_CTL_SPARE3,INT_MRG, 1, ISP_DRV_USER_ISPF);
    /////
    printf("QQ : ISP_READ_REG_NOPROTECT \n");

    isp_reg_t tuningDat;
    memcpy( (MUINT8*)(&tuningDat.CAM_CTL_START), (MUINT8*)(p2_ip_raw_tpipe_tuning_buffer), sizeof(p2_ip_raw_tpipe_tuning_buffer)/*(sizeof(isp_reg_t) - 0x4000)*/);
	frameParams.mTuningData = (MVOID*)&tuningDat;

	//frameParams.mTuningData = NULL;
    enqueParams.mvFrameParams.push_back(frameParams);

    memset((MUINT8*)(frameParams.mvOut[0].mBuffer->getBufVA(0)), 0xffffffff, wdmaoBuf.size);
    memset((MUINT8*)(frameParams.mvOut[1].mBuffer->getBufVA(0)), 0xffffffff, wrotoBuf.size);

    //flush first
    mpImemDrv->cacheFlushAll();
    printf("--- [ip_raw_with_tile(%d)...flush done\n]", type);


    //enque
    ret = pCCStream->enque(enqueParams);
    if(!ret)
    {
        printf("---ERRRRRRRRR [ip_raw_with_tile(%d)..enque fail\n]", type);
    }
    else
    {
        printf("---[ip_raw_with_tile(%d)..enque done\n]",type);
    }

    ////
    //deque
    QParams dequeParams;
    ret = pCCStream->deque(dequeParams);
    if(!ret)
    {
        printf("---ERRRRRRRRR [ip_raw_with_tile(%d)..deque fail\n]",type);
    }
    else
    {
        printf("---[ip_raw_with_tile(%d)..deque done\n]", type);
    }

    //dump image
    char filename[256];
    sprintf(filename, "/data/ip_raw_with_tile_type%d_wdmao_%dx%d.yuv",type,_imgi_w_,_imgi_h_);
    saveBufToFile(filename, reinterpret_cast<MUINT8*>(dequeParams.mvFrameParams[0].mvOut[0].mBuffer->getBufVA(0)), _imgi_w_ *_imgi_h_ * 2);
    char filename2[256];
    sprintf(filename2, "/data/ip_raw_with_tile_type%d_wroto_%dx%d.yuv",type,_imgi_w_,_imgi_h_);
    saveBufToFile(filename2, reinterpret_cast<MUINT8*>(dequeParams.mvFrameParams[0].mvOut[1].mBuffer->getBufVA(0)), _imgi_w_ *_imgi_h_ * 2);
    printf("--- [ip_raw_with_tile(%d)...save file done\n]", type);

    ////
    //temp enter while to attach code viser
    //LOG_INF("ENTER WHILE");
    //while(1);
    //

    //free
    srcBuffer->unlockBuf("ipraw");
    mpImemDrv->freeVirtBuf(&imgiBuf);
    outBuffer->unlockBuf("ipraw");
    mpImemDrv->freeVirtBuf(&wdmaoBuf);
    outBuffer_2->unlockBuf("ipraw");
    mpImemDrv->freeVirtBuf(&wrotoBuf);
    printf("--- [ip_raw_with_tile(%d)...free memory done\n]", type);

    pCCStream->uninit("ipraw");
    pCCStream->destroyInstance();
	printf("--- [ip_raw_with_tile(%d)...pStream uninit done\n]", type);
    mpImemDrv->uninit();
    mpImemDrv->destroyInstance();
    mpIspDrv->uninit("ipraw");
    mpIspDrv->destroyInstance();
    printf("TEST CASE DONE");
    return ret;
}

int Basic_ip_raw()
{
    int ret=0;
    printf("--- [ip_raw...enterrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr]\n");

    NSCam::NSIoPipe::NSPostProc::INormalStream* pCCStream;
    pCCStream= NSCam::NSIoPipe::NSPostProc::INormalStream::createInstance(0xFFFF);
    pCCStream->init("ipraw");
    printf("--- [ip_raw...pStream init done]\n");
    IMemDrv* mpImemDrv=NULL;
    mpImemDrv=IMemDrv::createInstance();
    mpImemDrv->init();
    IspDrv* mpIspDrv=NULL;
    mpIspDrv=IspDrv::createInstance();
    mpIspDrv->init("ipraw");

    QParams enqueParams;
    FrameParams frameParams;

    // we don't need to assgin streamtag due to NormalStreamUpper will do this for us on IP raw
    frameParams.mStreamTag = NSCam::NSIoPipe::NSPostProc::ENormalStreamTag_IP_Raw;

    ///////
    //create input
    IMEM_BUF_INFO imgiBuf;

    MINT32 bufBoundaryInBytes[3] = {0, 0, 0};
    IImageBuffer* srcBuffer;
    MUINT32 _imgi_w_,_imgi_h_;

    //imgi in/wdma & wroto out
    _imgi_w_= 640;
    _imgi_h_= 480;
    MUINT32 bufStridesInBytes[3] = {(_imgi_w_*10/8) , 0, 0};
    imgiBuf.size=sizeof(g_imgi_array_640x480_b10);
    mpImemDrv->allocVirtBuf(&imgiBuf);
    memcpy( (MUINT8*)(imgiBuf.virtAddr), (MUINT8*)(g_imgi_array_640x480_b10), imgiBuf.size);
    printf("--- [ip_raw...flag -1 ]\n");
    //imem buffer 2 image heap
    PortBufInfo_v1 portBufInfo = PortBufInfo_v1( imgiBuf.memID,imgiBuf.virtAddr,0,imgiBuf.bufSecu, imgiBuf.bufCohe);
    IImageBufferAllocator::ImgParam imgParam = IImageBufferAllocator::ImgParam((eImgFmt_BAYER10),MSize(_imgi_w_, _imgi_h_), bufStridesInBytes, bufBoundaryInBytes, 1);
    sp<ImageBufferHeap> pHeap = ImageBufferHeap::create( "ipraw", imgParam,portBufInfo,true);
    srcBuffer = pHeap->createImageBuffer();
    srcBuffer->incStrong(srcBuffer);
    srcBuffer->lockBuf("ipraw",eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
    printf("--- [ip_raw)...flag -8]\n");

    //
    Input src;
    src.mPortID=PORT_IMGI;
    src.mBuffer=srcBuffer;
    src.mPortID.group=0;
    frameParams.mvIn.push_back(src);
    printf("--- [ip_raw...push src done]\n");

    //crop information
    MCrpRsInfo crop;
    crop.mFrameGroup=0;
    crop.mGroupID=1;
    MCrpRsInfo crop2;
    crop2.mFrameGroup=0;
    crop2.mGroupID=2;
    MCrpRsInfo crop3;
    crop3.mFrameGroup=0;
    crop3.mGroupID=3;
    crop.mCropRect.p_fractional.x=0;
    crop.mCropRect.p_fractional.y=0;
    crop.mCropRect.p_integral.x=0;
    crop.mCropRect.p_integral.y=0;
    crop.mCropRect.s.w=_imgi_w_;
    crop.mCropRect.s.h=_imgi_h_;
    crop.mResizeDst.w=_imgi_w_;
    crop.mResizeDst.h=_imgi_h_;
    crop2.mCropRect.p_fractional.x=0;
    crop2.mCropRect.p_fractional.y=0;
    crop2.mCropRect.p_integral.x=0;
    crop2.mCropRect.p_integral.y=0;
    crop2.mCropRect.s.w=_imgi_w_;
    crop2.mCropRect.s.h=_imgi_h_;
    crop2.mResizeDst.w=_imgi_w_;
    crop2.mResizeDst.h=_imgi_h_;
    crop3.mCropRect.p_fractional.x=0;
    crop3.mCropRect.p_fractional.y=0;
    crop3.mCropRect.p_integral.x=0;
    crop3.mCropRect.p_integral.y=0;
    crop3.mCropRect.s.w=_imgi_w_;
    crop3.mCropRect.s.h=_imgi_h_;
    crop3.mResizeDst.w=_imgi_w_;
    crop3.mResizeDst.h=_imgi_h_;
    frameParams.mvCropRsInfo.push_back(crop);
    frameParams.mvCropRsInfo.push_back(crop2);
    frameParams.mvCropRsInfo.push_back(crop3);
    printf("--- [ip_raw...push crop information done\n]");

    /////////
    //output buffer
    IMEM_BUF_INFO imgoBuf;

    imgoBuf.size=_imgi_w_*_imgi_h_*10/8; //output pack 10
    mpImemDrv->allocVirtBuf(&imgoBuf);
    memset((MUINT8*)imgoBuf.virtAddr, 0xffffffff, imgoBuf.size);
    IImageBuffer* outBuffer=NULL;
    MUINT32 bufStridesInBytes_1[3] = {800, 0, 0};
    PortBufInfo_v1 portBufInfo_1 = PortBufInfo_v1( imgoBuf.memID,imgoBuf.virtAddr,0,imgoBuf.bufSecu, imgoBuf.bufCohe);
    IImageBufferAllocator::ImgParam imgParam_1 = IImageBufferAllocator::ImgParam((eImgFmt_BAYER10),MSize(_imgi_w_, _imgi_h_),  bufStridesInBytes_1, bufBoundaryInBytes, 1);
    sp<ImageBufferHeap> pHeap_1 = ImageBufferHeap::create( "ipraw", imgParam_1,portBufInfo_1,true);
    outBuffer = pHeap_1->createImageBuffer();
    outBuffer->incStrong(outBuffer);
    outBuffer->lockBuf("ipraw",eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
    Output dst;
    dst.mPortID=PORT_IMGO;
    dst.mBuffer=outBuffer;
    dst.mPortID.group=0;
    frameParams.mvOut.push_back(dst);

    printf("--- [ip_raw...push dst done]\n");


    /////
    //temp to add, (original done in pass1)
    MUINT32 reg_val=0x0;
    //ISP_WRITE_REG(mpIspDrv,CAM_CTL_SEL, 0, ISP_DRV_USER_ISPF);
    ISP_WRITE_REG(mpIspDrv,CAM_CTL_EN_P1, 0x40000000, ISP_DRV_USER_ISPF);
    reg_val = ISP_READ_REG_NOPROTECT(mpIspDrv,CAM_CTL_EN_P2);
    ISP_WRITE_REG(mpIspDrv,CAM_CTL_EN_P2, reg_val&0x04000000, ISP_DRV_USER_ISPF);//FMT_EN don't touch
    ISP_WRITE_REG(mpIspDrv,CAM_CTL_EN_P2_DMA, 0, ISP_DRV_USER_ISPF);
    ISP_WRITE_REG(mpIspDrv,CAM_CTL_EN_P1_DMA, 0, ISP_DRV_USER_ISPF);
    //disable GGM
    ISP_WRITE_REG(mpIspDrv,CAM_GGM_CTRL, 0, ISP_DRV_USER_ISPF);
    //reset GDMA relative setting
    //ISP_WRITE_REG(mpIspDrv,CAM_CTL_SEL, 0, ISP_DRV_USER_ISPF);
    //interrupt merge
    //ISP_WRITE_BITS(mpIspDrv,CAM_CTL_SPARE3,INT_MRG, 1, ISP_DRV_USER_ISPF);
    /////
    printf("QQ : ISP_READ_REG_NOPROTECT \n");

    //frameParams.mTuningData = NULL;
    enqueParams.mvFrameParams.push_back(frameParams);

    memset((MUINT8*)(frameParams.mvOut[0].mBuffer->getBufVA(0)), 0xffffffff, imgoBuf.size);

    //flush first
    mpImemDrv->cacheFlushAll();
    printf("--- [ip_raw...flush done]\n");


    //enque
    ret = pCCStream->enque(enqueParams);
    if(!ret)
    {
        printf("---ERRRRRRRRR [ip_raw..enque fail]\n");
    }
    else
    {
        printf("---[ip_raw..enque done]\n");
    }

    sleep(2);

    ////
    //deque
    QParams dequeParams;
    ret = pCCStream->deque(dequeParams);
    if(!ret)
    {
        printf("---ERRRRRRRRR [ip_raw(%d)..deque fail]\n", ret);
    }
    else
    {
        printf("---[ip_raw..deque done]\n");
    }

    sleep(1);

    //dump image
    char filename[256];
    sprintf(filename, "ip_raw_imgo_%dx%d_raw10.raw", _imgi_w_,_imgi_h_);
    saveImage(filename, dequeParams.mvFrameParams[0].mvOut[0].mBuffer);
    printf("--- [ip_raw...save file done]\n");

    //free
    srcBuffer->unlockBuf("ipraw");
    mpImemDrv->freeVirtBuf(&imgiBuf);
    outBuffer->unlockBuf("ipraw");
    mpImemDrv->freeVirtBuf(&imgoBuf);
    printf("--- [ip_raw...free memory done]\n");

    pCCStream->uninit("ipraw");
    pCCStream->destroyInstance();
    printf("--- [ip_raw...pStream uninit done]\n");
    mpImemDrv->uninit();
    mpImemDrv->destroyInstance();
    mpIspDrv->uninit("ipraw");
    mpIspDrv->destroyInstance();
    printf("TEST CASE DONE\n");
    return ret;
}

