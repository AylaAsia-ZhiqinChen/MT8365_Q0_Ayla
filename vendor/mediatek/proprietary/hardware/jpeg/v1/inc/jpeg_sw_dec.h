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

#ifndef __JPEG_SW_DEC_H__
#define __JPEG_SW_DEC_H__



#include "SwType.h"
#include "SwStream.h"
#include "SwJpegUtility.h"

extern "C" {
    #include "jpeglib.h"
    #include "jerror.h"
}


#include "img_dec_comp.h"

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
 
 
#undef bool
#define bool bool


#ifdef LOG_TAG
#undef LOG_TAG
#endif
 
#define LOG_TAG "JpgDecHal"

#define JPG_DBG(fmt, arg...)    ALOGD(fmt, ##arg)
#define JPG_WARN(fmt, arg...)   ALOGW(fmt, ##arg)
#define JPG_ERR(fmt, arg...)    ALOGE(fmt, ##arg)


/*******************************************************************************
* class JpgSwDec
********************************************************************************/


class JpgSwDec : public ImageDecoder{
public:
    JpgSwDec();
    virtual ~JpgSwDec();

//    enum ColorFormat {
//        
//        IMGDEC_3PLANE_YUV420,       
//        IMGDEC_3PLANE_YUV422
//        
//    }; 
    

    
    
    bool lock();
    bool unlock();
    bool parse();
    
    JUINT32 StartConfig() {return true;}
    JUINT32 Start();
    JUINT32 ResumeDecRow( JUINT32 decRowNum);
    bool Config(JUINT32 mode);
    
    
    JUINT32 getOutFormat(){ return fOutFormat; }

    JUINT32 getJpgWidth() { return fJpgWidth; }
    JUINT32 getJpgHeight() { return fJpgHeight; }
    JUINT32 getMcuRow() { return fMcuRow; }
    JUINT32 getMcuHeightY() { return fMcuHeightY; }
    JUINT32 getMcuHeightC() { return fMcuHeightC; }
    
    
    JUINT32 getMinLumaBufSize(){ return fMinLumaBufSize ;  }
    JUINT32 getMinCbCrBufSize(){ return fMinCbCrBufSize ;  }

    JUINT32 getMinLumaBufStride(){ return fMinLumaBufStride ;  }
    JUINT32 getMinCbCrBufStride(){ return fMinCbCrBufStride ;  }    

    JUINT32 getLumaImgStride(){ return fLumaImgStride ;  }
    JUINT32 getCbCrImgStride(){ return fCbCrImgStride ;  }  
 
    JUINT32 getLumaImgHeight(){ return fLumaImgHeight ; } 
    JUINT32 getCbCrImgHeight(){ return fCbCrImgHeight ; } 
    
    // in VA
    void setSrcAddr  (unsigned char * srcAddr) { fSrcAddr = srcAddr ;}
    // in PA
    void setSrcAddrPA(unsigned int srcAddrPA) { fSrcAddrPA = srcAddrPA ;}

    void setSrcSize(JUINT32 srcBufSize, JUINT32 srcSize) { fSrcBufferSize = srcBufSize ;
                                                         fSrcBsSize = TO_CEIL(srcSize, 128) + 128 ; 
                                                         if(fSrcBsSize> fSrcBufferSize) fSrcBsSize = fSrcBufferSize ; }  //must be TO_CEIL(fileSize, 128)  + 128 

    // out VA
    void setDstAddr0(JUINT32 dstAddr, JUINT32 dstAddr1, JUINT32 dstAddr2) 
                    { fDecOutBufAddr0[0] = dstAddr; fDecOutBufAddr0[1] = dstAddr1; fDecOutBufAddr0[2] = dstAddr2;
                      fDstAddr = (unsigned char *)dstAddr; 
                      //rowptr = (unsigned char *)fDecOutBufAddr0[0];
                      }
    
    void setDstAddr1(JUINT32 dstAddr, JUINT32 dstAddr1, JUINT32 dstAddr2) 
                    { fDecOutBufAddr1[0] = dstAddr; fDecOutBufAddr1[1] = dstAddr1; fDecOutBufAddr1[2] = dstAddr2;}

    // out PA
    void setDstAddrPA0(JUINT32 dstAddrPA, JUINT32 dstAddrPA1, JUINT32 dstAddrPA2) 
                    { fDecOutBufAddrPA0[0] = dstAddrPA; fDecOutBufAddrPA0[1] = dstAddrPA1; fDecOutBufAddrPA0[2] = dstAddrPA2;
                      //fDstAddr = dstAddrPA; 
                      fMemType = IMG_MEM_TYPE_PHY;
                    }
    
    void setDstAddrPA1(JUINT32 dstAddrPA, JUINT32 dstAddrPA1, JUINT32 dstAddrPA2) 
                    { fDecOutBufAddrPA1[0] = dstAddrPA; fDecOutBufAddrPA1[1] = dstAddrPA1; fDecOutBufAddrPA1[2] = dstAddrPA2;}


    
    //current NOUSE
    void setDstBufStride( JUINT32 dstStride, JUINT32 dstStride1){ fDstBufStride[0] = dstStride; fDstBufStride[1] = fDstBufStride[2] = dstStride1 ;}

    /* make sure output valid buffer size */
    void setDstBufSize(JUINT32 dstSize, JUINT32 dstSize1) 
                     { fDecDstBufSize[0] = dstSize; fDecDstBufSize[1] = fDecDstBufSize[2] =  dstSize1;
                        fDstSize = fDecDstBufSize[0];      }
    // set additional brz actor, should check if valid factor
    void setBRZ(JUINT32 brz){ fBRZ = brz; }
    void setTileBufNum(JUINT32 ringBufRowNum){ fDstTileBufRowNum = ringBufRowNum ;}
    bool setFirstDecRow(JUINT32 rowNum){fFirstDecRowNum = rowNum ; return true ;}
    
    bool setQuality(JUINT32 level){ fQuality = level ; 
                                    return true; }
    
    //JPEG_DEC_RESULT JPEG_PMEM_ALLOC( unsigned char *mva, unsigned int *pa, int *FD, unsigned int size, unsigned int lineNo);

    
private:
    bool onStart();
    bool checkParam();
    bool islock;
    bool alloc_pmem();
    bool alloc_m4u();
    bool free_m4u();
    
    bool create();
    

    JUINT32 fDecType ;
    
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

    JUINT32 fDecOutBufAddrPA0[3] ;
    JUINT32 fDecOutBufAddrPA1[3] ;
    JUINT32 setPA_flag ;
    
    //for tile
    
    JUINT32 fMcuRow ;
    JUINT32 fMcuHeightY ;
    JUINT32 fMcuHeightC ;
    
    JUINT32 fMemType;
    JUINT32 fMemTypeDefault;
    

    unsigned char *fSrcPmemVA;
    unsigned char *fDstPmemVA;
    unsigned char *fDstCbPmemVA;
    unsigned char *fDstCrPmemVA;
    int fSrcPmemFD;
    int fDstPmemFD;
    int fDstCbPmemFD;
    int fDstCrPmemFD;
    JUINT32 fDecSrcPmemAddrPA ;
    JUINT32 fDecDstPmemAddrPA0[3] ;
    
    //MTKM4UDrv *pM4uDrv;
    //M4U_MODULE_ID_ENUM fm4uJpegDecID ; 
    unsigned int fSrcMVA;
    JUINT32 fDecSrcM4UAddrMva ;
    JUINT32 fDecDstM4UAddrMva0[3];
    JUINT32 fDecDstM4UAddrMva1[3];
 
 
    /* libjpeg sw codec parameter */     
    jpeg_decompress_struct  *cinfo;
    swjpeg_error_mgr        sw_err;
    swjpeg_source_mgr *jpegStream;
    SwStream* srcStream  ;
    jpeg_error_mgr jerr;
    jpeg_source_mgr stream ;    
    
    JUINT32 fQuality ;

    unsigned char *rowptr ;
    INT32 bpr ;
    JUINT32 fImgFormat;
    JUINT32 f420to444;
    
    int decID;
};





#endif 