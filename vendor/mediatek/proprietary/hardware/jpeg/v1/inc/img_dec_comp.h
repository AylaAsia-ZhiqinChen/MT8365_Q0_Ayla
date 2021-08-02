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

#ifndef __IMG_DEC_COMP_H__
#define __IMG_DEC_COMP_H__

//#include "m4u_lib.h"

#ifdef MTK_LOG_ENABLE
#undef MTK_LOG_ENABLE
#endif
#define MTK_LOG_ENABLE 1
#include <cutils/log.h>

/*******************************************************************************
*
********************************************************************************/
#ifndef JUINT32
typedef unsigned int JUINT32;
#endif

#ifndef JINT32
typedef int JINT32;
#endif
 

//#define TO_CEIL(x,a) ( ((x) + ((a)-1)) & ~((a)-1) )
#ifdef LOG_TAG
 #undef LOG_TAG
#endif

//#define JPEG_PROFILING   

#define LOG_TAG "ImageDecoder"

#define IMGDEC_W(fmt, arg...)    ALOGW(fmt, ##arg)
#define IMGDEC_D(fmt, arg...)    ALOGD(fmt, ##arg)



/*******************************************************************************
* class ImageDecoder
********************************************************************************/
class ImageDecoder {
public:
    ImageDecoder(){
       //IMGDEC_D("ImageDecoder::ImageDecoder()\n");
    };
    virtual ~ImageDecoder(){
       //IMGDEC_D("ImageDecoder::~ImageDecoder()\n");
    };


    enum ColorFormat {
        IMGDEC_3PLANE_NONE,
        IMGDEC_3PLANE_YUV420,       
        IMGDEC_3PLANE_YUV422,      
        IMGDEC_3PLANE_GRAY,
        IMGDEC_3PLANE_YUV444
        
    }; 

    enum JpegColorFormat {
        IMG_COLOR_NONE,
        IMG_COLOR_YUV420,       
        IMG_COLOR_YUV422,  //2x1, 1x1, 1x1      
        IMG_COLOR_YUV444,       
        IMG_COLOR_YUV422V, //1x2, 1x1, 1x1      
        IMG_COLOR_GRAY,
        IMG_COLOR_NO_SUPPORT, 
        IMG_COLOR_INVALID
    }; 


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



    virtual bool lock() {return true;}

    virtual bool unlock(){return true;}
    virtual bool parse(){return true;}

    //virtual bool Start(){return true;}
    //virtual bool ResumeDecRow( JUINT32 decRowNum){return true;}
    
    virtual JUINT32 StartConfig(){return true;}
    virtual JUINT32 Start(){return true;}
    virtual JUINT32 ResumeDecRow( JUINT32 decRowNum){return true;}
    
    virtual bool Config(JUINT32 mode){return true;}
    
    
    virtual JUINT32 getOutFormat(){ return 0; }     //{ return fOutFormat; }

    virtual JUINT32 getJpgWidth(){ return 0; }      //{ return fJpgWidth; }
    virtual JUINT32 getJpgHeight(){ return 0; }     //{ return fJpgHeight; }
    virtual JUINT32 getMcuRow() { return 0; }      //{ return fMcuRow; }
    virtual JUINT32 getMcuHeightY() { return 0; }      //{ return fMcuHeightY; }
    virtual JUINT32 getMcuHeightC() { return 0; }      //{ return fMcuHeightC; }
    
    
    virtual JUINT32 getMinLumaBufSize(){ return 0; }      //{ return fMinLumaBufSize { return 0; }  }
    virtual JUINT32 getMinCbCrBufSize(){ return 0; }      //{ return fMinCbCrBufSize ;  }

    virtual JUINT32 getMinLumaBufStride(){ return 0; }      //{ return fMinLumaBufStride ;  }
    virtual JUINT32 getMinCbCrBufStride(){ return 0; }      //{ return fMinCbCrBufStride ;  }    

    virtual JUINT32 getLumaImgStride(){ return 0; }      //{ return fLumaImgStride ;  }
    virtual JUINT32 getCbCrImgStride(){ return 0; }      //{ return fCbCrImgStride ;  }  
  
    virtual JUINT32 getLumaImgHeight(){ return 0; }      
    virtual JUINT32 getCbCrImgHeight(){ return 0; }      

    
    // in VA
    virtual void setSrcAddr  (unsigned char * srcAddr) { return ; }      //{ fSrcAddr = srcAddr ;}
    // in PA
    virtual void setSrcAddrPA(unsigned int srcAddrPA) { return ; }      //{ fSrcAddrPA = srcAddrPA ;}

    virtual void setSrcSize(JUINT32 srcBufSize, JUINT32 srcSize) { return ; }      //{ fSrcBufferSize = TO_CEIL(srcSize, 128) + 128 ; }  //must be TO_CEIL(fileSize, 128)  + 128 


    // out VA
    virtual void setDstAddr0(JUINT32 dstAddr, JUINT32 dstAddr1, JUINT32 dstAddr2){ return ; }
          //{ fDecOutBufAddr0[0] = dstAddr; fDecOutBufAddr0[1] = dstAddr1; fDecOutBufAddr0[2] = dstAddr2;
          //            fDstAddr = (unsigned char *)dstAddr; }
    
    virtual void setDstAddr1(JUINT32 dstAddr, JUINT32 dstAddr1, JUINT32 dstAddr2) { return ; }
          //{ fDecOutBufAddr1[0] = dstAddr; fDecOutBufAddr1[1] = dstAddr1; fDecOutBufAddr1[2] = dstAddr2;}

    // out PA
    virtual void setDstAddrPA0(JUINT32 dstAddrPA, JUINT32 dstAddrPA1, JUINT32 dstAddrPA2) { return ; }
          //{ fDecOutBufAddrPA0[0] = dstAddrPA; fDecOutBufAddrPA0[1] = dstAddrPA1; fDecOutBufAddrPA0[2] = dstAddrPA2;
                      //fDstAddr = dstAddrPA; 
                      //setPA_flag = 1;}
    
    virtual void setDstAddrPA1(JUINT32 dstAddrPA, JUINT32 dstAddrPA1, JUINT32 dstAddrPA2) { return ; }
          //{ fDecOutBufAddrPA1[0] = dstAddrPA; fDecOutBufAddrPA1[1] = dstAddrPA1; fDecOutBufAddrPA1[2] = dstAddrPA2;}


    
    //current NOUSE
    virtual void setDstBufStride( JUINT32 dstStride, JUINT32 dstStride1){ return ; }
          //{ fDstBufStride[0] = dstStride; fDstBufStride[1] = fDstBufStride[2] = dstStride1 ;}

    /* make sure output valid buffer size */
    virtual void setDstBufSize(JUINT32 dstSize, JUINT32 dstSize1){ return ; }
          //{ fDecDstBufSize[0] = dstSize; fDecDstBufSize[1] = fDecDstBufSize[2] =  dstSize1;
          //              fDstSize = fDecDstBufSize[0];      }
    // set additional brz actor, should check if valid factor
    virtual void setBRZ(JUINT32 brz){ return ; }      //{ fBRZ = brz; }
    virtual void setTileBufNum(JUINT32 ringBufRowNum){ return ; }      //{ fDstTileBufRowNum = ringBufRowNum ;}
    virtual bool setFirstDecRow(JUINT32 rowNum){ return true; }      //{fFirstDecRowNum = rowNum ; return true ;}
    virtual bool setQuality(JUINT32 level){ return true; }





#if 0
{
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
//    void setProcHandler(void* h) { fProcHandler = h; }      


    
private:
    bool onStart();
    bool onStartFrame();
    //bool onStartTile();
    //bool onStartGdma();
    bool checkParam();
    
    bool config_gfmt(JUINT32 srcRowIdx, JUINT32 dstRowIdx);
    
    bool allocIOMEM();
    bool freeIOMEM();
    bool allocFmtDstMEM();
    bool freeFmtDstMEM();
    bool allocDecDstMEM();
    bool freeDecDstMEM();
    
    //vp8 function
    bool Decoder_deInit();
    bool Decoder_SetMode();
    bool Decoder_DecoderGo();
    
    
    
    bool islock;
    bool isDither;
    bool isRangeDecode;
    
    int gfmtID ;
    
    JUINT32 fDecOutFormat ;

    
    //JUINT32 fTileBufRowNum ;
    JUINT32 fImgMcuRowNum ;
    JUINT32 fImgMcuHeight ;

    

    JUINT32 fImgWidth;
    JUINT32 fImgHeight;
    //JUINT32 fOutWidth;
    //JUINT32 fOutHeight;

    JUINT32 fLeft;
    JUINT32 fRight;
    JUINT32 fTop;
    JUINT32 fBottom;
    
    //ColorFormat fOutFormat;
    
    JUINT32 fScaleType ;
    
    
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
    
    
    JUINT32 fDecBufHeight ;

    //VAL_BOOL_T  fgERRORFlag;
    VAL_HANDLE_T fDecHandle;
    VDEC_DRV_RINGBUF_T fRingBuf;   
    VDEC_DRV_SEQINFO_T fSeqInfo;    
    VDEC_DRV_FRAMEBUF_T vDecDst;
    //VDEC_DRV_VP8_HANDLE_T *prDrvVP8handle;    
    VAL_UCHAR_T *bitstream_va;
    VAL_UINT32_T bitstream_pa;
    VAL_UINT32_T frame_sz;
    VAL_UINT32_T u4I;
    
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
    JUINT32 fFmtDstBufStride[2];
    JUINT32 fFmtDstMBrowSize[3] ;     //0:Y, 1:Cb, 2:Cr
    
    

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
    
    JUINT32 fFirstDecRowNum ;
    JUINT32 fDecRowCnt ;
    
    JUINT32 fDstBufStride[3];
    
    ColorFormat fOutFormat;

    unsigned char *fSrcAddr;
    unsigned int fSrcAddrPA;
    
    unsigned char *fDstAddr;

    JUINT32 fSrcBufferSize;    
    JUINT32 fDstSize;

    JUINT32 fSrcConfigAddr;
    JUINT32 fDstConfigAddr;
    

    JUINT32 fDecDstBufAddr0[3] ; //set HW
    JUINT32 fDecDstBufAddr1[3] ; //set HW   
    JUINT32 fDecDstBufSize[3] ;    
    JUINT32 fDstTileBufRowNum ;
    JUINT32 fDecOutBufAddr0[3] ;
    JUINT32 fDecOutBufAddr1[3] ;    

    JUINT32 fDecOutBufAddrPA0[3] ;
    JUINT32 fDecOutBufAddrPA1[3] ;
    JUINT32 setPA_flag ;
    
    //for tile
    
    JUINT32 fMcuRow ;
    JUINT32 fMcuHeightY ;
    JUINT32 fMcuHeightC ;










//===============================================================
    
//#ifdef USE_PMEM
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
    

    
    
    
//#else
    MTKM4UDrv *pM4uDrv;
    unsigned int fSrcMVA;
//#endif
    int decID;
}    
#endif    
    
};



#endif 