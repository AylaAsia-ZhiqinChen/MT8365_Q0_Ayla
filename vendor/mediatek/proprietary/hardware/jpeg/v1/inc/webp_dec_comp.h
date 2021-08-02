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

#ifndef __WEBP_DEC_COMP_H__
#define __WEBP_DEC_COMP_H__

#include "m4u_lib.h"

#include <img_dec_comp.h>

#include <mtkcam/imageio/IPipe.h>
#include <mtkcam/imageio/ICamIOPipe.h>
#include <mtkcam/imageio/ICdpPipe.h>
#include <mtkcam/imageio/IPostProcPipe.h>
#include <mtkcam/drv/isp_drv.h>
#include "mtkcam/hal/sensor_hal.h"
#include <mtkcam/drv/res_mgr_drv.h>
#include <mtkcam/campipe/pipe_mgr_drv.h>
 
using namespace NSImageio ;
using namespace NSImageio::NSIspio;

//#define USE_VDEC_SO  

#if 1 //def USE_VDEC_SO    

  #include "vdec_drv_if_private.h"
  //#include "val_api_private.h"
  //#include "val_log.h"
  //#include "main.h"
  #include "vdec_drv_vp8_if.h"
#endif
 
#include "gfmt_hal.h"


#include "img_common_def.h"

/*******************************************************************************
*
********************************************************************************/
#ifndef JUINT32
typedef unsigned int JUINT32;
#endif

#ifndef JINT32
typedef int JINT32;
#endif
 


#if 1

/*******************************************************************************
* class WebpDecComp
********************************************************************************/
class WebpDecComp : public ImageDecoder{
public:
    WebpDecComp();
    virtual ~WebpDecComp();


#if 0
//    enum ColorFormat {
//        
//        IMGDEC_3PLANE_YUV420,       
//        IMGDEC_3PLANE_YUV422
//        
//    }; 

//    enum ColorFormat {
//        kRGB_565_Format,
//        kRGB_888_Format,
//        kBGR_888_Format,
//        kARGB_8888_Format,
//        kABGR_8888_Format,
//        kYUY2_Pack_Format,      // YUYV
//        kUYVY_Pack_Format,      // UYVY        
//        
//        kYUV_3PLANE_Format       // UYVY
//        
//    };
    
    enum  {
        IMG_DEC_MODE_NONE ,        
        IMG_DEC_MODE_FRAME, // frame mode
        IMG_DEC_MODE_LINK ,  // LINK to GDMA
        IMG_DEC_MODE_TILE   // tile mode do row decode
    }  ;
    


   
    enum {
      IMG_MEM_TYPE_PHY,
      IMG_MEM_TYPE_PMEM,
      IMG_MEM_TYPE_M4U,
      IMG_MEM_TYPE_ION
      
    };

    


    
    enum IMG_DEC_RESULT {
      IMG_DEC_RST_CFG_ERR, //false
      IMG_DEC_RST_DONE,    // true
      IMG_DEC_RST_ROW_DONE,
      IMG_DEC_RST_HUFF_ERROR,
      IMG_DEC_RST_DECODE_FAIL,
      IMG_DEC_RST_FORMAT_FAIL,
      IMG_DEC_RST_BS_UNDERFLOW
      
    };
  
    
//    bool lock();
//    bool unlock();
//    bool parse();
//    bool Start();

//    JUINT32 getImgWidth() { return fImgWidth; }
//    JUINT32 getImgHeight() { return fImgHeight; }
    
//    void setOutWidth(JUINT32 width) { fOutWidth = width; }
//    void setOutHeight(JUINT32 height) { fOutHeight = height; }
//    void setOutFormat(ColorFormat format) { fOutFormat = format; }
//    void setSrcAddr(unsigned char *srcAddr) { fSrcAddr = srcAddr; }    // in PA
//    void setSrcAddrPA(unsigned int srcAddrPA) { fSrcAddrPA = srcAddrPA ;}
//    void setDstAddr(unsigned char *dstAddr) { fDstAddr = dstAddr; }
//    void setSrcSize(JUINT32 srcSize) { fSrcBufferSize = TO_CEIL(srcSize, 128) + 128 ; }  //must be TO_CEIL(fileSize, 128)  + 128 
//    void setDstSize(JUINT32 dstSize) { fOutBufSize = dstSize; }
//    void setDither(bool doDither) { isDither = doDither; }
//    void setRangeDecode(JUINT32 left, JUINT32 top, JUINT32 right, JUINT32 bottom);    
    void setProcHandler(void* h) { fProcHandler = h; }      
#endif 

    enum {
        WEBP_VDEC_MODE_NONE,        
        WEBP_VDEC_MODE_FRAME, //MUST FRAME MODE LINK    to FMT (can   link to GDMA or not)
        WEBP_VDEC_MODE_ROW,   //MUST ROW   MODE NO_LINK to FMT (can't link to GDMA )
        WEBP_VDEC_MODE_VIDEO, //decode small than 1920x1088 
        
    };    


    virtual bool lock();
    virtual bool unlock();
    virtual bool parse();

    //virtual bool Start();
    //virtual bool ResumeDecRow( JUINT32 decRowNum);
    virtual JUINT32 StartConfig() ; //{return true;}
    virtual JUINT32 Start();
    virtual JUINT32 ResumeDecRow( JUINT32 decRowNum);    
    bool decodeRow( JUINT32 decRowNum);
    JUINT32 decodeOneRow();
    virtual bool Config(JUINT32 mode);


     
    
    virtual JUINT32 getOutFormat(){ return fOutFormat; }

    virtual JUINT32 getJpgWidth() { return fImgWidth; }
    virtual JUINT32 getJpgHeight() { return fImgHeight; }
    virtual JUINT32 getMcuRow() { return fMcuRow; }
    virtual JUINT32 getMcuHeightY() { return fMcuHeightY; }
    virtual JUINT32 getMcuHeightC() { return fMcuHeightC; }
    
    
    virtual JUINT32 getMinLumaBufSize(){ return fMinLumaBufSize ;  }
    virtual JUINT32 getMinCbCrBufSize(){ return fMinCbCrBufSize ;  }

    virtual JUINT32 getMinLumaBufStride(){ return fMinLumaBufStride ;  }
    virtual JUINT32 getMinCbCrBufStride(){ return fMinCbCrBufStride ;  }    

    virtual JUINT32 getLumaImgStride(){ return fLumaImgStride ;  }
    virtual JUINT32 getCbCrImgStride(){ return fCbCrImgStride ;  }  

    virtual JUINT32 getLumaImgHeight(){ return fLumaImgHeight ; } 
    virtual JUINT32 getCbCrImgHeight(){ return fCbCrImgHeight ; } 
    
    // in VA
    virtual void setSrcAddr  (unsigned char * srcAddr) { fSrcAddr = srcAddr ;}
    // in PA
    virtual void setSrcAddrPA(unsigned int srcAddrPA) { fSrcAddrPA = srcAddrPA ;}


    virtual void setSrcSize(JUINT32 srcBufSize, JUINT32 srcSize) { fSrcBufferSize = srcBufSize ;
                                                                 fSrcBsSize = TO_CEIL(srcSize, 128) + 128 ; 
                                                                 if(fSrcBsSize> fSrcBufferSize) fSrcBsSize = fSrcBufferSize ;}  //must be TO_CEIL(fileSize, 128)  + 128 


    // out VA
    virtual void setDstAddr0(JUINT32 dstAddr, JUINT32 dstAddr1, JUINT32 dstAddr2) 
                    { fDecOutBufAddr0[0] = dstAddr; fDecOutBufAddr0[1] = dstAddr1; fDecOutBufAddr0[2] = dstAddr2;
                      fDstAddr = (unsigned char *)dstAddr; }
    
    virtual void setDstAddr1(JUINT32 dstAddr, JUINT32 dstAddr1, JUINT32 dstAddr2) 
                    { fDecOutBufAddr1[0] = dstAddr; fDecOutBufAddr1[1] = dstAddr1; fDecOutBufAddr1[2] = dstAddr2;}

    // out PA
    virtual void setDstAddrPA0(JUINT32 dstAddrPA, JUINT32 dstAddrPA1, JUINT32 dstAddrPA2) 
                    { fDecOutBufAddrPA0[0] = dstAddrPA; fDecOutBufAddrPA0[1] = dstAddrPA1; fDecOutBufAddrPA0[2] = dstAddrPA2;
                      //fDstAddr = dstAddrPA; 
                      //setPA_flag = 1;
                      fMemType = IMG_MEM_TYPE_PHY;
                      }
    
    virtual void setDstAddrPA1(JUINT32 dstAddrPA, JUINT32 dstAddrPA1, JUINT32 dstAddrPA2) 
                    { fDecOutBufAddrPA1[0] = dstAddrPA; fDecOutBufAddrPA1[1] = dstAddrPA1; fDecOutBufAddrPA1[2] = dstAddrPA2;}


    
    //current NOUSE
    virtual void setDstBufStride( JUINT32 dstStride, JUINT32 dstStride1){ fDstBufStride[0] = dstStride; fDstBufStride[1] = fDstBufStride[2] = dstStride1 ;}

    /* make sure output valid buffer size */
    virtual void setDstBufSize(JUINT32 dstSize, JUINT32 dstSize1) 
                     { fDecDstBufSize[0] = dstSize; fDecDstBufSize[1] = fDecDstBufSize[2] =  dstSize1;
                        fDstSize = fDecDstBufSize[0];      }
    // set additional brz actor, should check if valid factor
    virtual void setBRZ(JUINT32 brz){ fBRZ = brz; }
    virtual void setTileBufNum(JUINT32 ringBufRowNum){ fDstTileBufRowNum = ringBufRowNum ;}
    virtual bool setFirstDecRow(JUINT32 rowNum){fFirstDecRowNum = rowNum ; return true ;}

 

    
private:
   
    bool onStartGdmaLink() ;
    bool onStart();
    bool onStartFrame();
    //bool onStartTile();
    //bool onStartGdma();
    bool checkParam();
    bool hwSyncSrc(unsigned int size, unsigned char* srcAddr, unsigned int vDecSrcAddrPA ) ;
    
    bool config_gfmt(JUINT32 srcRowIdx, JUINT32 dstRowIdx);
    
    //bool allocIOMEM();
    bool allocMMU();//(unsigned int *vDecSrcAddr, unsigned int *vDecSrcAddrPA );
    bool freeMMU();
    bool alloc_m4u();
    bool free_m4u();
    bool allocDecDstMEM();
    bool freeDecDstMEM();
    
    //vp8 function    
    bool Decoder_deInit();
    bool Decoder_SetMode(JUINT32 mode);
    bool Decoder_DecoderGo();
    
    
    bool fmtOnlyCreateScaler();
    bool fmtOnlyDestroyScaler();
    
    
    
    bool islock;
    bool isDither;
    bool isRangeDecode;
    
    int gfmtID ;
    
    JUINT32 fGoFlag_fmt ;
    JUINT32 fSkipVdec ;
    JUINT32 fSkip_fmt ;
    
    JUINT32 fDecOutFormat ;

    
    //JUINT32 fTileBufRowNum ;
    //JUINT32 fImgMcuRowNum ;
    //JUINT32 fImgMcuHeight ;

    

    JUINT32 fImgWidth;
    JUINT32 fImgHeight;
    //JUINT32 fOutWidth;
    //JUINT32 fOutHeight;

    JUINT32 fLeft;
    JUINT32 fRight;
    JUINT32 fTop;
    JUINT32 fBottom;
    
    //ColorFormat fOutFormat;
    
    JUINT32 fDecType ;
    
    
    JUINT32 fMemType ;
    JUINT32 fMemTypeDefault ;
    

    //unsigned char *fSrcAddr;
    //unsigned char *fDstAddr;
    //JUINT32 fSrcAddrPA ;

    //JUINT32 fSrcBufferSize;    
    JUINT32 fOutBufSize;

    JUINT32 fInBufAddrPA;
    JUINT32 fOutBufAddrPA;
    
    
    JUINT32 fDecDstImgWidth[3] ; 
    JUINT32 fDecDstImgHeight[3] ;
    JUINT32 fDecDstBufStride[3] ;  
    
    //JUINT32 fDecDstBufAddr0[3] ;
    //JUINT32 fDecDstBufAddr1[3] ;
    //JUINT32 fDecDstBufSize[3] ;    
    


    unsigned char *fDecDstBufAddrVA0[3];
    unsigned char *fDecDstBufAddrVA1[3];
    
    //int fDecPmemFD[3];
    
    JUINT32 fDecDstBufAddrPA0[3] ;
    JUINT32 fDecDstBufAddrPA1[3] ;
    
    JUINT32 fUseImgRszCmodel ;
    

    void* fProcHandler;
    
    /************************/
    //  vp8 decoder
    /************************/
    
    JUINT32 fVDecDone ;
    JUINT32 fVDecInitDone ;
    
    
    JUINT32 fDecBufHeight ;

#if 1 //def USE_VDEC_SO
 
    //VAL_BOOL_T  fgERRORFlag;
    VAL_HANDLE_T fDecHandle;
    VDEC_DRV_RINGBUF_T fRingBuf;   
    VDEC_DRV_SEQINFO_T fSeqInfo;    
    //VDEC_DRV_FRAMEBUF_T vDecDst;
    VDEC_DRV_FRAMEBUF_T frame;    
    //VDEC_DRV_VP8_HANDLE_T *prDrvVP8handle;    
    VAL_UCHAR_T *bitstream_va;
    VAL_UINT32_T bitstream_pa;
    VAL_UINT32_T frame_sz;
    VAL_UINT32_T u4I;

#endif
    
    JUINT32 fDecBufSize[2];
    JUINT32 fDecBufStride;
    JUINT32 fDecBufAddrPA[2] ;
    
    /************************/
    //        FMT 
    /************************/    

    JUINT32 fFmtSrcBufRowNum ;

    JUINT32 fFmtSrcAddrPA[2];
    JUINT32 fFmtSrcBufStride[2];
    JUINT32 fFmtSrcMBrowSize[2] ;     //0:Luma , 1:Chroma
    

    
    JUINT32 fFmtDstAddrPA0[3] ;
    JUINT32 fFmtDstAddrPA1[3] ;    
    JUINT32 fFmtDstBufSize[3] ;
    JUINT32 fFmtDstBufStride[3];
    JUINT32 fFmtDstMBrowSize[3] ;     //0:Y, 1:Cb, 2:Cr
    
    
    ResMgrDrv* pResMgr;
    RES_MGR_DRV_MODE_STRUCT ResMgrMode;
    //PipeMgrDrv* pPipeMgr;

    ICdpPipe* pCdpPipeRing;    
    EScenarioID scenario_init ;//= eScenarioID_N3D_IC;    

//=============================================================




    //JPEG_DEC_COMP_MODE fDecMode ;
    
    JUINT32 fJpgWidth;
    JUINT32 fJpgHeight;
    JUINT32 fOutWidth;
    JUINT32 fOutHeight;
    
    JUINT32 fBRZ ;
    JUINT32 fMinLumaBufSize ;
    JUINT32 fMinCbCrBufSize ;

    JUINT32 fMinLumaBufStride ;
    JUINT32 fMinCbCrBufStride ;
    
    JUINT32 fLumaImgStride ;
    JUINT32 fCbCrImgStride ;
    JUINT32 fLumaImgHeight ;
    JUINT32 fCbCrImgHeight ;
    
    JUINT32 fFirstDecRowNum ;
    JUINT32 fDecRowCnt ;
    
    JUINT32 fDstBufStride[3];
    
    JUINT32 fOutFormat;

    unsigned char *fSrcAddr;
    unsigned int fSrcAddrPA;
    
    unsigned char *fDstAddr;

    JUINT32 fSrcBufferSize;    
    JUINT32 fSrcBsSize ;
    JUINT32 fDstSize;

    JUINT32 fSrcConfigAddr;
    JUINT32 fDstConfigAddr;
    

    JUINT32 fDecDstBufAddr0[3] ; //set HW
    JUINT32 fDecDstBufAddr1[3] ; //set HW   
    JUINT32 fDecDstBufSize[3] ;    
    JUINT32 fDstTileBufRowNum ;
    JUINT32 fDecOutBufAddr0[3] ;
    JUINT32 fDecOutBufAddr1[3] ;
    
    JUINT32 fFmtDstMcuRowNum  ;
    JUINT32 fFmtMBrowHeight[2] ;

    JUINT32 fDecOutBufAddrPA0[3] ;
    JUINT32 fDecOutBufAddrPA1[3] ;
    JUINT32 setPA_flag ;
    
    //for tile
    
    JUINT32 fMcuRow ;
    JUINT32 fMcuHeightY ;
    JUINT32 fMcuHeightC ;










//===============================================================

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
    
    MTKM4UDrv *pM4uDrv;
    
    M4U_MODULE_ID_ENUM fm4uVDecID;
    M4U_MODULE_ID_ENUM fm4uFmtID ;
    unsigned int fSrcMVA;
    
    JUINT32 fDecSrcM4UAddrMva;
    JUINT32 fDecDstM4UAddrMva[2] ;
    
    JUINT32 fFmtSrcM4UAddrMva[2] ;
    JUINT32 fFmtDstM4UAddrMva0[3];
    JUINT32 fFmtDstM4UAddrMva1[3];
    
    unsigned char* fDecDstMallocYAddr    ;
    unsigned char* fDecDstMallocCbCrAddr ;
    unsigned char* fDecDstMallocYAddr_fmt    ;
    unsigned char* fDecDstMallocYAddr_org ;
    JUINT32 fDoubleBuf ;
    
    JUINT32 fvDecSrcAddr ;
    JUINT32 fvDecSrcAddrPA ;
    
    
    int decID;
};
#endif


#endif 