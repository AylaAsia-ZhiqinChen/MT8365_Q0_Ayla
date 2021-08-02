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
#include <stdlib.h>
#include <cutils/log.h>
#include <utils/Errors.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cutils/log.h>
#include <string.h>

//#include <linux/ion.h>
#include <linux/ion_drv.h>

#include "jpeg_enc_hal.h"
#include "jpeg_hal.h"
#include "m4u_lib.h"
#include "img_mmu.h"

#include "img_common_def.h"


#ifdef JENC_LOCK_VIDEO
#include <val_types_private.h>
#include <val_api_private.h>
#include <vdec_drv_base.h>
#endif

#ifdef MTK_PLATFORM_OPTIMIZE
extern "C"
{
extern int drvb_f0 (void);
extern int drvb_f1(unsigned int mask);
}
#endif

//#define IMGMMU_SUPPORT_PMEM
#define __USE_SP_SW_JPEG_ENCODE__
#if defined(__USE_SP_SW_JPEG_ENCODE__)
extern "C" {
    #include "jpeglib.h"
    #include "jerror.h"
   }
#endif
//#define IMGMMU_SUPPORT_M4U
//#define IMGMMU_SUPPORT_ION

#ifdef IMGMMU_SUPPORT_PMEM
  #include <cutils/pmem.h>
#endif

 
#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "JpgEncHal"

#if 1
  #define JPG_LOG(fmt, arg...)    ALOGW(LOG_TAG fmt, ##arg)
  #define JPG_DBG(fmt, arg...)    ALOGD(LOG_TAG fmt, ##arg)
#else
  #define JPG_LOG printf
  #define JPG_DBG printf
#endif


#define JPEG_IRQ_TIMEOUT_ENC 3000  /*3000*/
       

#define IS_NO_ALIGN(x,a) ((x)&((a)-1))

static bool jc_fail_return(const char msg[]) {
#if 1
    JPG_LOG("[JPEG Encoder] - %s", msg);
#endif
    return false;   // must always return false
}


  #define JPEGENC_PMEM_ALLOC(size, va, pa, fd) {}
  #define JPEGENC_PMEM_FREE(va, size, fd)  {}

  
JpgEncHal::JpgEncHal()
{
    JPG_DBG("JpgEncHal::JpgEncHal");

    fIsAddSOI = true;
    fROIWidth = fROIHeight = 0;
    

    fEncSrcPmemVA   = NULL;
    fEncSrcCbCrPmemVA = NULL;
    fEncDstPmemVA   = NULL;

    pM4uDrv = NULL;    
 
    
    
#ifdef IMGMMU_SUPPORT_M4U    
    fMemType = fMemTypeDefault = JPEG_ENC_MEM_M4U ;
    //fm4uJpegID = JPGENC_RDMA;//M4U_CLNTMOD_JPGENC ;    
#ifdef JENC_HW_SUPPORT
    fm4uJpegInputID = JPGENC_RDMA;
    fm4uJpegOutputID = JPGENC_BSDMA;
#endif
#else
    fMemType = fMemTypeDefault = JPEG_ENC_MEM_PMEM ;
#endif
    

    fIsSrc2p = 0;
    fSrcPlaneNumber = 0;
    fIonDevFD = 0;
    islock = false;
    fDRI = 0;
    
    fEncSrcBufSize = 0;
    fEncCbCrBufSize = 0;
    
    fDstM4uPA = 0;
    fSrcM4uPA = 0;
    fDstIonPA = 0;
    fSrcIonPA = 0;
    fDstIonVA = NULL ;
    fSrcIonVA = NULL ;

    fSrcCb = NULL;
    fSrcCr = NULL;
    fSrcChromaAddr = NULL;    
    
#if 0 //def JENC_LOCK_VIDEO        
    LVDriverType = VAL_DRIVER_TYPE_JPEG_ENC;
#endif
}

JpgEncHal::~JpgEncHal()
{
    JPG_DBG("JpgEncHal::~JpgEncHal");
} 

bool JpgEncHal::lockVideo()
{
#ifdef JENC_LOCK_VIDEO       
    VAL_UINT32_T LVDriverType = VAL_DRIVER_TYPE_JPEG_ENC;
    VAL_HW_LOCK_T LVLock;

    //VDEC_HANDLE_T hHandle ;
    //hHandle.hValHandle = VAL_DRIVER_TYPE_JPEG_ENC ;
    
    // Init Val once per instance
    JPG_DBG("::lockVideo, L:%d!!\n", __LINE__);

    if(VAL_RESULT_NO_ERROR != eValInit((VAL_HANDLE_T *)&LVDriverType)){ 
      return false ;  
    }
    
    VAL_RESULT_T eValRet;
    
    LVLock.pvHandle = (VAL_VOID_T*)this;//prParam->pvHandle; // VA of JPEG encoder instance
    LVLock.eDriverType = VAL_DRIVER_TYPE_JPEG_ENC;
    LVLock.u4TimeoutMs = 0;              // return immediately if lock failed   
    
    eValRet = eVideoLockHW(&LVLock, sizeof(VAL_HW_LOCK_T));
    if(VAL_RESULT_NO_ERROR != eValRet )
    {
         JPG_DBG("lockVideo fail, type %d!!\n", eValRet);
         // Deinit Val once per instance
         eValDeInit((VAL_HANDLE_T *)&LVDriverType);
        // Lock HW failed - switch to SW JPEG ENC
        return false; //VAL_FALSE;
        
    }
    JPG_DBG("::lockVideo done, L:%d!!\n", __LINE__);

#endif    

    return true ;
}

bool JpgEncHal::unlockVideo()
{
#ifdef JENC_LOCK_VIDEO       
    VAL_UINT32_T LVDriverType = VAL_DRIVER_TYPE_JPEG_ENC;
    VAL_HW_LOCK_T LVLock;
    
    JPG_DBG("::unlockVideo, L:%d!!\n", __LINE__);
    //VDEC_HANDLE_T hHandle ;
    //hHandle.hValHandle = VAL_DRIVER_TYPE_JPEG_ENC ;

   VAL_RESULT_T eValRet;

    LVLock.pvHandle = (VAL_VOID_T*)this;//prParam->pvHandle; // VA of JPEG encoder instance
    LVLock.eDriverType = VAL_DRIVER_TYPE_JPEG_ENC;
    LVLock.u4TimeoutMs = 0;              // return immediately if lock failed     
   // Unlock HW
   eValRet = eVideoUnLockHW(&LVLock, sizeof(VAL_HW_LOCK_T));
   if(VAL_RESULT_NO_ERROR != eValRet )
   {
      JPG_DBG("Unlock Video fail, type %d!!\n", eValRet);  
   }
   
   // Deinit Val once per instance
   eValDeInit((VAL_HANDLE_T *)&LVDriverType);
   JPG_DBG("::unlockVideo done, L:%d!!\n", __LINE__);
   
#endif   
   return true ;
}   


bool JpgEncHal::LevelLock(EncLockType type)  
{
    JPG_DBG("::LevelLock type %d, islock %d, L:%d!!", type, islock, __LINE__);

#ifdef JENC_HW_SUPPORT

    if(islock){
      JPG_DBG("::LevelLock encoder already lock in type %d, islock %d, L:%d!!", type, islock, __LINE__);
      //JPG_DBG("encoder already lock %d, L:%d!!", islock, __LINE__);
      return false ;
    }
    
    if(type == JPEG_ENC_LOCK_SW_ONLY){
      JPG_DBG("::LevelLock, lock SW encoder only(%d), L:%d!!", type, __LINE__);
      fEncoderType = JPEG_ENC_SW;
    }
    else
    {
      // Lock HW
      if(!lockVideo())
      {
         if(type == JPEG_ENC_LOCK_HW_ONLY){
           JPG_DBG("::LevelLock, video is busy, return fail bcuz lock mode is HW only, L:%d!!",  __LINE__);
           return false ;
         }
         else
         {
           JPG_DBG("::LevelLock, video is busy, change to SW encoder(%d), L:%d!!", type, __LINE__);
           fEncoderType = JPEG_ENC_SW;
         }
      }
      else
      { //lock video success
         unsigned int ret = JPEG_ENC_STATUS_ERROR_CODEC_UNAVAILABLE ;
         #ifdef JENC_LOCK_HWENC
           ret = jpegEncLockEncoder(&encID) ;
         #endif        
         if (JPEG_ENC_STATUS_OK != ret)
         {
             unlockVideo();
             fEncoderType = JPEG_ENC_SW;
         }
         else    
         {
             fEncoderType = JPEG_ENC_HW;
         }      
      }
    }
    JPG_DBG("JpgEncHal::LevelLock enc_type %d, L:%d!!", fEncoderType,  __LINE__);

    islock = true;
#else
    
    fEncoderType = JPEG_ENC_SW;
    
#endif
    return true;
}

bool JpgEncHal::lock()
{
    JPG_DBG("JpgEncHal::lock %d!!", islock);
    
#ifdef JENC_HW_SUPPORT

    if(islock){
      JPG_DBG("encoder already lock %d, L:%d!!", islock, __LINE__);
      return false ;
    }
    
    // Lock HW
    if(!lockVideo())
    {
        fEncoderType = JPEG_ENC_SW;
    }
    else
    { //lock video success
       unsigned int ret = JPEG_ENC_STATUS_ERROR_CODEC_UNAVAILABLE ;
       #ifdef JENC_LOCK_HWENC
         ret = jpegEncLockEncoder(&encID) ;
       #endif        
       if (JPEG_ENC_STATUS_OK != ret)
       {
           unlockVideo();
           fEncoderType = JPEG_ENC_SW;
       }
       else    
       {
           fEncoderType = JPEG_ENC_HW;
       }      
    }
    JPG_DBG("JpgEncHal::lock enc_type %d, L:%d!!", fEncoderType,  __LINE__);

    islock = true;
#else

    fEncoderType = JPEG_ENC_SW;

#endif
    return true;
}


bool JpgEncHal::unlock()
{
    JPG_DBG("JpgEncHal::unlock");
    
#ifdef JENC_HW_SUPPORT

    if(islock)
    {   

      if(JPEG_ENC_HW == fEncoderType){        

        jpegEncUnlockEncoder(encID);

        unlockVideo();

        if( fMemType == JPEG_ENC_MEM_PMEM){
          
          JPEGENC_PMEM_FREE(fEncSrcPmemVA, fEncSrcBufSize, fEncSrcPmemFD);
          JPEGENC_PMEM_FREE(fEncSrcCbCrPmemVA, fEncCbCrBufSize, fEncSrcCbCrPmemFD);
          JPEGENC_PMEM_FREE(fEncDstPmemVA, fDstSize, fEncDstPmemFD);
          
        }else if( fMemType == JPEG_ENC_MEM_M4U ){
          free_m4u() ;      
          
        }else if( fMemType == JPEG_ENC_MEM_ION ){
          free_ion() ; 
        }    
	    }
    
      islock = false;
    }
#endif /*JENC_HW_SUPPORT*/

    return true;
}

#if 0
void JpgEncHal::setROI(JUINT32 x, JUINT32 y, JUINT32 width, JUINT32 height)
{
    fROIX = x;
    fROIY = y;
    fROIWidth = width;
    fROIHeight = height;
}

#endif 

bool JpgEncHal::setEncSize(JUINT32 width, JUINT32 height, EncFormat encformat, bool isSrcBufNeedFlush)
{  
   
   unsigned int width2, width_c;
       
   if( (width * height) > JPEG_MAX_ENC_SIZE ) {
      JPG_DBG("JpgEncHal:: Unsupported Encode Image Size (%d, %d)!!\n", width, height);
      return false ;
   }
      
   fDstWidth = width; 
   fDstHeight = height; 
   fEncFormat = encformat;
   fIsSrcBufNeedFlush = isSrcBufNeedFlush;
   
    width2 = ((width+1)>>1) << 1;

    if (fEncFormat == kENC_YUY2_Format || fEncFormat == kENC_UYVY_Format)
    {
        
        fSrcMinBufferStride  = TO_CEIL((width2 << 1), 32) ; 
        fSrcBufHeight  = TO_CEIL((height), 8) ; 

        fSrcMinBufferSize = fSrcMinBufferStride * fSrcBufHeight;
        
        fSrcMinCbCrSize = fSrcMinCbCrStride = fSrcCbCrBufHeight = 0;
        fIsSrc2p = 0;
       fSrcPlaneNumber = 1;
        
    }
    else if (fEncFormat == kENC_NV12_Format || fEncFormat == kENC_NV21_Format)
    {        
        fSrcMinBufferStride    = TO_CEIL((width2), 16) ; 
        fSrcBufHeight    = TO_CEIL((height), 16) ;
        fSrcMinBufferSize   = fSrcMinBufferStride * fSrcBufHeight; 
        
        fSrcMinCbCrStride = fSrcMinBufferStride ;
        fSrcCbCrBufHeight = fSrcBufHeight / 2 ;
        fSrcMinCbCrSize = fSrcMinCbCrStride * fSrcCbCrBufHeight;
        fIsSrc2p = 1;
       fSrcPlaneNumber = 2;
    }   
   else if (fEncFormat == kENC_YV12_Format)
   {
       fSrcMinBufferStride = TO_CEIL((width2), 16) ;
       fSrcBufHeight       = TO_CEIL((height), 16) ;
       fSrcMinBufferSize   = fSrcMinBufferStride * fSrcBufHeight;
   
       fSrcMinCbCrStride = TO_CEIL((fSrcMinBufferStride/2), 16) ; 
       fSrcCbCrBufHeight = fSrcBufHeight / 2; 
       fSrcMinCbCrSize   = fSrcMinCbCrStride * fSrcCbCrBufHeight;
       printf("ENCHAL:: Y: stride %d, height %d, size %x!!\n", fSrcMinBufferStride, fSrcBufHeight, fSrcMinBufferSize);
       printf("ENCHAL:: C: stride %d, height %d, size %x!!\n", fSrcMinCbCrStride, fSrcCbCrBufHeight, fSrcMinCbCrSize);
       fIsSrc2p = 1;
       fSrcPlaneNumber = 3;
   }
   else
   {
       printf("Unsupport JPEG Encode format!");
   }
   //JPG_DBG("JpgEncHal:: stride %d, height %d , min Size %x %x, L:%d!!\n",fSrcMinBufferStride, fSrcBufHeight,fSrcMinBufferSize, fSrcMinCbCrSize, __LINE__);
   return true ;
}

bool JpgEncHal::setSrcAddr(void *srcAddr, void *srcChromaAddr) 
{
   if( ((unsigned int)srcAddr & 0x0f) || (srcAddr == NULL) ){
      JPG_DBG("JpgEncHal:: Src Luma Address is NULL or not 16-byte alignment %x!!\n", srcAddr);
      return false;            
   }
#ifdef OLD_C_FD   
   if ( fIsSrc2p && ( (srcChromaAddr == NULL) || ((unsigned int)srcChromaAddr & 0x0f) )  ){
      JPG_DBG("JpgEncHal::set NV Src CbCr Address NULL or not 16-byte align %x!!\n", srcChromaAddr);
      return false;      
   }
#endif
   
   fSrcAddr = srcAddr; 
#ifdef OLD_C_FD      
   if( fIsSrc2p )
     fSrcChromaAddr = srcChromaAddr ;
#endif     
   return true ;
};

/// For YUV420(3P), the source bitstream Y, U, V might not be continuous
/// Add a new member function for support Y, U, V are seperate address
/// when fSrcPlaneNumber = 3, there contain 3 plane
bool JpgEncHal::setSrcAddr(void *srcAddr, void *srcCb, void *srcCr)
{
   /// checking source buffer addres alignment
   if(((JUINT32)srcAddr & 0x0f) ||
      (NULL == srcAddr))
   {
      JPG_DBG("JpgEncHal:: Src Luma Address is NULL or not 16-byte alignment %x!!\n", srcAddr);

      return false;
   }

   fSrcAddr = srcAddr;

   if(fIsSrc2p)
   {
      fSrcChromaAddr = srcCb;
   }

   if (3 == fSrcPlaneNumber)
   {
      fSrcCb = srcCb;
      fSrcCr = srcCr;
   }

   return true ;
}


bool JpgEncHal::setSrcBufSize( JUINT32 srcStride,JUINT32 srcSize, JUINT32 srcSize2)//, JUINT32 srcStride2) 
{
   if( (!fIsSrc2p) && (srcSize < fSrcMinBufferSize ) 
      && IS_NO_ALIGN(srcStride, 32)  ) {
      JPG_DBG("JpgEncHal::set YUYV Src Size Fail, %x >= %x, align %x, %x !!\n", srcSize, fSrcMinBufferSize, srcStride);
      return false;
   }else if ( fIsSrc2p && (srcSize2 < fSrcMinCbCrSize) 
      && IS_NO_ALIGN(srcStride, 16)  ){
      JPG_DBG("JpgEncHal::set NV Src Size Fail, %x >= %x, %x >= %x, stride %x !!\n", srcSize, fSrcMinBufferSize, srcSize2, fSrcMinCbCrSize, srcStride);
      return false;      
   }

   fEncSrcBufSize    = srcSize;
   fSrcBufStride    =  srcStride ;  
   if(fIsSrc2p){
     fEncCbCrBufSize = srcSize2 ;
     fSrcCbCrBufStride = srcStride ;
   }else{
     fEncCbCrBufSize = 0 ;
     fSrcCbCrBufStride = 0 ;      
   }
   
   
   return true ;
};



bool JpgEncHal::setSrcBufSize(JUINT32 srcStride, JUINT32 srcSize, JUINT32 srcSize2, JUINT32 srcSize3, JUINT32 srcUVStride)
{

   if( (!fIsSrc2p) &&
       (srcSize < fSrcMinBufferSize) &&
       IS_NO_ALIGN(srcStride, 32))
   {
      JPG_DBG("JpgEncHal::set YUYV Src Size Fail, %x >= %x, align %x, %x !!\n", srcSize, fSrcMinBufferSize, srcStride);
      return false;
   }
   else if ( fIsSrc2p &&
            (srcSize2 < fSrcMinCbCrSize) &&
            IS_NO_ALIGN(srcStride, 16))
   {
      JPG_DBG("JpgEncHal::set NV Src Size Fail, %x >= %x, %x >= %x, stride %x !!\n", srcSize, fSrcMinBufferSize, srcSize2, fSrcMinCbCrSize, srcStride);
      return false;
   }

   fEncSrcBufSize = srcSize;
   fSrcBufStride  = srcStride;
   fEncCbCrBufSize   = srcSize2;
   fSrcCbCrBufStride = TO_CEIL( srcStride/2, 16);   
   
   if ((3 == fSrcPlaneNumber) && 
       (srcSize2 != srcSize3))
   {
      return false;
   }
      
   //if(3 == fSrcPlaneNumber)
   //{
   //   fEncCbCrBufSize   = srcSize2;
   //   fSrcCbCrBufStride = TO_CEIL( srcStride/2, 16);
   //}
   //else
   //{
   //   fEncCbCrBufSize   = 0;
   //   fSrcCbCrBufStride = 0;
   //}

   return true ;
}

bool JpgEncHal::allocPMEM()
{
   
#ifdef JENC_HW_SUPPORT
      
      // allocate Y buffer
      JPEGENC_PMEM_ALLOC(fEncSrcBufSize, fEncSrcPmemVA, fEncSrcPmemPA, fEncSrcPmemFD);       
      memcpy((void*)fEncSrcPmemVA, fSrcAddr, fEncSrcBufSize);
      JPG_DBG(" Allocate Source Y Pmem, va:0x%x, pa:0x%x, size:%x", fEncSrcPmemVA, fEncSrcPmemPA, fEncSrcBufSize);

      // allocate C buffer
      if (fIsSrc2p){
        JPEGENC_PMEM_ALLOC(fEncCbCrBufSize, fEncSrcCbCrPmemVA, fEncSrcCbCrPmemPA, fEncSrcCbCrPmemFD);  
        memcpy((void*)fEncSrcCbCrPmemVA, fSrcChromaAddr, fEncCbCrBufSize); 
        JPG_DBG(" Allocate Source C Pmem, va:0x%x, pa:0x%x, size:%x", fEncSrcCbCrPmemVA, fEncSrcCbCrPmemPA, fEncCbCrBufSize);   
      }
      
      // allocate bitstream buffer
      JPEGENC_PMEM_ALLOC(fDstSize, fEncDstPmemVA, fEncDstPmemPA, fEncDstPmemFD);  
      JPG_DBG(" Allocate Dst Pmem, va:0x%x, pa:0x%x, size:%x", fEncDstPmemVA, fEncDstPmemPA, fDstSize);   
   
#endif /*JENC_HW_SUPPORT*/

    return true;
}


 





bool JpgEncHal::alloc_m4u()
{
#ifdef JENC_HW_SUPPORT

#ifdef IMGMMU_SUPPORT_M4U
    
    if(!imgMmu_create(&pM4uDrv, fm4uJpegInputID))
      return false; 
    
    if(!imgMmu_create(&pM4uDrv, fm4uJpegOutputID))
      return false; 
          
    //if(!imgMmu_reset(&pM4uDrv, fm4uJpegID))
    //  return false;       

#ifdef OLD_C_FD
    if(!imgMmu_alloc_pa(&pM4uDrv, fm4uJpegInputID, (JUINT32) fSrcAddr, fEncSrcBufSize, &fSrcM4uPA))
      return false; 
    //src chroma
    if(fIsSrc2p && !imgMmu_alloc_pa(&pM4uDrv, fm4uJpegInputID, (JUINT32) fSrcChromaAddr, fEncCbCrBufSize, &fSrcChromaM4uPA))
      return false;
#else
    //src Luma
    //if(fIsSrc2p){      
      if(!imgMmu_alloc_pa(&pM4uDrv, fm4uJpegInputID, (JUINT32) fSrcAddr, fEncSrcBufSize + fEncCbCrBufSize, &fSrcM4uPA))
        return false;      
    //}else{
    //  if(!imgMmu_alloc_pa(&pM4uDrv, fm4uJpegInputID, (JUINT32) fSrcAddr, fEncSrcBufSize, &fSrcM4uPA))
    //    return false;      
    //}      
#endif

    //dst bitstream
    if(!imgMmu_alloc_pa(&pM4uDrv, fm4uJpegOutputID, (JUINT32) fDstAddr, fDstSize, &fDstM4uPA))
      return false;
    
    //config module port
    imgMmu_cfg_port(pM4uDrv, fm4uJpegInputID, JPGENC_RDMA);
    imgMmu_cfg_port(pM4uDrv, fm4uJpegOutputID, JPGENC_BSDMA);
    

#ifdef OLD_C_FD    
    imgMmu_sync(pM4uDrv, fm4uJpegInputID, (JUINT32) fSrcAddr, fEncSrcBufSize, SYNC_HW_READ);
    if(fIsSrc2p) imgMmu_sync(pM4uDrv, fm4uJpegInputID, (JUINT32) fSrcChromaAddr, fEncCbCrBufSize, SYNC_HW_READ);
#else
    //flush cache
    //if(fIsSrc2p){
      imgMmu_sync(pM4uDrv, fm4uJpegInputID, (JUINT32) fSrcAddr, fEncSrcBufSize + fEncCbCrBufSize, SYNC_HW_READ);
    //}else{
    //  imgMmu_sync(pM4uDrv, fm4uJpegInputID, (JUINT32) fSrcAddr, fEncSrcBufSize, SYNC_HW_READ);
    //}
      
#endif      
    imgMmu_sync(pM4uDrv, fm4uJpegOutputID, (JUINT32) fDstAddr, fDstSize, SYNC_HW_WRITE);

#endif

#endif /*JENC_HW_SUPPORT*/
   
   return true;
}




bool JpgEncHal::free_m4u()
{
#ifdef JENC_HW_SUPPORT

#ifdef IMGMMU_SUPPORT_M4U   
   if(pM4uDrv)
   {
      
#ifdef OLD_C_FD      
      imgMmu_dealloc_pa(pM4uDrv, fm4uJpegInputID, (JUINT32) fSrcAddr      , fEncSrcBufSize    , &fSrcM4uPA       );
      if(fIsSrc2p) imgMmu_dealloc_pa(pM4uDrv, fm4uJpegInputID, (JUINT32) fSrcChromaAddr, fEncCbCrBufSize  , &fSrcChromaM4uPA );
#else
      
      imgMmu_dealloc_pa(pM4uDrv, fm4uJpegInputID, (JUINT32) fSrcAddr      , fEncSrcBufSize  + fEncCbCrBufSize  , &fSrcM4uPA       );   
      
#endif         
      imgMmu_dealloc_pa(pM4uDrv, fm4uJpegOutputID, (JUINT32) fDstAddr      , fDstSize         , &fDstM4uPA       );
      delete pM4uDrv;
   }   
   pM4uDrv = NULL;
   
#endif   

#endif /*JENC_HW_SUPPORT*/
   return true;
}





bool JpgEncHal::alloc_ion()
{
#ifdef JENC_HW_SUPPORT
   
#ifdef IMGMMU_SUPPORT_ION   
    
    if(!imgMmu_create(&pM4uDrv, fm4uJpegInputID))
      return false; 
    if(!imgMmu_create(&pM4uDrv, fm4uJpegOutputID))
      return false;           
   
   
   imgMmu_cfg_port(pM4uDrv, fm4uJpegInputID, JPGENC_RDMA);
   imgMmu_cfg_port(pM4uDrv, fm4uJpegOutputID, JPGENC_BSDMA);
   
   
   imgIon_open(&fIonDevFD );

   //dst
   //imgIon_getVA(fDstFD, fDstSize, &fDstIonVA);
   if( !imgIon_getPA(fIonDevFD, fDstFD, fm4uJpegOutputID, fDstIonVA, fDstSize, &fDstIonHdle, &fDstIonPA))
      return false;
   if(!imgMmu_pa_map_tlb(&pM4uDrv, fm4uJpegOutputID , fDstSize, &fDstIonPA))
      return false;

#ifdef OLD_C_FD
   //src
   //imgIon_getVA(fSrcFD, fEncSrcBufSize, &fSrcIonVA);
   if( !imgIon_getPA(fIonDevFD, fSrcFD, fm4uJpegInputID, fSrcIonVA, fEncSrcBufSize, &fSrcIonHdle, &fSrcIonPA))
      return false;
   if(!imgMmu_pa_map_tlb(&pM4uDrv, fm4uJpegInputID , fEncSrcBufSize, &fSrcIonPA))
      return false;
      
   //src chroma
   if( fIsSrc2p ){
     //imgIon_getVA(fSrcFD2, fEncCbCrBufSize, &fSrcChromaIonVA);
     if( !imgIon_getPA(fIonDevFD, fSrcFD2, fm4uJpegInputID, fSrcChromaIonVA, fEncCbCrBufSize, &fSrcChromaIonHdle, &fSrcChromaIonPA))
        return false;
     if(!imgMmu_pa_map_tlb(&pM4uDrv, fm4uJpegInputID , fEncCbCrBufSize, &fSrcChromaIonPA))
        return false;        
   }
#else
   //src
   //imgIon_getVA(fSrcFD, fEncSrcBufSize + fEncCbCrBufSize, &fSrcIonVA);
   if( !imgIon_getPA(fIonDevFD, fSrcFD, fm4uJpegInputID, fSrcIonVA, fEncSrcBufSize + fEncCbCrBufSize, &fSrcIonHdle, &fSrcIonPA))
      return false;
   if(!imgMmu_pa_map_tlb(&pM4uDrv, fm4uJpegInputID , fEncSrcBufSize + fEncCbCrBufSize, &fSrcIonPA))
      return false;
   
#endif


   
   imgIon_sync(fIonDevFD, fDstIonHdle);
   if (fIsSrcBufNeedFlush == true)
   {
       imgIon_sync(fIonDevFD, fSrcIonHdle);   
    #ifdef OLD_C_FD   
       if(fIsSrc2p)imgIon_sync(fIonDevFD, fSrcChromaIonHdle);
    #endif   
   }

#endif

#endif /*JENC_HW_SUPPORT*/
   return true;

}

bool JpgEncHal::free_ion()
{
#ifdef JENC_HW_SUPPORT
  
#ifdef IMGMMU_SUPPORT_ION

   if(pM4uDrv)
   {
#ifdef OLD_C_FD      
      imgMmu_pa_unmap_tlb(pM4uDrv, fm4uJpegInputID, fEncSrcBufSize , &fSrcIonPA      );
      if( fIsSrc2p )imgMmu_pa_unmap_tlb(pM4uDrv, fm4uJpegInputID, fEncCbCrBufSize, &fSrcChromaIonPA);
#else
      imgMmu_pa_unmap_tlb(pM4uDrv, fm4uJpegInputID, fEncSrcBufSize + fEncCbCrBufSize, &fSrcIonPA      );   
#endif         
      imgMmu_pa_unmap_tlb(pM4uDrv, fm4uJpegOutputID, fDstSize       , &fDstIonPA      );      
      delete pM4uDrv;
   }   
   pM4uDrv = NULL;
  
  //imgIon_freeVA(&fDstIonVA, fDstSize);
#ifdef OLD_C_FD  
  //imgIon_freeVA(&fSrcIonVA, fEncSrcBufSize);
  //imgIon_freeVA(&fSrcChromaIonVA, fEncCbCrBufSize);
#else
  //imgIon_freeVA(&fSrcIonVA, fEncSrcBufSize + fEncCbCrBufSize);
#endif  
  imgIon_close(fIonDevFD);

#endif

#endif /*JENC_HW_SUPPORT*/
  
  return true; 
 
}


bool JpgEncHal::onSwEncode(JUINT32 *encSize)
{
       /// using SP JPEG encoder(libjpeg)
       int quality  = fQuality;
       unsigned int image_width  = fDstWidth;
       unsigned int image_height = fDstHeight;
       unsigned int encodeBufferSize = 0;
       
       struct jpeg_compress_struct cinfo;
       struct jpeg_error_mgr jerr;
       int i,j;
       int  encLines = 0;
       JSAMPROW y[16],cb[16],cr[16];           
       JSAMPARRAY data[3];           
       data[0] = y;
       data[1] = cb;
       data[2] = cr;
       
       /// Step 1: allocate and initialize JPEG compression object
       cinfo.err = jpeg_std_error(&jerr);
       
       jpeg_create_compress(&cinfo);
       
       /// Step 2: set parameters for compression
       cinfo.image_width      = image_width;    /* image width and height, in pixels */
       cinfo.image_height     = image_height;
       cinfo.input_components = 3;          /* # of color components per pixel */
       cinfo.in_color_space   = JCS_YCbCr;  /* colorspace of input image */
       jpeg_set_defaults(&cinfo);
       
       cinfo.raw_data_in = true;
       jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);
       cinfo.en_soi = fIsAddSOI ;
       cinfo.dct_method = JDCT_IFAST;
       
       /// Step 3: specify data destination (eg, a memory)
       jpeg_mem_dest (&cinfo, (unsigned char **)&fDstAddr, (unsigned long *)&fDstSize);
       
       if ((kENC_NV21_Format == fEncFormat) ||      /// YUV420
           (kENC_NV12_Format == fEncFormat) ||
           (kENC_YV12_Format == fEncFormat))
       {
            /// supply downsampled data
            cinfo.comp_info[0].h_samp_factor = 2;
            cinfo.comp_info[0].v_samp_factor = 2;
            cinfo.comp_info[1].h_samp_factor = 1;
            cinfo.comp_info[1].v_samp_factor = 1;
            cinfo.comp_info[2].h_samp_factor = 1;
            cinfo.comp_info[2].v_samp_factor = 1;
       }
       else if ((kENC_YUY2_Format == fEncFormat) ||  /// YUV422
                (kENC_UYVY_Format == fEncFormat))
       {
            /// supply downsampled data
            cinfo.comp_info[0].h_samp_factor = 2;
            cinfo.comp_info[0].v_samp_factor = 1;
            cinfo.comp_info[1].h_samp_factor = 1;
            cinfo.comp_info[1].v_samp_factor = 1;
            cinfo.comp_info[2].h_samp_factor = 1;
            cinfo.comp_info[2].v_samp_factor = 1;
       }
       else
       {
           printf("JPEG HAL:Unsupport format!\n");
           return false;
       }
       
       switch(fEncFormat)
       {
       case kENC_NV21_Format:  /// YUV420, 2x2 subsampled , interleaved V/U plane
       case kENC_NV12_Format:  /// YUV420, 2x2 subsampled , interleaved V/U plane
            {
               unsigned char *pCompY    = (unsigned char *)fSrcAddr;
               unsigned char *pCompCbCr = (unsigned char *)fSrcAddr+ fEncSrcBufSize;//fSrcChromaAddr;
       
               unsigned char *pSrcY = NULL;
               unsigned char *pSrcU = NULL;
               unsigned char *pSrcV = NULL;
       
               int  count = 0;
       
               unsigned char *mcuTempU = (unsigned char *)malloc(image_width * 8);
               unsigned char *mcuTempV = (unsigned char *)malloc(image_width * 8);
               
               JPG_DBG(" sw jpeg encoder format %d, %x %x %x %x !!\n", fEncFormat, pCompY, pCompCbCr, mcuTempU, mcuTempV);
       
               /// Step 4: Start compressor
               jpeg_start_compress(&cinfo, TRUE);
       
               /// assign the JSAMPARRAY address
               for (i = 0; i < 8; i++)
               {
                   cb[i] = mcuTempU + (i/1) * (image_width/2);
                   cr[i] = mcuTempV + (i/1) * (image_width/2);
               }
       
               if (kENC_NV21_Format == fEncFormat) /// NV12: Y, VU
               {
                  for (j = 0; j < image_height; j += 16)
                  {
                     encLines = ((image_height - j) >= 16)? 16 : (image_height - j);
                     for (i = 0; i < 16; i++)
                     {
                        if (i < encLines)
                           y[i]  = pCompY + i * image_width;
                        // dummy pointer to avoid libjpeg access invalid address
                        else
                           y[i]  = pCompY;
                     }
                  
                     pSrcU = mcuTempU;
                     pSrcV = mcuTempV;
                  
                     /// convert YUV420 UV-itlv to YUV420 UV SP temp buffer
                     count = (image_width >> 1) * (encLines >> 1);
                     while (--count >= 0)
                     {
                        *pSrcV++ = *pCompCbCr++;
                        *pSrcU++ = *pCompCbCr++;
                     }
                  
                     jpeg_write_raw_data (&cinfo, data, 16);
                     pCompY  = pCompY  + encLines * image_width;
                  }
               }
               else /// NV12: Y, UV
               {
                  for (j = 0; j < image_height; j += 16)
                  {
                     encLines = ((image_height - j) >= 16)? 16 : (image_height - j);
                     for (i = 0; i < 16; i++)
                     {
                        if (i < encLines)
                           y[i]  = pCompY + i * image_width;
                        // dummy pointer to avoid libjpeg access invalid address
                        else
                           y[i]  = pCompY;
                     }
                  
                     pSrcU = mcuTempU;
                     pSrcV = mcuTempV;
                  
                     /// convert YUV420 UV-itlv to YUV420 UV SP temp buffer
                     count = (image_width >> 1)* (encLines >> 1);
                     while (--count >= 0)
                     {
                         *pSrcU++ = *pCompCbCr++;
                         *pSrcV++ = *pCompCbCr++;
                     }
                  
                     jpeg_write_raw_data (&cinfo, data, 16);
                     pCompY  = pCompY  + encLines * image_width;
                  }
               }
               free(mcuTempU);
               free(mcuTempV);
            }
            break;
       case kENC_YV12_Format:  /// YUV420 3P
            {
               unsigned char *compY  = (unsigned char *)fSrcAddr;
               unsigned char *compCb = (unsigned char *)fSrcCb;
               unsigned char *compCr = (unsigned char *)fSrcCr;
       
               unsigned char *mcuTempU = NULL;
               unsigned char *mcuTempV = NULL;
               bool useTempUV = false;

               // Non-MCU align case need to allocate extra buffer
               if (image_height % 16)
               {
                   mcuTempU = (unsigned char *)malloc(fSrcCbCrBufStride * 8);
                   mcuTempV = (unsigned char *)malloc(fSrcCbCrBufStride * 8);
                   if (!mcuTempU || !mcuTempV)
                   {
                       JPG_DBG("Sw jpeg encode fail due to memory not enough\n");
                       return false;
                   }
               }
               
               /// Step 4: Start compressor
               jpeg_start_compress(&cinfo, TRUE);
       
               for (j = 0; j < image_height; j += 16)
               {
                  encLines = ((image_height - j) >= 16)? 16 : (image_height - j);

                  if (encLines < 16)
                  {
                      memcpy(mcuTempU, compCb, fSrcCbCrBufStride * (encLines >> 1));
                      memcpy(mcuTempV, compCr, fSrcCbCrBufStride * (encLines >> 1));
                      useTempUV = true;
                  }
                  
                  for (i = 0; i < 16; i++)
                  {
                      if (i < encLines)
                      {
                          y[i] = compY + i * fSrcBufStride;
                          if (i%2 == 0)
                          {
                              if (useTempUV)
                              {
                                  cb[i/2] = mcuTempU + (i/2) * ( fSrcCbCrBufStride );
                                  cr[i/2] = mcuTempV + (i/2) * ( fSrcCbCrBufStride );
                              }
                              else
                              {
                                  cb[i/2] = compCb + (i/2) * ( fSrcCbCrBufStride );
                                  cr[i/2] = compCr + (i/2) * ( fSrcCbCrBufStride );
                              }
                          }
                      }
                      // dummy pointer to avoid libjpeg access invalid address
                      else
                      {
                          y[i] = compY;
                          if (i%2 == 0)
                          {
                              if (useTempUV)
                              {
                                  cb[i/2] = mcuTempU;
                                  cr[i/2] = mcuTempV;
                              }
                              else
                              {
                                  cb[i/2] = compCb;
                                  cr[i/2] = compCr;
                              }
                          }
                      }
                  }
       
                  jpeg_write_raw_data (&cinfo, data, 16);
                  compY  = compY  + encLines * fSrcBufStride;
                  compCb = compCb + (encLines >> 1)  * fSrcCbCrBufStride;
                  compCr = compCr + (encLines >> 1)  * fSrcCbCrBufStride;
               }
               
               if (useTempUV)
               {
                   free(mcuTempU);
                   free(mcuTempV);
               }
            }
            break;
       case kENC_YUY2_Format: /// YUV422 (YUYV, UYVY)
       case kENC_UYVY_Format:
            {
               unsigned int *pCompYUVpacked = (unsigned int *)fSrcAddr;
               unsigned char *pSrcY = NULL;
               unsigned char *pSrcU = NULL;
               unsigned char *pSrcV = NULL;
       
               int  count = 0;
               unsigned char *mcuTempY = (unsigned char *)malloc(image_width * 8);
               unsigned char *mcuTempU = (unsigned char *)malloc(image_width * 4);
               unsigned char *mcuTempV = (unsigned char *)malloc(image_width * 4);
       
               /// Step 4: Start compressor
               jpeg_start_compress(&cinfo, TRUE);
       
               /// assign the JSAMPARRAY address
               for (i = 0; i < 8; i++)
               {
                  y[i]  = mcuTempY + i * image_width;
                  cb[i] = mcuTempU + i * image_width/2;
                  cr[i] = mcuTempV + i * image_width/2;
               }
       
               if (kENC_UYVY_Format == fEncFormat)
               {
                   for (j = 0; j < image_height; j += 8)
                   {
                      encLines = ((image_height - j) >= 8)? 8 : (image_height - j);
                      pSrcY = mcuTempY;
                      pSrcU = mcuTempU;
                      pSrcV = mcuTempV;
       
                      /// convert YUV422 itlv to YUV422 temp buffer
                      count = (image_width >> 1) * encLines;
                      while (--count >= 0)
                      {
                          unsigned int val = *pCompYUVpacked++;
                          *pSrcU++ = val & 0xFF;
                          *pSrcY++ = (val >> 8) & 0xFF;
                          *pSrcV++ = (val >> 16) & 0xFF;
                          *pSrcY++ = (val >> 24) & 0xFF;
                      }
       
                      jpeg_write_raw_data (&cinfo, data, 8);
                   }
               }
               else
               {
                   for (j = 0; j < image_height; j += 8)
                   {
                      encLines = ((image_height - j) >= 8)? 8 : (image_height - j);
                      pSrcY = mcuTempY;
                      pSrcU = mcuTempU;
                      pSrcV = mcuTempV;
       
                      /// convert YUV422 itlv to YUV422 temp buffer
                      count = (image_width >> 1) * encLines;
                      while (--count >= 0)
                      {
                          unsigned int val = *pCompYUVpacked++;
                          *pSrcY++ = val & 0xFF;
                          *pSrcU++ = (val >> 8) & 0xFF;
                          *pSrcY++ = (val >> 16) & 0xFF;
                          *pSrcV++ = (val >> 24) & 0xFF;
                      }
       
                      jpeg_write_raw_data (&cinfo, data, 8);
                   }
               }
               free(mcuTempY);
               free(mcuTempU);
               free(mcuTempV);
            }
            break;
       default:
           break;
       }
       
       /// Step 6: Finish compression
       jpeg_finish_compress(&cinfo);
       
       /// Step 7: release JPEG compression object
       jpeg_destroy_compress(&cinfo);

       //if (!fIsAddSOI)
       //{
       //   unsigned char* pt = (unsigned char*)fDstAddr;
       //   pt[0] = 0x00;
       //   pt[1] = 0x00;
       //}
       
       *encSize = fDstSize;       
       return true;   
   
}

bool JpgEncHal::start(JUINT32 *encSize)
{
    JPEG_ENC_HAL_IN inJpgEncParam;

#ifdef MTK_PLATFORM_OPTIMIZE
    JPG_LOG("drvb S\n");
  #ifdef MTK_HW_ENHANCE
    #ifdef USE_DRVB_F0
      int drvb_ret=0;
  
      drvb_f0();
      drvb_ret=drvb_f1(0x3fffffff);
    #else
      drvb_f1(0x3fffffff);
    #endif
  #else
    #ifdef USE_DRVB_F0
      int drvb_ret=0;
    
      drvb_f0();
      drvb_ret=drvb_f1(0x1fffffff);
    #else
      drvb_f1(0x1fffffff);
    #endif
  #endif
  
  #ifdef USE_DRVB_F0
    JPG_LOG("drvb E (0x%x)\n",drvb_ret);
  #else
    JPG_LOG("drvb E\n");
  #endif
#endif

    JPG_DBG(" JpgEncHal::start -> config jpeg encoder");
    JPG_DBG(" Src Addr:0x%x, width/height:[%u, %u], format:%u", (unsigned int)fSrcAddr, fDstWidth, fDstHeight, fEncFormat);
    JPG_DBG(" Src Size:0x%x, 0x%x, Stride %x!!", (unsigned int)fEncSrcBufSize, fEncCbCrBufSize,fSrcBufStride);
    JPG_DBG(" Dst Addr:0x%x, Quality:%u, Buffer Size:%u, Need add SOI:%d, memType %d!!", (unsigned int)fDstAddr,fQuality, fDstSize, fIsAddSOI, fMemType);

#ifdef JENC_HW_SUPPORT

    if (JPEG_ENC_HW == fEncoderType)
    {
        bool switchToSw = false;
        // NV12/21 must 16x16 align
        if ((kENC_NV12_Format == fEncFormat || kENC_NV21_Format == fEncFormat) &&
            ((fDstWidth & 0xF) || (fDstHeight & 0xF)))
        {
            switchToSw = true;
            JPG_DBG(" JpgEncHal::start YUV420 Non-MCU align => force to use sw");
        }

        // YUY2 must 16x8 align
        if (kENC_YUY2_Format == fEncFormat &&
            ((fDstWidth & 0xF) || (fDstHeight & 0x7)))
        {
            switchToSw = true;
            JPG_DBG(" JpgEncHal::start YUV422 Non-MCU align => force to use sw");
        }

        // redirect encode to sw since HW did not support YV12 and UYVY format
        if (kENC_YV12_Format == fEncFormat || kENC_UYVY_Format == fEncFormat)
        {
            switchToSw = true;
            JPG_DBG(" JpgEncHal::unsupported format %d => force to use sw", fEncFormat);
        }

        if (switchToSw == true)
        {
            jpegEncUnlockEncoder(encID);
            unlockVideo();
            fEncoderType = JPEG_ENC_SW;
        }
    }

#endif

   if (JPEG_ENC_SW == fEncoderType)
    {
        if(!onSwEncode(encSize))
            return jc_fail_return("jpeg sw encode fail!!");
    }

#ifdef JENC_HW_SUPPORT

    else
    { /// if (JPEG_ENC_SW == fEncoderType)
    // config jpeg encoder
    memset(&inJpgEncParam, 0 , sizeof(JPEG_ENC_HAL_IN));

    inJpgEncParam.dstBufferSize     = fDstSize;
    inJpgEncParam.dstWidth          = fDstWidth;
    inJpgEncParam.dstHeight         = fDstHeight;
    inJpgEncParam.srcBufferStride   = fSrcBufStride ;     //TODO: check validation
    //inJpgEncParam.srcChromaStride = fSrcCbCrBufStride ; //TODO: check validation
    inJpgEncParam.restartInterval   = fDRI;

    if( fMemType == JPEG_ENC_MEM_PHY){
      inJpgEncParam.dstBufferAddr = (unsigned int) fDstAddr;
      inJpgEncParam.srcBufferAddr = (unsigned int) fSrcAddr ;
      inJpgEncParam.srcChromaAddr = (unsigned int) fSrcChromaAddr;
      
    }else if( fMemType == JPEG_ENC_MEM_PMEM ){
      
      if(!allocPMEM()){
        JPG_LOG("Encoder Allocate PMEM Fail\n");
        return false;         
      }
      
      inJpgEncParam.srcBufferAddr = (unsigned int) fEncSrcPmemPA ;
      inJpgEncParam.srcChromaAddr = (unsigned int) fEncSrcCbCrPmemPA;      
      inJpgEncParam.dstBufferAddr = (unsigned int) fEncDstPmemPA;
      
    }else if( fMemType == JPEG_ENC_MEM_M4U ){
      if(!alloc_m4u()) {    
        JPG_LOG("Encoder Allocate M4U Fail!!\n");
        return false;   
      }
      inJpgEncParam.srcBufferAddr = (unsigned int) fSrcM4uPA       ;
#ifdef OLD_C_FD      
      inJpgEncParam.srcChromaAddr = (unsigned int) fSrcChromaM4uPA ;
#else
      inJpgEncParam.srcChromaAddr = (unsigned int) fSrcM4uPA + fEncSrcBufSize ;      
#endif      
      inJpgEncParam.dstBufferAddr = (unsigned int) fDstM4uPA       ;
    }else if( fMemType == JPEG_ENC_MEM_ION ){
      if(!alloc_ion()) {    
        JPG_LOG("Encoder Allocate ION Fail!!\n");
        return false;   
      }
      inJpgEncParam.srcBufferAddr = (unsigned int) fSrcIonPA       ;
#ifdef OLD_C_FD       
      inJpgEncParam.srcChromaAddr = (unsigned int) fSrcChromaIonPA ;
#else
      inJpgEncParam.srcChromaAddr = (unsigned int) fSrcIonPA + fEncSrcBufSize ;
#endif      
      inJpgEncParam.dstBufferAddr = (unsigned int) fDstIonPA       ;      
      
    }

    if(fQuality >= 97)
    {
        inJpgEncParam.dstQuality = JPEG_ENCODE_QUALITY_Q97;
    } 
    else if(fQuality >= 95)
    {
        inJpgEncParam.dstQuality = JPEG_ENCODE_QUALITY_Q95;
    }
    else if(fQuality >= 92)
    {
        inJpgEncParam.dstQuality = JPEG_ENCODE_QUALITY_Q92;
    }
    else if(fQuality >= 90)
    {
        inJpgEncParam.dstQuality = JPEG_ENCODE_QUALITY_Q90;
    }
    else if(fQuality >= 87)
    {
        inJpgEncParam.dstQuality = JPEG_ENCODE_QUALITY_Q87;
    }
    else if(fQuality >= 84)
    {
        inJpgEncParam.dstQuality = JPEG_ENCODE_QUALITY_Q84;
    }
    else if(fQuality >= 80)
    {
        inJpgEncParam.dstQuality = JPEG_ENCODE_QUALITY_Q80;
    }
    else if(fQuality >= 74)
    {
        inJpgEncParam.dstQuality = JPEG_ENCODE_QUALITY_Q74;
    }
    else if(fQuality >= 64)
    {
        inJpgEncParam.dstQuality = JPEG_ENCODE_QUALITY_Q64;
    }
    else if(fQuality >= 60)
    {
        inJpgEncParam.dstQuality = JPEG_ENCODE_QUALITY_Q60;
    }
    else
    {
        inJpgEncParam.dstQuality = JPEG_ENCODE_QUALITY_Q48;
    }
    
    inJpgEncParam.isPhyAddr = 0;

    if(fIsAddSOI)
        inJpgEncParam.enableEXIF = 0;
    else
        inJpgEncParam.enableEXIF = 1;

    switch (fEncFormat)
    {
        case kENC_YUY2_Format:
            inJpgEncParam.dstFormat = JPEG_SAMPLING_FORMAT_YUY2;
            break;

        case kENC_UYVY_Format:
            inJpgEncParam.dstFormat = JPEG_SAMPLING_FORMAT_YVYU;
            break;

        case kENC_NV12_Format:
            inJpgEncParam.dstFormat = JPEG_SAMPLING_FORMAT_NV12;
            break;

        case kENC_NV21_Format:
            inJpgEncParam.dstFormat = JPEG_SAMPLING_FORMAT_NV21;
            break;

        default :
            JPG_LOG("Unsupport Encoder Format : %d", fEncFormat);
            return false;
            
    }
    
    if(JPEG_ENC_STATUS_OK != jpegEncConfigEncoder(encID, inJpgEncParam))
    {
        JPG_LOG("Config Encoder Fail");
        return false;
    }

    JPG_DBG(" JpgEncHal::trigger jpeg encoder");
    // Trigger Jpeg Encoder
    if(JPEG_ENC_STATUS_OK != jpegEncStart(encID))
    {
        JPG_LOG("Trigger Encoder Fail");
        return false;
    }

    JPG_DBG(" JpgEncHal::wait jpeg encoder");
    JPEG_ENC_RESULT_ENUM result;
    if(JPEG_ENC_STATUS_OK != jpegEncWaitIRQ(encID, JPEG_IRQ_TIMEOUT_ENC, encSize, &result)) 
    {
        JPG_LOG("Wait IRQ Fail");
        return false;
    }

    JPG_DBG(" JpgEncHal::wait jpeg encoder done");

    if( fMemType == JPEG_ENC_MEM_PMEM ){
      memcpy(fDstAddr,(void*)fEncDstPmemVA, fDstSize);
    } 


    JPG_DBG(" result:%d, size:%u", result, *encSize);
    
    if(result != JPEG_ENC_RESULT_DONE)
        return false;
   }
#endif /*JENC_HW_SUPPORT*/

   JPG_DBG("JpgEncHal::jpeg encoder done");
    return true;
}
    
