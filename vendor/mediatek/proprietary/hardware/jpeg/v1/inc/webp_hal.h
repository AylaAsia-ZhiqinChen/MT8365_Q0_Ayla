/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
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
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

#ifndef __WEBP_HAL_H__
#define __WEBP_HAL_H__

  #include "m4u_lib.h"

#include <img_cdp_pipe.h>

#include "img_common_def.h"
/*******************************************************************************
*
********************************************************************************/
#ifndef UINT32
typedef unsigned int UINT32;
#endif

#ifndef INT32
typedef int INT32;
#endif
 



/*******************************************************************************
* class WebpDecHal
********************************************************************************/
class WebpDecHal {
public:
    WebpDecHal();
    virtual ~WebpDecHal();

    enum ColorFormat {
        kRGB_565_Format,
        kRGB_888_Format,
        kBGR_888_Format,
        kARGB_8888_Format,
        kABGR_8888_Format,
        kYUY2_Pack_Format,      // YUYV
        kUYVY_Pack_Format,      // UYVY        
        
        kYUV_3PLANE_Format       // UYVY
        
    };
    
    enum {
        IMG_DEC_MODE_NONE,        
        IMG_DEC_MODE_FRAME,
        IMG_DEC_MODE_LINK,
        IMG_DEC_MODE_TILE
    };
    
    enum WEBP_DEC_HAL_RESULT {
      WEBP_DEC_HAL_RST_CFG_ERR,
      WEBP_DEC_HAL_RST_DONE,      
      WEBP_DEC_HAL_RST_ROW_DONE,
      WEBP_DEC_HAL_RST_HUFF_ERROR,
      WEBP_DEC_HAL_RST_DECODE_FAIL,
      WEBP_DEC_HAL_RST_BS_UNDERFLOW
      
    };

    
    
    bool lock();
    bool unlock();
    bool parse();
    bool start();

    UINT32 getImgWidth() { return fImgWidth; }
    UINT32 getImgHeight() { return fImgHeight; }
    
    void setOutWidth(UINT32 width) { fOutWidth = width; }
    void setOutHeight(UINT32 height) { fOutHeight = height; }
    void setOutFormat(ColorFormat format) { fOutFormat = format; }
    void setSrcAddr(unsigned char *srcAddr) { fSrcAddr = srcAddr; }
    void setDstAddr(unsigned char *dstAddr) { fDstAddr = dstAddr; }
    void setSrcSize(UINT32 srcSize) { fSrcBufferSize = TO_CEIL(srcSize, 128) + 128 ; }  //must be TO_CEIL(fileSize, 128)  + 128 
    void setDstSize(UINT32 dstSize) { fOutBufSize = dstSize; }
    void setDither(bool doDither) { isDither = doDither; }
    void setRangeDecode(UINT32 left, UINT32 top, UINT32 right, UINT32 bottom);    
    void setProcHandler(void* h) { fProcHandler = h; }      

    
private:
    bool onStart();
    bool onStartFrame();
    bool onStartTile();
    bool onStartGdma();
    bool checkParam();
    bool allocIOMEM();
    
    bool config_FMT();
    
    bool allocFmtMEM();
    bool freeFmtMEM();
    bool allocDecMEM();
    
    //vp8 function
    bool Decoder_deInit();
    bool Decoder_SetMode();
    bool Decoder_DecodeOneRow();
    
    
    
    bool islock;
    bool isDither;
    bool isRangeDecode;
    
    int gfmtID ;
    
    UINT32 fDecOutFormat ;
    UINT32 fCdpMode ;
    UINT32 fCdpDstFormat ;
    UINT32 fCdpDstBufSize ;
    UINT32 fCdpDstImgWidth ;
    UINT32 fCdpDstImgHeight ;
    UINT32 fCdpDstBufAddr ;

    //JpgDecComp* pJpgDecComp ;
    ImgCdpPipe* pImgCdp ;
    
    UINT32 fTileBufRowNum ;
    UINT32 fImgMcuRowNum ;
    UINT32 fImgMcuHeight ;

    

    UINT32 fImgWidth;
    UINT32 fImgHeight;
    UINT32 fOutWidth;
    UINT32 fOutHeight;

    UINT32 fLeft;
    UINT32 fRight;
    UINT32 fTop;
    UINT32 fBottom;
    
    ColorFormat fOutFormat;
    
    UINT32 fScaleType ;

    unsigned char *fSrcAddr;
    unsigned char *fDstAddr;

    UINT32 fSrcBufferSize;    
    UINT32 fOutBufSize;

    UINT32 fInBufAddrPA;
    UINT32 fOutBufAddrPA;
    
    
    UINT32 fDecDstImgWidth[3] ; 
    UINT32 fDecDstImgHeight[3] ;
    UINT32 fDecDstBufStride[3] ;  
    
    UINT32 fDecDstBufAddr0[3] ;
    UINT32 fDecDstBufAddr1[3] ;
    UINT32 fDecDstBufSize[3] ;    
    //UINT32 fDstTileBufRowNum ;
    
    //UINT32 fOutBufSize ;    
    


    unsigned char *fDecDstBufAddrVA0[3];
    unsigned char *fDecDstBufAddrVA1[3];
    
    //int fDecPmemFD[3];
    
    UINT32 fDecDstBufAddrPA0[3] ;
    UINT32 fDecDstBufAddrPA1[3] ;
    
    UINT32 fUseImgRszCmodel ;
    

    void* fProcHandler;
    
    /************************/
    //  vp8 decoder
    /************************/
    UINT32 fDecBufHeight ;

    VAL_BOOL_T  fgERRORFlag;
    VAL_HANDLE_T fDecHandle;
    VDEC_DRV_RINGBUF_T fRingBuf;   
    VDEC_DRV_SEQINFO_T fSeqInfo;    
    VDEC_DRV_FRAMEBUF_T *frame;
    //VDEC_DRV_VP8_HANDLE_T *prDrvVP8handle;    
    VAL_UCHAR_T *bitstream_va;
    VAL_UINT32_T bitstream_pa;
    VAL_UINT32_T frame_sz;
    VAL_UINT32_T u4I;
    
    UINT32 fDecBufSize[2];
    UINT32 fDecBufAddrPA[2] ;
    
    /************************/
    //        FMT 
    /************************/    

    UINT32 fFmtSrcBufStride[2];
    UINT32 fFmtDstBufStride[2];
    
    UINT32 fFmtSrcAddrPA[2];
    
    UINT32 fFmtDstAddrPA0[3] ;
    UINT32 fFmtDstAddrPA1[3] ;
    
    UINT32 fFmtDstBufSize[3] ;
    
    
#ifdef USE_PMEM
    unsigned char *fSrcPmemVA;
    unsigned char *fOutPmemVA;
    int fSrcPmemFD;
    int fOutPmemFD;
    
    unsigned char *fDecPmemVA[2];
    int fDecPmemFD[2] ;
    
    
    unsigned char *fFmtPmemVA0[3] ;
    unsigned char *fFmtPmemVA1[3] ;
    int fFmtPmemFD0[3] ;
    int fFmtPmemFD1[3] ;


    //FMT
    

    
    
    
#else
    MTKM4UDrv *pM4uDrv;
    unsigned int fSrcMVA;
#endif
    int decID;
};



#endif 