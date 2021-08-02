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
  
#define MTK_LOG_ENABLE 1
#include <stdio.h>
#include <cutils/log.h>
#include <utils/Errors.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "Trace.h" 


#include "jpeg_dec_hal.h"
#include "jpeg_dec_comp.h"

#include "img_dec_comp.h"   

#include "img_mmu.h"



//#define ATRACE_TAG ATRACE_TAG_GRAPHICS

#define USE_PMEM

#ifdef USE_PMEM
#include <cutils/pmem.h>

#endif

#ifdef LOG_TAG
#undef LOG_TAG
#endif
 
//#define JPEG_PROFILING   

#define LOG_TAG "JpgDecComp"

#define JPG_LOG(fmt, arg...)    ALOGW(fmt, ##arg)
#define JPG_DBG(fmt, arg...)    ALOGD(fmt, ##arg)
//#define JPG_DBG(fmt, arg...)


#define JPEG_IRQ_TIMEOUT_DEC 3000 /*5000*/  /*3000*/

#define IMG_COMP_NOT_SUPPORT_GRAY

static bool fail_return(const char msg[]) {
#if 1
    JPG_LOG("::[CODE_RETURN] - %s", msg);
#endif
    return false;   // must always return false
}


static int code_return(const char msg[], int line,int code) {
#if 1
    JPG_DBG("::[CODE_RETURN] - %s, L:%d!!\n", msg, line);
#endif
    return code;   // must always return code
}

//#define CHECK_FAIL(check, action)   
//{
//   if(!check)
//     action;   
//}


JpgDecComp::JpgDecComp()
{
    JPG_DBG("JpgDecComp::JpgDecComp");

      ATRACE_CALL();
#if 1


    
    fBRZ      = 0;
    fMinLumaBufSize = 0;
    fMinCbCrBufSize = 0;

    fMinLumaBufStride = 0;
    fMinCbCrBufStride = 0;
    
    fLumaImgStride = 0;
    fCbCrImgStride = 0;
    fLumaImgHeight = 0;
    fCbCrImgHeight = 0;
    
    
    
    
    
    fOutFormat = 0;

    
    

    fSrcBufferSize = 0;    
    fSrcBsSize     = 0;
    fDstSize       = 0;

    fSrcConfigAddr = 0;
    fDstConfigAddr = 0;
    

    setPA_flag  = 0;;
    
    
    fMcuHeightY = 0;
    fMcuHeightC = 0;
    
    

    fSrcPmemVA   = NULL;
    fDstPmemVA   = NULL;
    fDstCbPmemVA = NULL;
    fDstCrPmemVA = NULL;
    fSrcPmemFD   = 0;
    fDstPmemFD   = 0;
    fDstCbPmemFD = 0;
    fDstCrPmemFD = 0;
    fDecSrcPmemAddrPA  =0;
    
    
    fSrcMVA = 0;
    fDecSrcM4UAddrMva = 0;
    
    
    decID =0;

#endif    
    


    //fMemType = fMemTypeDefault = IMG_MEM_TYPE_PHY ;
    //fMemType = fMemTypeDefault = IMG_MEM_TYPE_PMEM ;
    fMemType = fMemTypeDefault = IMG_MEM_TYPE_M4U ;
    fm4uJpegDecID = M4U_CLNTMOD_JPGDEC ;    

    fSrcPmemVA = NULL;
    fDstPmemVA = NULL;
    fDstCbPmemVA = NULL;
    fDstCrPmemVA = NULL;
    pM4uDrv = NULL ;
    fSrcMVA = 0;


    islock = false;
    fDecRowCnt = 0;
    fDstTileBufRowNum =  0;
    fFirstDecRowNum = 1;
    fDecType = IMG_DEC_MODE_NONE ;
    fOutWidth = fOutHeight = 0;
    fJpgWidth = fJpgHeight = 0;
    fSrcAddr = fDstAddr = NULL;

    fSrcAddrPA = NULL;
    for(int i =0; i<3;i++){
      fDecOutBufAddrPA0[i] = NULL ;
      fDecOutBufAddrPA1[i] = NULL ;
      fDecDstBufAddr0[i] = 0 ; //set HW
      fDecDstBufAddr1[i] = 0 ; //set HW   
      fDecDstBufSize[i]  = 0 ; 
      fDecOutBufAddr0[i] = 0 ;
      fDecOutBufAddr1[i] = 0 ; 
      fDstBufStride[i] = 0;
      fDecDstPmemAddrPA0[i] = 0;
      fDecDstM4UAddrMva0[i] = 0;
      fDecDstM4UAddrMva1[i] = 0;

    }

    fMcuRow = 0;

    fLumaImgHeight = 0;    
    fCbCrImgHeight = 0;  
    
    //memset( &info, 0, sizeof(JPEG_FILE_INFO_IN));    
    memset( &t3, 0, sizeof(timeval));
    
    

      
    
}

#define JPEG_PMEM_ALLOC(size, mva, pa, fd)  \
{ \
   if(size == 0){ \
       JPG_DBG("Allocate Pmem with zero size %d, L:%d!!\n", size, __LINE__);  \
       return false; \
   } \
    mva = (unsigned char *)pmem_alloc_sync(size, &fd);   \
    if(mva == NULL) {   \
        JPG_DBG("Can not allocate PMEM, L:%d!!\n", __LINE__);  \
        return false ;  \
    } \
    pa = (JUINT32)pmem_get_phys(fd);       \
    JPG_DBG("Allocate Pmem, va:0x%x, pa:0x%x, size:%x, L:%d!!\n", mva, pa, size, __LINE__);  \
    memset(mva, 0x00, sizeof(char)*size);    \
}


#define JPEG_PMEM_FREE(va, size, fd) \
{  \
   if(va != NULL) { \
       pmem_free(va, size, fd); \
       va = NULL;   \
   }  \
}
 
   
JpgDecComp::~JpgDecComp()
{
    JPG_DBG("JpgDecComp::~JpgDecComp");

    unlock();


    
}

//void JpgDecComp::setRangeDecode(JUINT32 left, JUINT32 top, JUINT32 right, JUINT32 bottom)
//{
//    isRangeDecode = true;
//    fLeft = left;
//    fTop = top;
//    fRight = right;
//    fBottom = bottom;
//}
 

bool JpgDecComp::lock()
{
    JPG_DBG("lock!!\n");
    ATRACE_CALL();
    if(JPEG_DEC_STATUS_OK != jpegDecLockDecoder(&decID)) {
        return fail_return("Jpeg decoder resource is busy");
    }

    islock = true;
    return true;
}

bool JpgDecComp::unlock()
{
    if(islock)
    {   
      JPG_DBG("unlock!!\n");
      ATRACE_CALL();
        jpegDecUnlockDecoder(decID);

        if(fMemType == IMG_MEM_TYPE_PHY){    
            JPEG_PMEM_FREE(  fSrcPmemVA,    fSrcBsSize,   fSrcPmemFD);
        }else if(fMemType == IMG_MEM_TYPE_PMEM ){
            JPEG_PMEM_FREE(  fSrcPmemVA,    fSrcBsSize,   fSrcPmemFD);
            JPEG_PMEM_FREE(  fDstPmemVA, fDecDstBufSize[0],   fDstPmemFD);
            JPEG_PMEM_FREE(fDstCbPmemVA, fDecDstBufSize[1], fDstCbPmemFD);
            JPEG_PMEM_FREE(fDstCrPmemVA, fDecDstBufSize[2], fDstCrPmemFD);
        }else if(fMemType == IMG_MEM_TYPE_M4U ){
          
           if(!free_m4u())
             JPG_DBG("free M4U fail, L:%d!!\n", __LINE__);
        
        }
        
        islock = false;
    }
    
    return true;
}

bool JpgDecComp::checkParam()
{
    //JPG_DBG("checkParam!!\n");
    if(fJpgWidth == 0 || fJpgHeight == 0)
    {
        JPG_LOG("Invalid JPEG width/height %u/%u", fJpgWidth, fJpgHeight);
        return false;
    }

#if 0    
    if(fOutWidth < 3 || fOutHeight < 3)
    {
        JPG_LOG("Invalid width/height %u/%u [Range 3~4095]", fOutWidth, fOutHeight);
        return false;
    }

    if(fOutWidth > 4095 || fOutHeight > 4095)
    {
        JPG_LOG("Invalid width/height %u/%u [Range 3~4095]", fOutWidth, fOutHeight);
        return false;
    }

    if(fSrcAddr == NULL || fDstAddr == NULL) 
    {
        return fail_return("Invalid Address");
    }
#endif
    return true;
}


bool JpgDecComp::parse()
{
    //JPG_DBG("parse!!\n");
    if(fSrcAddr == NULL)
    {
        return fail_return("source address is null");
    }

    if(JPEG_DEC_STATUS_OK != jpegDecSetSourceFile(decID, fSrcAddr, fSrcBsSize))
    {
        return false;
    }
    JPEG_FILE_INFO_IN info;
    if(JPEG_DEC_STATUS_OK != jpegDecGetFileInfo(decID, &info))
    {
        return false; 
    }

    fJpgWidth = info.width;
    fJpgHeight = info.height;
    fMcuRow = info.McuRow ;
    

#if 1

    if(info.samplingFormat == JPEG_DEC_YUV_420){
      fOutFormat = IMGDEC_3PLANE_YUV420 ;
    }else if(info.samplingFormat == JPEG_DEC_YUV_422){
      fOutFormat = IMGDEC_3PLANE_YUV422 ;
    }else if(info.samplingFormat == JPEG_DEC_YUV_GRAY){
      fOutFormat = IMGDEC_3PLANE_GRAY ;
    }else{
      fOutFormat = IMGDEC_3PLANE_NONE ;
    }    
    JPG_DBG("COMP_PARSE sfmt %d, Outfmt %d, w %d, h %d, row %d, vSample %d %d!!\n", info.samplingFormat, fOutFormat,info.width, info.height, info.McuRow, info.y_vSamplingFactor, info.u_vSamplingFactor);    
    
#else    
    fMcuHeightY = info.y_vSamplingFactor << 3;
    if(info.samplingFormat == JPEG_DEC_YUV_GRAY)
      fMcuHeightC = 8;
    else
      fMcuHeightC = info.u_vSamplingFactor << 3;

    fLumaImgHeight = TO_CEIL(fJpgHeight, 4);    
    
    if(info.samplingFormat == JPEG_DEC_YUV_420){
      fLumaImgStride = TO_CEIL(fJpgWidth, 4); 
      fCbCrImgStride = fLumaImgStride/2;
      fOutFormat = IMGDEC_3PLANE_YUV420 ;
      fCbCrImgHeight = TO_CEIL(fLumaImgHeight/2, 2);
      
    }else if(info.samplingFormat == JPEG_DEC_YUV_422){
      fLumaImgStride = TO_CEIL(fJpgWidth, 4); 
      fCbCrImgStride = fLumaImgStride/2;      
      fOutFormat = IMGDEC_3PLANE_YUV422 ;
      fCbCrImgHeight = TO_CEIL(fLumaImgHeight, 2); 
    }else if(info.samplingFormat == JPEG_DEC_YUV_GRAY){
      fLumaImgStride = TO_CEIL(fJpgWidth, 4); 
      fCbCrImgStride = fLumaImgStride/2;
      fOutFormat = IMGDEC_3PLANE_GRAY ;
      fCbCrImgHeight = TO_CEIL(fLumaImgHeight, 2); 
    }else{
      fOutFormat = IMGDEC_3PLANE_NONE ;
    }
    JPG_DBG("COMP_PARSE sfmt %d, Outfmt %d, w %d, h %d, row %d, McuHeight %d %d, H %d %d!!\n ", info.samplingFormat, fOutFormat,info.width, info.height, info.McuRow, fMcuHeightY, fMcuHeightC, fLumaImgHeight, fCbCrImgHeight);    
#endif

    

    if(fOutFormat == IMGDEC_3PLANE_NONE )    
      return false ;
    else  
      return true;
}

//bool JpgDecComp::ResumeDecRow(JUINT32 decRowNum)
JUINT32 JpgDecComp::ResumeDecRow(JUINT32 decRowNum)
{
   JUINT32 ret ;
   //ATRACE_CALL();
   
    //JPG_DBG("ResumeDecRow!!\n"); 

    // Trigger & Wait JPEG Decoder
    // Set Timeout accroding to image pixels
    JPEG_DEC_RESULT_ENUM result;
    long time_out;
    time_out = (fJpgWidth * fJpgHeight * 100) / (1024 * 1024);

    if(time_out > 3000) time_out = 3000;
    if(time_out < 100) time_out = 100;


    if(decRowNum == 0){
      JPG_DBG("jpeg decoder resume %d+%d->%d !!\n ", fDecRowCnt, decRowNum,fDecRowCnt+decRowNum);       
      return IMG_DEC_RST_DONE ;
    }
      
       
      
    if( fDecType != IMG_DEC_MODE_TILE  ){
      JPG_DBG("JpgDecComp::ResumeDecRow decodeMode is not ROW_MODE %d!!\n", fDecType );      
      return IMG_DEC_RST_DONE ;
    }
#if 1
    time_out  =  JPEG_IRQ_TIMEOUT_DEC ;
#endif

#ifdef JPEG_PROFILING
      gettimeofday(&t1, NULL); 
#endif
      if(JPEG_DEC_STATUS_OK != jpegDecResumeRow(decID, decRowNum,time_out, &result)) 
      {
          //fail_return("JPEG Decoder Start fail");
          //return IMG_DEC_RST_DECODE_FAIL ;
          return code_return("JPEG Decoder Start fail, ",__LINE__, IMG_DEC_RST_DECODE_FAIL);
      }
#ifdef JPEG_PROFILING      
      gettimeofday(&t2, NULL); 
      t3.tv_sec  += t2.tv_sec  - t1.tv_sec ;
      t3.tv_usec += t2.tv_usec - t1.tv_usec ;
#endif
      
    if(fMemType == IMG_MEM_TYPE_PHY){

    }else if(fMemType == IMG_MEM_TYPE_PMEM){
         memcpy((unsigned char*)fDecOutBufAddr0[0]+ fDecRowCnt*fMinLumaBufSize ,   fDstPmemVA+ fDecRowCnt*fMinLumaBufSize, decRowNum * fMinLumaBufSize);
         memcpy((unsigned char*)fDecOutBufAddr0[1]+ fDecRowCnt*fMinCbCrBufSize , fDstCbPmemVA+ fDecRowCnt*fMinCbCrBufSize, decRowNum * fMinCbCrBufSize);
         memcpy((unsigned char*)fDecOutBufAddr0[2]+ fDecRowCnt*fMinCbCrBufSize , fDstCrPmemVA+ fDecRowCnt*fMinCbCrBufSize, decRowNum * fMinCbCrBufSize);
    }else if(fMemType == IMG_MEM_TYPE_M4U){
      
      
    }


    JPG_DBG("jpeg decoder resume %d+%d->%d result:%d ", fDecRowCnt, decRowNum,fDecRowCnt+decRowNum, result);     
    fDecRowCnt +=  decRowNum ;
    
 

     
    
    //if(JPEG_DEC_RESULT_EOF_DONE != result)
    //{
    //    fail_return("JPEG Decode Fail");
    //    return IMG_DEC_RST_DECODE_FAIL ;
    //}
   
   if( fDecRowCnt < fMcuRow && result == JPEG_DEC_RESULT_PAUSE )
      ret = IMG_DEC_RST_ROW_DONE ; 
   else
      ret = IMG_DEC_RST_DONE ;

#ifdef JPEG_PROFILING 
   if(ret == IMG_DEC_RST_DONE ){
      JPG_DBG("::[DECTIME] DecComp type %d, rst %d, time %u, pixels %dK(%d %d)!!\n",
        fDecType, ret ,(t3.tv_sec)*1000000 + (t3.tv_usec)
       , fJpgWidth*fJpgHeight/(1024), fJpgWidth, fJpgHeight);              
       //printf("go SUCCESS %d !!\n", __LINE__);      
   }
#endif
   
   return ret ;
   
}



bool JpgDecComp::Config(JUINT32 decMode)
{
   
   fDecType = decMode;
   unsigned char horDecimate[2]  ;
   unsigned char verDecimate[2]  ;
   unsigned int i = 0;

    JPEG_DEC_HAL_MODE decHalMode    ;
    JPEG_MIN_BUF_INFO min_info ;
    
    //unsigned int ds_width = fJpgWidth >> fBRZ ;
    //unsigned int ds_height = fJpgHeight >> fBRZ ;
    unsigned int ds_align_pixel = (fBRZ == 3)? 8 : 4;
    //JPG_DBG("Config!!\n");

    switch(fDecType)
    {
      case IMG_DEC_MODE_NONE:
          decHalMode = JPEG_DEC_HAL_NONE; break;
      case IMG_DEC_MODE_FRAME:
         decHalMode = JPEG_DEC_HAL_FRAME; break;
      case IMG_DEC_MODE_LINK: 
         decHalMode = JPEG_DEC_HAL_DC_LINK;  break;
      case IMG_DEC_MODE_TILE: 
         decHalMode = JPEG_DEC_HAL_MCU_ROW;  break;
      
      default:
         decHalMode = JPEG_DEC_HAL_NONE; break;
    }

    JPEG_FILE_INFO_IN info;
    if(JPEG_DEC_STATUS_OK != jpegDecGetFileInfo(decID, &info))
    {
        return false;
    }

    
    for(i = 0; i<2;i++){
      horDecimate[i] = fBRZ ;
      verDecimate[i] = fBRZ ;
    }


    fMcuHeightY = info.y_vSamplingFactor << (3 - fBRZ);
    if(info.samplingFormat == JPEG_DEC_YUV_GRAY)
      fMcuHeightC = fMcuHeightY; //8
    else
      fMcuHeightC = info.u_vSamplingFactor << (3 - fBRZ);


    fLumaImgStride = TO_CEIL(fJpgWidth, ds_align_pixel); 
    fLumaImgStride >>= fBRZ ; 
    fCbCrImgStride = fLumaImgStride/2;

    fLumaImgHeight = TO_CEIL(fJpgHeight, ds_align_pixel);    
    fLumaImgHeight >>= fBRZ ; 
    
    if(fOutFormat == IMGDEC_3PLANE_YUV420){
      fCbCrImgHeight = TO_CEIL(fLumaImgHeight/2, 2);      
    }else if( fOutFormat == IMGDEC_3PLANE_YUV422){
      fCbCrImgHeight = TO_CEIL(fLumaImgHeight, 2); 
    }else if(fOutFormat == IMGDEC_3PLANE_GRAY){
#ifdef IMG_COMP_NOT_SUPPORT_GRAY
      if(decHalMode == JPEG_DEC_HAL_MCU_ROW ){
         JPG_DBG("JPEG_CONFIG: Current not support MCUROW gray!!\n");
         return false ;
      }
#endif        
      fCbCrImgHeight = TO_CEIL(fLumaImgHeight, 2); 
    }
    //else{
    //  //fOutFormat = IMGDEC_3PLANE_NONE ;
    //}


    JPG_DBG("JPEG_CONFIG: fmt %d, BRZ %d, dsap %d, MCU_H %d %d, imgStride %d %d, imgH %d %d!!\n", fOutFormat,fBRZ, ds_align_pixel, fMcuHeightY, fMcuHeightC, fLumaImgStride, fCbCrImgStride, fLumaImgHeight, fCbCrImgHeight);

    


      

   if(JPEG_DEC_STATUS_OK != jpegComputeDstBuffer(decID, &min_info, decHalMode, horDecimate, verDecimate))
   {
       return false;
   }
   
   
   fMinLumaBufSize   =  min_info.minLumaBufSize   ;
   fMinCbCrBufSize   =  min_info.minCbCrBufSize   ;
   fMinLumaBufStride =  min_info.minLumaBufStride ;
   fMinCbCrBufStride =  min_info.minCbCrBufStride ;
   //fLumaImgStride    =  min_info.ds_lumaImgStride ;
   //fCbCrImgStride    =  min_info.ds_cbcrImgStride ;
   
   
   
   return true ;
   
}

bool JpgDecComp::onStart()
{
    //JPG_DBG("onStart!!\n");
    ATRACE_CALL();

    JPEG_DEC_CONFIG dec_config ;

    JPEG_FILE_INFO_IN info;
    if(JPEG_DEC_STATUS_OK != jpegDecGetFileInfo(decID, &info))
    {
        return false;
    }
   
    if(fFirstDecRowNum == 0) fFirstDecRowNum = 1 ;      
    fDecRowCnt = fFirstDecRowNum ;

    
    for(int comp=0;comp<3;comp++){
      dec_config.decDstBufAddr0[comp] = fDecDstBufAddr0[comp];
      dec_config.decDstBufAddr1[comp] = fDecDstBufAddr1[comp];
    }
    dec_config.tileBufRowNum = fDstTileBufRowNum ;
    dec_config.srcVA = fSrcConfigAddr ;      
    dec_config.srcSize = fSrcBsSize ;    
    
    dec_config.firstDecRowNum = fFirstDecRowNum ;
    dec_config.dstBufStride[0] = fMinLumaBufStride;
    dec_config.dstBufStride[1] = dec_config.dstBufStride[2] = fMinCbCrBufStride ;

    if(JPEG_DEC_STATUS_OK != jpegDecSetSourceSize(decID, &dec_config)) 
    {
        return fail_return("JPEG Configure Driver fail");    
    }




    // Trigger & Wait JPEG Decoder
    // Set Timeout accroding to image pixels
    JPEG_DEC_RESULT_ENUM result;
    long time_out;
    time_out = ((fJpgWidth/1024) * (fJpgHeight/1024) * 100) ;/// (1024 * 1024);
#if 1    
    if(time_out > 3000) time_out = 3000;
    if(time_out < 100) time_out = 100;
      

    time_out  =  JPEG_IRQ_TIMEOUT_DEC ;
#endif

#ifdef JPEG_PROFILING
      gettimeofday(&t1, NULL);     
#endif      
      if(JPEG_DEC_STATUS_OK != jpegDecStart(decID, time_out, &result)) 
      {
          return fail_return("JPEG Decoder Start fail");
          //return IMG_DEC_RST_DECODE_FAIL ;
      }
#ifdef JPEG_PROFILING      
      gettimeofday(&t2, NULL);  

      if(fDecType == IMG_DEC_MODE_LINK){
          JPG_DBG("::[DECTIME] DecComp type %d, rst %d, time %u, pixels %dK(%d %d)!!\n",
            fDecType, result ,(t2.tv_sec - t1.tv_sec)*1000000 + (t2.tv_usec - t1.tv_usec)
           , fJpgWidth*fJpgHeight/(1024), fJpgWidth, fJpgHeight);       
           //printf("go SUCCESS %d !!\n", __LINE__);        
      }else if(fDecType == IMG_DEC_MODE_TILE){
        t3.tv_sec  += t2.tv_sec  - t1.tv_sec ;
        t3.tv_usec += t2.tv_usec - t1.tv_usec ;
        JPG_DBG("jpeg decoder result:%d ", result);
      }
#endif      
     
    
    if(JPEG_DEC_RESULT_EOF_DONE != result && result != JPEG_DEC_RESULT_PAUSE)
    {
        return fail_return("JPEG Decode Fail");
        //return IMG_DEC_RST_DECODE_FAIL ;
    }
   

    return true;

}

bool JpgDecComp::alloc_pmem()
{


       // src bitstream memory already allocate in skia level
       // source buffer pmem alloc
       JPEG_PMEM_ALLOC( fSrcBsSize, fSrcPmemVA, fDecSrcPmemAddrPA, fSrcPmemFD ) ;
       memcpy((void*)fSrcPmemVA, fSrcAddr, fSrcBsSize);

       //Y output buffer
       JPEG_PMEM_ALLOC( fDecDstBufSize[0], fDstPmemVA, fDecDstPmemAddrPA0[0], fDstPmemFD ) ;
       
       //allocate Cb output buffer 
       JPEG_PMEM_ALLOC( fDecDstBufSize[1], fDstCbPmemVA, fDecDstPmemAddrPA0[1], fDstCbPmemFD ) ;
       
       //allocate Cr output buffer 
       JPEG_PMEM_ALLOC( fDecDstBufSize[2], fDstCrPmemVA, fDecDstPmemAddrPA0[2], fDstCrPmemFD ) ;

      
      
      return true;
      
}



#if 0
bool JpgDecComp::alloc_m4u()
{

#ifdef JPEG_PROFILING
    struct timeval t11, t22;
#endif   

   pM4uDrv = new MTKM4UDrv();
   fSrcMVA = 0;
   if (pM4uDrv == NULL) {
       JPG_LOG("new MTKM4UDrv() fail \n");
   }         
      
#ifdef JPEG_PROFILING
  gettimeofday(&t11, NULL);
#endif
    if(M4U_STATUS_OK != pM4uDrv->m4u_reset_mva_release_tlb(M4U_CLNTMOD_JPEG_DEC))
    {
        fSrcMVA = 0;
        return fail_return("Can not reset m4u mva release tlb\n");        
    }
    
    if(M4U_STATUS_OK != pM4uDrv->m4u_alloc_mva(M4U_CLNTMOD_JPEG_DEC, (unsigned int)fSrcAddr , fSrcBufferSize, &fSrcMVA))
    {
        fSrcMVA = 0;
        return fail_return("Can't allocate mva\n");       
    }
    
    if(M4U_STATUS_OK != pM4uDrv->m4u_insert_tlb_range(M4U_CLNTMOD_JPEG_DEC, fSrcMVA, fSrcMVA + fSrcBufferSize - 1, RT_RANGE_HIGH_PRIORITY, 1))
    {
        pM4uDrv->m4u_dealloc_mva(M4U_CLNTMOD_JPEG_DEC, (unsigned int)fSrcAddr , fSrcBufferSize, fSrcMVA);
        fSrcMVA = 0;
        return fail_return("Can't insert tlb range\n"); 
    }
            
    fSrcConfigAddr = fSrcMVA;
    
    M4U_PORT_STRUCT m4uPort;
    
    m4uPort.ePortID = M4U_PORT_JPEG_DEC;
    m4uPort.Virtuality = 1;
    m4uPort.Security = 0;
    m4uPort.Distance = 1;
    m4uPort.Direction = 0;
    
    if(M4U_STATUS_OK != pM4uDrv->m4u_config_port(&m4uPort))
    {
        return fail_return("Can config m4u port\n");         
    }
    
    pM4uDrv->m4u_cache_sync(M4U_CLNTMOD_JPEG_DEC, M4U_CACHE_FLUSH_BEFORE_HW_READ_MEM, (unsigned int)(void *)fSrcAddr, fSrcBufferSize);
    
#ifdef JPEG_PROFILING
  gettimeofday(&t22, NULL);
  JPG_LOG("Jpeg HW config m4u time : %u, (%dx%d)", (t22.tv_sec - t11.tv_sec)*1000000 + (t22.tv_usec - t11.tv_usec), fJpgWidth, fJpgHeight);
#endif
          
   
   
   
   return true;   
}
#endif

bool JpgDecComp::alloc_m4u()
{
    unsigned int en_mci = 0; 

    //JPG_DBG("alloc_m4u!!\n");
#ifdef JPEG_PROFILING
    struct timeval t11, t22;   
    gettimeofday(&t11, NULL);
#endif

	  android::Tracer::traceBegin(ATRACE_TAG_GRAPHICS,"DEC_M4U_ALLOC");

    if(!imgMmu_create(&pM4uDrv, fm4uJpegDecID))
      return false; 
          
    if(!imgMmu_reset(&pM4uDrv, fm4uJpegDecID))
      return false;       

    //src bitstream
    if(en_mci){
      if(!imgMmu_alloc_pa_mci(&pM4uDrv, fm4uJpegDecID, (JUINT32) fSrcAddr, fSrcBufferSize, &fDecSrcM4UAddrMva))
        return false;          
    }else{
      if(!imgMmu_alloc_pa(&pM4uDrv, fm4uJpegDecID, (JUINT32) fSrcAddr, fSrcBufferSize, &fDecSrcM4UAddrMva))
        return false;    
    }
    
    //dst bank 0 Luma, Cb, Cr
    if(!imgMmu_alloc_pa(&pM4uDrv, fm4uJpegDecID, (JUINT32) fDecOutBufAddr0[0], fDecDstBufSize[0], &fDecDstM4UAddrMva0[0]))
      return false;
    if(!imgMmu_alloc_pa(&pM4uDrv, fm4uJpegDecID, (JUINT32) fDecOutBufAddr0[1], fDecDstBufSize[1], &fDecDstM4UAddrMva0[1]))
      return false;
    if(!imgMmu_alloc_pa(&pM4uDrv, fm4uJpegDecID, (JUINT32) fDecOutBufAddr0[2], fDecDstBufSize[2], &fDecDstM4UAddrMva0[2]))
      return false;

    if(fDecType == IMG_DEC_MODE_LINK){
      
      //dst bank 1 Luma, Cb, Cr
      if(!imgMmu_alloc_pa(&pM4uDrv, fm4uJpegDecID, (JUINT32) fDecOutBufAddr1[0], fDecDstBufSize[0], &fDecDstM4UAddrMva1[0]))
        return false;
      if(!imgMmu_alloc_pa(&pM4uDrv, fm4uJpegDecID, (JUINT32) fDecOutBufAddr1[1], fDecDstBufSize[1], &fDecDstM4UAddrMva1[1]))
        return false;
      if(!imgMmu_alloc_pa(&pM4uDrv, fm4uJpegDecID, (JUINT32) fDecOutBufAddr1[2], fDecDstBufSize[2], &fDecDstM4UAddrMva1[2]))
        return false;      
      
    }
    
    //config module port
    if(en_mci)
      imgMmu_cfg_port_mci(pM4uDrv, fm4uJpegDecID, M4U_PORT_JPGDEC_BSDMA);
    else
      imgMmu_cfg_port(pM4uDrv, fm4uJpegDecID, M4U_PORT_JPGDEC_BSDMA);
      
    imgMmu_cfg_port(pM4uDrv, fm4uJpegDecID, M4U_PORT_JPGDEC_WDMA);
    
    //flush cache
    if(!en_mci){
      imgMmu_sync(pM4uDrv, fm4uJpegDecID, (JUINT32) fSrcAddr, fSrcBufferSize, SYNC_HW_READ);    
    }
    
    //if(!en_mci)
    {
      
      imgMmu_sync(pM4uDrv, fm4uJpegDecID, (JUINT32) fDecOutBufAddr0[0], fDecDstBufSize[0], SYNC_HW_WRITE);    
      imgMmu_sync(pM4uDrv, fm4uJpegDecID, (JUINT32) fDecOutBufAddr0[1], fDecDstBufSize[1], SYNC_HW_WRITE);    
      imgMmu_sync(pM4uDrv, fm4uJpegDecID, (JUINT32) fDecOutBufAddr0[2], fDecDstBufSize[2], SYNC_HW_WRITE);
      
      if(fDecType == IMG_DEC_MODE_LINK){
        imgMmu_sync(pM4uDrv, fm4uJpegDecID, (JUINT32) fDecOutBufAddr1[0], fDecDstBufSize[0], SYNC_HW_WRITE);    
        imgMmu_sync(pM4uDrv, fm4uJpegDecID, (JUINT32) fDecOutBufAddr1[1], fDecDstBufSize[1], SYNC_HW_WRITE);    
        imgMmu_sync(pM4uDrv, fm4uJpegDecID, (JUINT32) fDecOutBufAddr1[2], fDecDstBufSize[2], SYNC_HW_WRITE);      
      }
    }
#ifdef JPEG_PROFILING    
     gettimeofday(&t22, NULL);
     JPG_LOG("JpegCompM4U (%dx%d), time : %u!!\n", fJpgWidth, fJpgHeight, (t22.tv_sec - t11.tv_sec)*1000000 + (t22.tv_usec - t11.tv_usec));
#endif
	  android::Tracer::traceEnd(ATRACE_TAG_GRAPHICS);   
   
   return true;
}

bool JpgDecComp::free_m4u()
{
   

   if(pM4uDrv)
   {
      JPG_DBG("free_m4u!!\n");
      imgMmu_dealloc_pa(pM4uDrv, M4U_CLNTMOD_JPGDEC, (JUINT32) fSrcAddr           , fSrcBufferSize   , &fDecSrcM4UAddrMva       );
      imgMmu_dealloc_pa(pM4uDrv, M4U_CLNTMOD_JPGDEC, (JUINT32) fDecOutBufAddr0[0] , fDecDstBufSize[0], &fDecDstM4UAddrMva0[0]   );
      imgMmu_dealloc_pa(pM4uDrv, M4U_CLNTMOD_JPGDEC, (JUINT32) fDecOutBufAddr0[1] , fDecDstBufSize[1], &fDecDstM4UAddrMva0[1]   );
      imgMmu_dealloc_pa(pM4uDrv, M4U_CLNTMOD_JPGDEC, (JUINT32) fDecOutBufAddr0[2] , fDecDstBufSize[2], &fDecDstM4UAddrMva0[2]   );
      if( fDecType == IMG_DEC_MODE_LINK ){
        imgMmu_dealloc_pa(pM4uDrv, M4U_CLNTMOD_JPGDEC, (JUINT32) fDecOutBufAddr1[0] , fDecDstBufSize[0], &fDecDstM4UAddrMva1[0]   );
        imgMmu_dealloc_pa(pM4uDrv, M4U_CLNTMOD_JPGDEC, (JUINT32) fDecOutBufAddr1[1] , fDecDstBufSize[1], &fDecDstM4UAddrMva1[1]   );
        imgMmu_dealloc_pa(pM4uDrv, M4U_CLNTMOD_JPGDEC, (JUINT32) fDecOutBufAddr1[2] , fDecDstBufSize[2], &fDecDstM4UAddrMva1[2]   );
      }
      
      delete pM4uDrv;
   }   
   pM4uDrv = NULL;
   
 
   
   return true; 
}   



//bool JpgDecComp::Start()
JUINT32 JpgDecComp::Start()
{
    //JPG_DBG("Start!!\n");
    //Check param
    if(true != checkParam()) {
        return IMG_DEC_RST_CFG_ERR;
    }
    ATRACE_CALL();

    //JPG_DBG("JpgDecComp::start -> config jpeg path");
    JPG_DBG("Decoder Src Addr:0x%x, size %x, width/height:[%u, %u]           ", (unsigned int)fSrcAddr, fSrcBufferSize, fJpgWidth, fJpgHeight);
    JPG_DBG("Decoder Dst Addr:0x%x, size %d, width/height:[%u, %u], format:%u", (unsigned int)fDstAddr, fDecDstBufSize[0], fOutWidth, fOutHeight, fOutFormat);
    //JPG_DBG("Decoder Dither:%d, RangeDecode:%d [%d %d %d %d]", isDither, isRangeDecode, fLeft, fTop, fRight, fBottom);

     if( fMemType == IMG_MEM_TYPE_PHY ){
       // source buffer pmem alloc
       JPEG_PMEM_ALLOC( fSrcBsSize, fSrcPmemVA, fSrcConfigAddr, fSrcPmemFD ) ;
       memcpy((void*)fSrcPmemVA, fSrcAddr, fSrcBsSize);
       
       JPG_DBG("Allocate Source Pmem, va:0x%x, pa:0x%x, size:%u", fSrcPmemVA, fSrcConfigAddr, fSrcBsSize);
       for( int i = 0;i<3; i++){
          fDecDstBufAddr0[i] = fDecOutBufAddrPA0[i] ;
          fDecDstBufAddr1[i] = fDecOutBufAddrPA1[i] ;
       }
     }else if(fMemType == IMG_MEM_TYPE_PMEM){
       if(!alloc_pmem())
          return code_return(" allocate PMEM fail, ",__LINE__, IMG_DEC_RST_CFG_ERR);
       
       fSrcConfigAddr = fDecSrcPmemAddrPA;
       for(unsigned int i=0; i<3;i++)
        fDecDstBufAddr0[i] = fDecDstPmemAddrPA0[i];
       
       
     }else if(fMemType == IMG_MEM_TYPE_M4U){
     
        if(!alloc_m4u())
           return code_return(" allocate M4U fail, ",__LINE__, IMG_DEC_RST_CFG_ERR);
     
        //fDstConfigAddr = (unsigned int)(void *)fDstAddr;
        fSrcConfigAddr = fDecSrcM4UAddrMva;
        for(unsigned int i=0; i<3;i++){
         fDecDstBufAddr0[i] = fDecDstM4UAddrMva0[i]; 
         fDecDstBufAddr1[i] = fDecDstM4UAddrMva1[i]; 
        }
        
     }


    if(true == onStart()) {

      if(fMemType == IMG_MEM_TYPE_PMEM){
            memcpy((unsigned char*)fDecOutBufAddr0[0], fDstPmemVA, fDecDstBufSize[0]);
            memcpy((unsigned char*)fDecOutBufAddr0[1], fDstCbPmemVA, fDecDstBufSize[1]);
            memcpy((unsigned char*)fDecOutBufAddr0[2], fDstCrPmemVA, fDecDstBufSize[2]);
  #if 0
            {
              FILE *fp;
              unsigned char* targetAddr[3];
              unsigned char* cptr ;
              fp = fopen("/data/otis/dec_hal_out.raw", "w");
            
              targetAddr[0] = fDstPmemVA ;
              targetAddr[1] = fDstCbPmemVA ;
              targetAddr[2] = fDstCrPmemVA ;
              for( int j=0;j<3;j++){ /* runcomp 0~2 */
                cptr = targetAddr[j] ;
                for( int i=0;i<fDecDstBufSize[j];i++){  /* total size in comp */
                  fprintf(fp,"%c", *cptr );  
                  cptr++;
                }
              
                fclose(fp);
                if( j==0 )
                   fp = fopen("/data/otis/dec_hal_out.cb.raw", "w"); 
                else if(j == 1)
                   fp = fopen("/data/otis/dec_hal_out.cr.raw", "w"); 
                
             
              }          
            
            
            fclose(fp);        
          }
  #endif          
      }
        

    } else {

      if(fMemType == IMG_MEM_TYPE_PMEM){
              memcpy((unsigned char*)fDecOutBufAddr0[0], fDstPmemVA, fDecDstBufSize[0]);
              memcpy((unsigned char*)fDecOutBufAddr0[1], fDstCbPmemVA, fDecDstBufSize[1]);
              memcpy((unsigned char*)fDecOutBufAddr0[2], fDstCrPmemVA, fDecDstBufSize[2]);
#if 0          
            {
              FILE *fp;
              unsigned char* targetAddr[3];
              unsigned char* cptr ;
              fp = fopen("/data/otis/dec_hal_out.raw", "w");
            
              targetAddr[0] = fDstPmemVA ;
              targetAddr[1] = fDstCbPmemVA ;
              targetAddr[2] = fDstCrPmemVA ;
              for( int j=0;j<3;j++){ /* runcomp 0~2 */
                cptr = targetAddr[j] ;
                for( int i=0;i<fDecDstBufSize[j];i++){  /* total size in comp */
                  fprintf(fp,"%c", *cptr );  
                  cptr++;
                }
              }          
              
              fclose(fp);        
            }
#endif                
      }
        
        return code_return(" JpegComp Decode fail, ",__LINE__, IMG_DEC_RST_DECODE_FAIL); 
    }
    

    return IMG_DEC_RST_DONE;
}

