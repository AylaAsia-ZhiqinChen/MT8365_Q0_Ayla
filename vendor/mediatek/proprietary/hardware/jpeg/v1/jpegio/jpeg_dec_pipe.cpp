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

//#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

//#include "Trace.h" 
#define ATRACE_TAG ATRACE_TAG_GRAPHICS
#include "utils/Trace.h" 
//#define ATRACE_CALL(...) 

#include "jpeg_sw_dec.h"
  #include "jpeg_hal_dec.h"
#ifdef JPEG_HW_DECODE_COMP
  #include "jpeg_dec_hal.h"
  #include "jpeg_dec_comp.h"
#endif

#ifdef WEBP_HW_DECODE_COMP
  #include "webp_dec_comp.h"
#endif  

#include "img_common_def.h"


#include <img_mdp_pipe.h>
   
//#include "mdp_path.h"
//#include "mdp_element.h"
//#include "mdp_datatypes.h"


//#define USE_PMEM
#define TEST_LDVT
 

#ifdef USE_PMEM
#include <cutils/pmem.h>
//#include <cutils/memutil.h>
#endif

#ifdef LOG_TAG
#undef LOG_TAG
#endif


#undef bool
#define bool bool

//#define JPEG_PROFILING   

#define LOG_TAG "JpgDecPipe"

#define JPG_LOG(fmt, arg...)    ALOGW(fmt, ##arg)
#define JPG_DBG(fmt, arg...)    ALOGD(fmt, ##arg)
//#define JPG_DBG(fmt, arg...)

#define JPG_MDP_MAX_ELEMENT 3

#define JPEG_IRQ_TIMEOUT_DEC 5000  /*3000*/

//#define IMGDEC_MALLOC_M4U_ALIGN_SIZE (256)
#define IMGDEC_MALLOC_M4U_ALIGN_SIZE (1024)

#define IMGDEC_MALLOC_M4U_BS_ALIGN_SIZE (128)   //(64)

static unsigned int pixel_byte[10] = {2,3,3,4,4,4,4,4,1,1};


#define JPEG_PMEM_ALLOC(size, va, pa, fd) {}
#define JPEG_PMEM_FREE(va, size, fd) {}


#define IMGDEC_ALLOC(ptr, size, align, align_ptr) \
{  \
   ptr = (unsigned char *)malloc(size+align) ; \
   align_ptr = (unsigned char *)TO_CEIL(ptr, align);    \
   /*JPG_DBG("[IMGDEC_ALLOC] memory_alloc addr %x(%x), size %x, align %x, L:%d!!\n", (unsigned int)(ptr), (unsigned int)(align_ptr), size,align,__LINE__);*/ \
   /*memset(ptr, 0 , size+align);*/ \
}

#define IMGDEC_FREE(ptr) \
{  \
   /*JPG_DBG("[IMGDEC_FREE] memory_free addr %x, L:%d!!\n", (unsigned int)(ptr), __LINE__);*/ \
   if(ptr != NULL)   \
     free(ptr) ;  \
   ptr = NULL ;   \
}



static bool fail_return(const char msg[]) {
#if 1
    JPG_LOG("[JPEG Decoder] - %s", msg);
#endif
    return false;   // must always return false
}

static int code_return(const char msg[], int line,int code) {
#if 1
    JPG_LOG("[ImageDecPipe] - %s, L:%d!!\n", msg, line);
#endif
    return code;   // must always return code
}


#define DUMP_DEC_LVL_OUT 1
#define DUMP_DEC_LVL_STEP 2
#define DUMP_DEC_LVL_SCALE 3
#define DUMP_DEC_LVL_SRCBS 4

//#define ENABLE_IMG_CODEC_DUMP_RAW
unsigned int dump_width = 0;
unsigned int dump_height = 0;
unsigned int dump_fmt = 0;

int dumpBin2File(unsigned int level, const char filename[], unsigned int *index, unsigned char *SrcAddr, unsigned int size)
{
   
   FILE *fp = NULL;
   FILE *fpEn = NULL;
   unsigned char* cptr ;
   const char tag[64] = "DUMP_DEC_PIPE";
   char filepath[128];
   char dumpEn[128] ;

#ifndef ENABLE_IMG_CODEC_DUMP_RAW
   return false ;
#endif
   
   //sprintf(filepath, "/data/otis/dec_pipe_scaler_step_%04d.raw", fileNameIdx);   
   sprintf(  dumpEn, "//data//otis//%s_%d", tag, level);   
   sprintf(filepath, "//data//otis//%s_%04d_%d_%d_%d.raw", filename, *index,dump_fmt, dump_width, dump_height);   

   fpEn = fopen(dumpEn, "r");
   if(fpEn == NULL)
   {
       //JPG_LOG("Check Dump En is zero!!\n");
       return false;
   }
   fclose(fpEn);
      
   fp = fopen(filepath, "w");
   if (fp == NULL)
   {
       JPG_LOG("open Dump file fail: %s\n", filepath);
       return false;
   }

   JPG_LOG("\nDumpRaw -> %s, En %s, addr %x, size %x !!", filepath,dumpEn,(unsigned int)SrcAddr, size);                     
   cptr = (unsigned char*)SrcAddr ;
   for( unsigned int i=0;i<size;i++){  /* total size in comp */
     fprintf(fp,"%c", *cptr );  
     cptr++;
   }          
   
   fclose(fp); 
   //*index++;
   
   return true ;       
}



JpgDecHal::JpgDecHal()
{
    JPG_DBG("JpgDecPipe::JpgDecPipe");
    
    //fImgDecType = IMG_DEC_TYPE_WEBP ;
    fImgDecType = IMG_DEC_TYPE_JPEG ;    
    
    fOutPmemVA = NULL;





    islock = false;
    isDither = false;
    isRangeDecode = false;
    fScaleType = IMG_RESCALE_TYPE_NONE;

    fOutWidth = fOutHeight = 0;
    fJpgWidth = fJpgHeight = 0;
    fSrcAddr = fDstAddr = NULL;
    fProcHandler = NULL;
    
    fUseSwDecoder = 1;
    fPreferDecDownSample = 0;
    
    //fMemType = fMemTypeDefault = IMG_MEM_TYPE_PMEM ;
    fMemType = fMemTypeDefault = IMG_MEM_TYPE_M4U ;
    fScalerBufferType = IMG_MEM_DEC_BUF_SCALER ;
    

    
   //fScaleType =  IMG_RESCALE_TYPE_TILE ;
   fScaleType =  IMG_RESCALE_TYPE_LINK ; 
   


    fUseImgRszCmodel = 0;   
    
    for(int i =0;i<3;i++){
      fDecDstBufAddrVA0[i] = NULL;
      fDecDstBufAddrVA1[i] = NULL;
      fDecDstBufAddrPA0[i]  = 0;
      fDecDstBufAddrPA1[i]  = 0;
      fDecDstBufAddrVA0_org[i] = NULL; 
      fDecDstBufAddrVA1_org[i] = NULL; 
      
    }
    fIonEn = false;
    fDstFD = -1;
    fDecBRZ = 0;
    fTotalBRZ = 0;
    fTileBufRowMargin = 0;
    fPQlevel = 0;
    fQualitylevel = 0;
    
    
}



JpgDecHal::~JpgDecHal()
{
    JPG_DBG("JpgDecPipe::~JpgDecPipe");

    unlock();

    
}

void JpgDecHal::setRangeDecode(JUINT32 left, JUINT32 top, JUINT32 right, JUINT32 bottom)
{
    isRangeDecode = true;
    fLeft = left;
    fTop = top;
    fRight = right;
    fBottom = bottom;
}
 
//#define USE_VDEC_SO

bool JpgDecHal::setScalerType(JUINT32 scalerType)
{  
   JPG_LOG("::[setScalerType] set fScaleType %d->%d, L:%d!!\n", fScaleType, scalerType,__LINE__);
   if(scalerType == IMG_RESCALE_TYPE_LINK)
      return false ;
   fScaleType = scalerType; 
   return true ;
} 


bool JpgDecHal::lock()
{

    if(!islock){
      JPG_LOG("::[LOCK] lock, islock %d!!\n", islock);
      ATRACE_CALL();
      
#ifdef USE_VDEC_SO      
      if(fImgDecType == IMG_DEC_TYPE_WEBP){
        pJpgDecComp = new WebpDecComp();      
      }else
#endif             
      {
        if(fUseSwDecoder)
          pJpgDecComp = new JpgSwDec();  
#ifdef JPEG_HW_DECODE_COMP          
        else
          pJpgDecComp = new JpgDecComp();       
#endif          
      }
      

      if( !pJpgDecComp->lock() ) 
      {
         
         if(fImgDecType == IMG_DEC_TYPE_WEBP){
           return fail_return("Jpeg decoder Comp resource is busy");
         }else if(fImgDecType == IMG_DEC_TYPE_JPEG ){
            if(fUseSwDecoder){
              return code_return("SW decoder Lock resource fail!! ",__LINE__,false);            
            }else if( fScaleType == IMG_RESCALE_TYPE_LINK){ 
              return code_return("Jpeg decoder Lock resource for GDMA fail!! ",__LINE__,false);             
            }else{
              delete pJpgDecComp ;            
              pJpgDecComp = new JpgSwDec();  
              if( !pJpgDecComp->lock() ) 
                return code_return("change to SW decoder Lock resource fail!! ",__LINE__,false);
              fUseSwDecoder = 1;
            }
         }else{
            return code_return("Jpeg decoder Lock resource fail!! ",__LINE__,false);            
         }
      }
 
      if(fUseSwDecoder){
        fScaleType =  IMG_RESCALE_TYPE_TILE ;
        JPG_LOG("::[LOCK] use SW decoder lock scaler type to %d!!\n", fScaleType);
      }
#if 1
      pImgCdp = new ImgCdpPipe();
#else
      pImgCdp = new ImgMdpPipe();
#endif

#ifdef PRELOCK_SCALER      
      fScaleType = lockScaler(fScaleType);
      JPG_LOG("::[LOCK] lock scaler type is %d!!\n", fScaleType);
      
      if(fScaleType == IMG_RESCALE_TYPE_NONE ){
         JPG_LOG("::[LOCK] lock scaler type fail!!\n");
         pJpgDecComp->unlock();
         return fail_return("Scaler resource is busy");
      }
#endif       
      islock = true;
      

    }
    return true;
}

bool JpgDecHal::lockResizer()
{
#ifdef PRELOCK_SCALER
  return true ;
#endif
    if(islock){
      //JPG_LOG("::[LOCKR] lock, islock %d!!\n", islock);
      JPG_LOG("::[LOCKR] lock scaler type is %d!!\n", fScaleType);
      fScaleType = lockScaler(fScaleType);
      
      if(fScaleType == IMG_RESCALE_TYPE_NONE ){
         JPG_LOG("::[LOCKR] lock scaler type fail!!\n");
         pJpgDecComp->unlock();
         return fail_return("Scaler resource is busy");
      }
    }   
    
    return true ;
}

 
bool JpgDecHal::unlock()
{
    if(islock)
    {        
        JPG_LOG("::[UNLOCK] unlock , islock %d!!\n", islock);
        ATRACE_CALL();
        
	      //android::Tracer::traceBegin(ATRACE_TAG_GRAPHICS,"UNLOCK_DECODER");
        pJpgDecComp->unlock();
        //android::Tracer::traceEnd(ATRACE_TAG_GRAPHICS);  
	      //android::Tracer::traceBegin(ATRACE_TAG_GRAPHICS,"UNLOCK_CDP");
        pImgCdp->unlock();
        //android::Tracer::traceEnd(ATRACE_TAG_GRAPHICS);  
 
        if(fMemType == IMG_MEM_TYPE_PMEM){
             JPG_LOG("memcpy L:%d: %x, %x, %x!!\n", __LINE__, fDstAddr, fOutPmemVA, fOutBufSize);
             memcpy(fDstAddr, fOutPmemVA, fOutBufSize);
             
        }
        if( pJpgDecComp ) delete pJpgDecComp ;    
        pJpgDecComp = NULL;
        
        if( pImgCdp ) delete pImgCdp ;          
        pImgCdp = NULL;
        

	      //android::Tracer::traceBegin(ATRACE_TAG_GRAPHICS,"DEC_PIPE_FREE_MEM");
        freeDecMEM();
        //android::Tracer::traceEnd(ATRACE_TAG_GRAPHICS);  
                         
        if(fMemType == IMG_MEM_TYPE_PMEM){  //free IO MEM
             JPEG_PMEM_FREE(  fOutPmemVA,    fOutBufSize,   fOutPmemFD);
        }
        
        islock = false;
   }
    return true;
}
  
bool JpgDecHal::checkParam()
{
#if 0   
    if(fScaleType == IMG_RESCALE_TYPE_TILE && ((fOutWidth&0x01)  || (fOutHeight&0x01)) )
    {
        JPG_LOG("NOT SUPPORT JPEG width/height scale Tile with %d/%d!!\n", fOutWidth, fOutHeight);
        return false;
    }
#endif       
   
    if(fJpgWidth == 0 || fJpgHeight == 0)
    {
        JPG_ERR("Invalide JPEG width/height %u/%u", fJpgWidth, fJpgHeight);
        return false;
    }
    
    if(fOutWidth < 3 || fOutHeight < 3)
    {
        JPG_ERR("Invalid width/height %u/%u [Range 3~8191]", fOutWidth, fOutHeight);
        return false;
    }

    if(fOutWidth > 8191 || fOutHeight > 8191)
    {
        JPG_ERR("Invalid width/height %u/%u [Range 3~8191]", fOutWidth, fOutHeight);
        return false;
    }

    if(fSrcAddr == NULL || fDstAddr == NULL) 
    {
        return fail_return("Invalide Address");
    }
    
    if(fScaleType == IMG_RESCALE_TYPE_NONE)
    {
       JPG_LOG("Invalid Scaling Type %d!!\n", fScaleType);
        return code_return("chcek Scaling Type error, ",__LINE__,false);
    }
    
    return true;
}


bool JpgDecHal::parse()
{
   
    ATRACE_CALL();
    if(fSrcAddr == NULL)
    {
        return fail_return("source address is null");
    }

    pJpgDecComp->setSrcAddr(fSrcAddr);
    pJpgDecComp->setSrcSize( TO_FLOOR(fSrcBufferSize, (IMGDEC_MALLOC_M4U_BS_ALIGN_SIZE)), fSrcBsSize);
    if(!pJpgDecComp->parse())
      return code_return("decoder parse fail", __LINE__, false);



    fJpgWidth  = pJpgDecComp->getJpgWidth(); 
    fJpgHeight = pJpgDecComp->getJpgHeight();
    
    if(fImgDecType == IMG_DEC_TYPE_WEBP && fScaleType == IMG_RESCALE_TYPE_LINK){
       if(fJpgWidth > 8191)
         return code_return("webp parse not support width > 8191 in LINK mode!!", __LINE__, false);
    }
    

#ifdef ENABLE_IMG_CODEC_DUMP_RAW    
    dump_width =  fJpgWidth  ;
    dump_height = fJpgHeight ;

#endif
        
    return true;
}


bool JpgDecHal::allocDecMEM()
{
   if( fScalerBufferType != IMG_MEM_DEC_BUF_TOP )
      return true ;
   ATRACE_CALL();
   if(fMemType == IMG_MEM_TYPE_PMEM){
     JPEG_PMEM_ALLOC( fDecDstBufSize[0], fDecDstBufAddrVA0[0], fDecDstBufAddrPA0[0], fDecPmemFD[0] ) ;
     JPEG_PMEM_ALLOC( fDecDstBufSize[1], fDecDstBufAddrVA0[1], fDecDstBufAddrPA0[1], fDecPmemFD[1] ) ;
     JPEG_PMEM_ALLOC( fDecDstBufSize[2], fDecDstBufAddrVA0[2], fDecDstBufAddrPA0[2], fDecPmemFD[2] ) ;
     if( fScaleType == IMG_RESCALE_TYPE_LINK ){
       JPEG_PMEM_ALLOC( fDecDstBufSize[0], fDecDstBufAddrVA1[0], fDecDstBufAddrPA1[0], fDecPmemFD[3] ) ;
       JPEG_PMEM_ALLOC( fDecDstBufSize[1], fDecDstBufAddrVA1[1], fDecDstBufAddrPA1[1], fDecPmemFD[4] ) ;
       JPEG_PMEM_ALLOC( fDecDstBufSize[2], fDecDstBufAddrVA1[2], fDecDstBufAddrPA1[2], fDecPmemFD[5] ) ;        
     }
     
   }else if(fMemType == IMG_MEM_TYPE_M4U){
     IMGDEC_ALLOC(fDecDstBufAddrVA0_org[0], fDecDstBufSize[0], IMGDEC_MALLOC_M4U_ALIGN_SIZE, fDecDstBufAddrVA0[0]) ;
     IMGDEC_ALLOC(fDecDstBufAddrVA0_org[1], fDecDstBufSize[1], IMGDEC_MALLOC_M4U_ALIGN_SIZE, fDecDstBufAddrVA0[1]) ;
     IMGDEC_ALLOC(fDecDstBufAddrVA0_org[2], fDecDstBufSize[2], IMGDEC_MALLOC_M4U_ALIGN_SIZE, fDecDstBufAddrVA0[2]) ;
     
     
     if( fScaleType == IMG_RESCALE_TYPE_LINK ){
       IMGDEC_ALLOC(fDecDstBufAddrVA1_org[0], fDecDstBufSize[0], IMGDEC_MALLOC_M4U_ALIGN_SIZE, fDecDstBufAddrVA1[0]) ;
       IMGDEC_ALLOC(fDecDstBufAddrVA1_org[1], fDecDstBufSize[1], IMGDEC_MALLOC_M4U_ALIGN_SIZE, fDecDstBufAddrVA1[1]) ;
       IMGDEC_ALLOC(fDecDstBufAddrVA1_org[2], fDecDstBufSize[2], IMGDEC_MALLOC_M4U_ALIGN_SIZE, fDecDstBufAddrVA1[2]) ;
       
     }
   }

   
   return true ;
   
}
bool JpgDecHal::freeDecMEM()
{
   if( fScalerBufferType != IMG_MEM_DEC_BUF_TOP )
      return true ;
            
   ATRACE_CALL();
   if(fMemType == IMG_MEM_TYPE_PMEM){
     JPEG_PMEM_FREE( fDecDstBufAddrVA0[0], fDecDstBufSize[0], fDecPmemFD[0] ) ;
     JPEG_PMEM_FREE( fDecDstBufAddrVA0[1], fDecDstBufSize[1], fDecPmemFD[1] ) ;
     JPEG_PMEM_FREE( fDecDstBufAddrVA0[2], fDecDstBufSize[2], fDecPmemFD[2] ) ;
     if(fScaleType == IMG_RESCALE_TYPE_LINK){
       JPEG_PMEM_FREE( fDecDstBufAddrVA1[0], fDecDstBufSize[0], fDecPmemFD[3] ) ;
       JPEG_PMEM_FREE( fDecDstBufAddrVA1[1], fDecDstBufSize[1], fDecPmemFD[4] ) ;
       JPEG_PMEM_FREE( fDecDstBufAddrVA1[2], fDecDstBufSize[2], fDecPmemFD[5] ) ;      
     }
   }else if(fMemType == IMG_MEM_TYPE_M4U){
     IMGDEC_FREE(fDecDstBufAddrVA0_org[0]);
     IMGDEC_FREE(fDecDstBufAddrVA0_org[1]);
     IMGDEC_FREE(fDecDstBufAddrVA0_org[2]);  
     
     if(fScaleType == IMG_RESCALE_TYPE_LINK){
       IMGDEC_FREE(fDecDstBufAddrVA1_org[0]);
       IMGDEC_FREE(fDecDstBufAddrVA1_org[1]);
       IMGDEC_FREE(fDecDstBufAddrVA1_org[2]);        
     }
   }
   
   return true ;
   
}      
      

/*
// Tile step
1. config JPEG
2. get CDP tdri setting
3. get cdp nxt row, jpeg decode row 
4. wait CDP, config + trigger CDP
5. repeat 3~4

*/
 
 
/*
//GDMA step
1. parse JPEG
2. config + trigger CDP
3. cofig + trigger + wait JPEG
4. wait GDMA

*/

JUINT32 JpgDecHal::lockScaler(JUINT32 preferType)
{
   JUINT32 cdpMode ;
    //set connection type to gdma -> tile -> fail
    if(preferType ==  IMG_RESCALE_TYPE_LINK ){ 
      cdpMode = ImgCdpPipe::SCALER_MODE_LINK ;   
    }else{
      cdpMode = ImgCdpPipe::SCALER_MODE_TILE ;  
    }
    for(; !pImgCdp->lock(cdpMode) ;){
#if 0 //no need to change scaling mode      
      if(cdpMode == ImgCdpPipe::SCALER_MODE_LINK ){
        //if lock gdma mode fail, try concurrcy tile mode
        preferType =  IMG_RESCALE_TYPE_TILE ;
        cdpMode = ImgCdpPipe::SCALER_MODE_TILE ;      
      }else
#endif         
      {
        return code_return("set cdp connection type fail, ",__LINE__, IMG_RESCALE_TYPE_NONE);
      }      
    }   
    
   return preferType ;
   
   
}
 



bool JpgDecHal::onConfigCompTile()
{
   
   JUINT32 decMode;
   ATRACE_CALL();





    if(fScaleType ==  IMG_RESCALE_TYPE_TILE ){
      decMode = ImageDecoder::IMG_DEC_MODE_TILE ;      
      if(fPreferDecDownSample){
        //for(fDecBRZ = 0; fDecBRZ < 3 && (fJpgWidth>>fDecBRZ) > fOutWidth ; fDecBRZ++);
        for(fTotalBRZ = 0; (fJpgWidth>>fTotalBRZ) > fOutWidth ; fTotalBRZ++);
        fDecBRZ = (fTotalBRZ > 3)? 3 : fTotalBRZ;
      }
    }else{
      decMode = ImageDecoder::IMG_DEC_MODE_LINK ;

      for(fDecBRZ = 0; fDecBRZ <= 3 && (fJpgWidth>>fDecBRZ) > SCALER_LINK_MAX_WIDTH ; fDecBRZ++);
         
    }    
    pJpgDecComp->setBRZ(fDecBRZ);
    pJpgDecComp->setQuality(fQualitylevel);
      
    if(!pJpgDecComp->Config(decMode)){
        return code_return("JPEG Config fail,", __LINE__, false);
    }

    
    fDecOutFormat = pJpgDecComp->getOutFormat();
    
    //CDP source format
    if(fDecOutFormat == ImageDecoder::IMGDEC_3PLANE_YUV420)
      fScalerInFormat = ImgCdpPipe::SCALER_IN_3P_YUV420 ;
    else if(fDecOutFormat == ImageDecoder::IMGDEC_3PLANE_YUV422)
      fScalerInFormat = ImgCdpPipe::SCALER_IN_3P_YUV422 ;
    else if(fDecOutFormat == ImageDecoder::IMGDEC_3PLANE_GRAY)
      fScalerInFormat = ImgCdpPipe::SCALER_IN_1P_GRAY ;
    else if(fDecOutFormat == ImageDecoder::IMGDEC_3PLANE_YUV444)
      fScalerInFormat = ImgCdpPipe::SCALER_IN_3P_YUV444 ;         
    else
      fScalerInFormat = ImgCdpPipe::SCALER_IN_3P_YUV422 ;


    if(fOutFormat == kABGR_8888_Format){
       fCdpDstFormat = ImgCdpPipe::SCALER_OUT_1P_ARGB8888 ; //this is ABGR8888
    }else if(fOutFormat == kARGB_8888_Format){ 
       fCdpDstFormat = ImgCdpPipe::SCALER_OUT_1P_ARGB8888 ; //TODO modify
    }else if(fOutFormat == kRGB_888_Format || fOutFormat == kBGR_888_Format){
       fCdpDstFormat = ImgCdpPipe::SCALER_OUT_1P_RGB888;  //eImgFmt_RGB888  ;
    }else if(fOutFormat == kRGB_565_Format){
       fCdpDstFormat = ImgCdpPipe::SCALER_OUT_1P_RGB565;  //eImgFmt_RGB565  ;
    }else if(fOutFormat == kYUY2_Pack_Format){
       fCdpDstFormat = ImgCdpPipe::SCALER_OUT_1P_YUV422;  //eImgFmt_YUY2 ;
    }else if(fOutFormat == kUYVY_Pack_Format){
       fCdpDstFormat = ImgCdpPipe::SCALER_OUT_1P_YUV422;  //eImgFmt_UYVY  ;
    }else if(fOutFormat == kYVU420_3PLANE_Format){ 
       fCdpDstFormat = ImgCdpPipe::SCALER_OUT_3P_YVU420 ; 
    }else if(fOutFormat == kYVU420_YV12_Format){ 
       fCdpDstFormat = ImgCdpPipe::SCALER_OUT_3P_YV12 ; 
    }else{
       fCdpDstFormat = ImgCdpPipe::SCALER_OUT_1P_ARGB8888 ; 
    }
    

    
    fImgMcuRowNum = pJpgDecComp->getMcuRow();

	//decode image MCU info
    fImgMcuHeight[0] = pJpgDecComp->getMcuHeightY();
    fImgMcuHeight[1] = fImgMcuHeight[2] = pJpgDecComp->getMcuHeightC();
   
    //img height
    fDecDstImgHeight[0] = pJpgDecComp->getLumaImgHeight(); 
    fDecDstImgHeight[1] = fDecDstImgHeight[2] = pJpgDecComp->getCbCrImgHeight();  

    // buffer stride
    fDecDstBufStride[0] = pJpgDecComp->getMinLumaBufStride(); 
    fDecDstBufStride[1] = fDecDstBufStride[2] = pJpgDecComp->getMinCbCrBufStride();    

    // image width/height
    fDecDstImgWidth[0]  = pJpgDecComp->getLumaImgStride();
    fDecDstImgWidth[1] = fDecDstImgWidth[2] = pJpgDecComp->getCbCrImgStride();



    JPG_LOG("::[CONFIG_COMP] decInfo imgY:%d %d, imgC:%d %d, bufS %d %d !!\n", fDecDstImgWidth[0], fDecDstImgHeight[0], fDecDstImgWidth[1], fDecDstImgHeight[1]
    ,fDecDstBufStride[0], fDecDstBufStride[1]);  


    //configure scaling ratio 
    ConfigScalingRatio();


	if( fScalerBufferType == IMG_MEM_DEC_BUF_SCALER ){

      unsigned int pSrcBuf[3];
      unsigned int pSrcStride[3];		
      unsigned int i=0;
      
      for(i=0;i<3;i++){
        pSrcStride[i] = fDecDstBufStride[i] ;
      }
      
      if(!pImgCdp->getSrcBuffer(pSrcBuf, pSrcStride, &fTileBufRowNum)){
	  	  return code_return("scaler getSrcBuffer fail,", __LINE__, false);
	    } 
	  
      fDecDstBufAddrVA0[0] = (unsigned char*)pSrcBuf[0];
      fDecDstBufAddrVA0[1] = (unsigned char*)pSrcBuf[1];
      fDecDstBufAddrVA0[2] = (unsigned char*)pSrcBuf[2];	
	  fDecDstBufSize[0] = fDecDstBufStride[0]*fImgMcuHeight[0]*fTileBufRowNum;
	  fDecDstBufSize[1] = fDecDstBufSize[2] = fDecDstBufStride[1]*fImgMcuHeight[1]*fTileBufRowNum;
	}else{
	
#if 0 //def TEST_LDVT    
    fTileBufRowNum = 16;//30;//fImgMcuRowNum; //
    fUseImgRszCmodel = 0;
    pImgCdp->setCmodel(fUseImgRszCmodel) ; //use software converter     
#else
    
    if( fDecDstImgHeight[0] >= fOutHeight ) {  //scale down
      //mcuRow = 12*inH/(outH*MCU_H)
      fTileBufRowNum = (12*fDecDstImgHeight[0])/(fOutHeight*fImgMcuHeight[0]) ;
      if (fImgDecType == IMG_DEC_TYPE_WEBP && fTileBufRowNum < 30) fTileBufRowNum  = 30;
      else if(fTileBufRowNum < 20 ) fTileBufRowNum  = 20;
      
    }else
      fTileBufRowNum = 12 ; //scale up

#endif   
    if(fScaleType == IMG_RESCALE_TYPE_LINK)
      fTileBufRowNum = 1;
    // buffer size
    fDecDstBufSize[0] = pJpgDecComp->getMinLumaBufSize()* (fTileBufRowNum + fTileBufRowMargin); 
    fDecDstBufSize[1] = fDecDstBufSize[2] = pJpgDecComp->getMinCbCrBufSize()* (fTileBufRowNum + fTileBufRowMargin);


    }
   


    JPG_LOG("::[CONFIG_COMP] type %d, w %d, brz %d, sFMT %d->%d, rowBuf %d/%d, MCU_H %d %d!!\n", fScaleType, fJpgWidth, fDecBRZ ,fScalerInFormat,fOutFormat, fTileBufRowNum, fImgMcuRowNum, fImgMcuHeight[0], fImgMcuHeight[1]);  



    //=====================================================================
    //    step0: Config & Query Jpeg Decoder Output Buffer requirement
    //=====================================================================



    if(!allocDecMEM()){
        return fail_return("JPEG Tile DecMEM alloc fail!!");
    }

    //==============================================
    //    step1: Config Jpeg Decoder 
    //==============================================

    
    pJpgDecComp->setTileBufNum(fTileBufRowNum);    
    pJpgDecComp->setFirstDecRow(1);         

    pJpgDecComp->setSrcAddr(fSrcAddr);
    pJpgDecComp->setSrcSize( TO_FLOOR(fSrcBufferSize, (IMGDEC_MALLOC_M4U_BS_ALIGN_SIZE)), fSrcBsSize);
          
    pJpgDecComp->setDstBufSize(fDecDstBufSize[0],fDecDstBufSize[1]) ;
    pJpgDecComp->setDstAddr0((unsigned int)fDecDstBufAddrVA0[0], (unsigned int)fDecDstBufAddrVA0[1], (unsigned int)fDecDstBufAddrVA0[2]);
    
    if(fScaleType == IMG_RESCALE_TYPE_LINK) 
      pJpgDecComp->setDstAddr1((unsigned int)fDecDstBufAddrVA1[0], (unsigned int)fDecDstBufAddrVA1[1], (unsigned int)fDecDstBufAddrVA1[2]);

    if(fMemType == IMG_MEM_TYPE_PMEM){
      pJpgDecComp->setDstAddrPA0(fDecDstBufAddrPA0[0], fDecDstBufAddrPA0[1], fDecDstBufAddrPA0[2]);
      if(fScaleType == IMG_RESCALE_TYPE_LINK) 
         pJpgDecComp->setDstAddrPA1(fDecDstBufAddrPA1[0], fDecDstBufAddrPA1[1], fDecDstBufAddrPA1[2]);   
    }    

    if( fImgDecType == IMG_DEC_TYPE_WEBP ){ //fScaleType == IMG_RESCALE_TYPE_LINK &&
       if(!pJpgDecComp->StartConfig()){
         return code_return("decoder StartConfiguration fail,", __LINE__, false);
       }      
             
    }
      
    //==========================================================
    //    step2: Config Tile Mode & get ImgScalerPipe Tile Info
    //==========================================================

    if(fScalerBufferType == IMG_MEM_DEC_BUF_TOP)
	    ConfigScalingSrcBuffer();

    JPG_LOG("DECPIPE: decode_brz %d (%d %d)->(%d %d), scaler (%d %d)->(%d %d) !!\n",
     fDecBRZ, fJpgWidth, fJpgHeight, fDecDstImgWidth[0], fDecDstImgHeight[0]
    , fJpgWidth >> fDecBRZ, fJpgHeight >> fDecBRZ
    , fOutWidth, fOutHeight);
   
   return true ;
}

      

bool JpgDecHal::ConfigScalingRatio()
{

    //==========================================================
    //    step2: Config Tile Mode & get ImgScalerPipe Tile Info
    //==========================================================

   
    //set configuration
    if(fScaleType == IMG_RESCALE_TYPE_LINK){
      bool srcJpg = (fImgDecType == IMG_DEC_TYPE_JPEG)? 1: 0 ;
      pImgCdp->setGtrSrcIsJpeg(srcJpg);
    }

    pImgCdp->setSrcImageColor(fScalerInFormat);
    //set tile only configure
    pImgCdp->setSrcImageSize(fDecDstImgWidth[0],  fDecDstImgHeight[0], fDecDstImgWidth[1], fDecDstImgHeight[1]);    
    
    JPG_LOG("DECPIPE: config_brz w/h %d %d -> %d %d -> %d %d, brz %d/%d, L:%d!!\n", fJpgWidth, fJpgHeight, fJpgWidth >> fDecBRZ, fJpgHeight >> fDecBRZ,fOutWidth,fOutHeight , fDecBRZ, fTotalBRZ, __LINE__); 
    //if(fPreferDecDownSample && fDecBRZ)
    //  pImgCdp->setCropImageSize(fOutWidth<<(fTotalBRZ-fDecBRZ), fOutHeight<<(fTotalBRZ-fDecBRZ));
    //else
    pImgCdp->setCropImageSize(fJpgWidth >> fDecBRZ, fJpgHeight >> fDecBRZ);
    pImgCdp->setSrcImgMcuNum(fImgMcuRowNum);
    pImgCdp->setSrcRowHeight(fImgMcuHeight[0],fImgMcuHeight[1]);

    

    pImgCdp->setTdsp(fPQlevel); 
    pImgCdp->setDstImageColor(fCdpDstFormat);
    pImgCdp->setDstImageSize(fOutWidth, fOutHeight);    
    pImgCdp->setDstBufSize(fOutBufSize, fOutCbBufSize, fOutCrBufSize);
    pImgCdp->setDstAddr(fCdpDstBufAddr, (unsigned int)fDstCbAddr, (unsigned int)fDstCrAddr); 
    pImgCdp->setDstHandler((unsigned long)fProcHandler);
    //pImgCdp->setDstAddrPA(fOutBufAddrPA);
    
    if(fIonEn && fDstFD >= 0)
      pImgCdp->setDstFD(fDstFD);



	
/*                    
    if(fScaleType == IMG_RESCALE_TYPE_TILE ){
      if(!fUseImgRszCmodel){
         if(!pImgCdp->TileInit()){
           return code_return("Scaler Tile Init fail,", __LINE__, false);
         }
      }
    }
*/    
             
   return true ;

}



bool JpgDecHal::ConfigScalingSrcBuffer()
{

    pImgCdp->setTileBufRowNum(fTileBufRowNum);
    pImgCdp->setSrcBufSize(fDecDstBufSize[0], fDecDstBufSize[1]);    
    pImgCdp->setSrcBufStride(fDecDstBufStride[0], fDecDstBufStride[1]);

#if 1    
    pImgCdp->setSrcAddr0((unsigned int)fDecDstBufAddrVA0[0], (unsigned int)fDecDstBufAddrVA0[1], (unsigned int)fDecDstBufAddrVA0[2]);

    if(fScaleType == IMG_RESCALE_TYPE_LINK) 
      pImgCdp->setSrcAddr1((unsigned int)fDecDstBufAddrVA1[0], (unsigned int)fDecDstBufAddrVA1[1], (unsigned int)fDecDstBufAddrVA1[2]);
    
    if(fMemType == IMG_MEM_TYPE_PMEM ) {
      pImgCdp->setSrcAddrPA0(fDecDstBufAddrPA0[0], fDecDstBufAddrPA0[1], fDecDstBufAddrPA0[2]);      
      if(fScaleType == IMG_RESCALE_TYPE_LINK) 
         pImgCdp->setSrcAddrPA1(fDecDstBufAddrPA1[0], fDecDstBufAddrPA1[1], fDecDstBufAddrPA1[2]);
    }
#else    
    pImgCdp->setSrcAddr0((unsigned int)fDecDstBufAddrVA1[0], (unsigned int)fDecDstBufAddrVA0[1], (unsigned int)fDecDstBufAddrVA0[2]);

    if(fScaleType == IMG_RESCALE_TYPE_LINK) 
      pImgCdp->setSrcAddr1((unsigned int)fDecDstBufAddrVA0[0], (unsigned int)fDecDstBufAddrVA1[1], (unsigned int)fDecDstBufAddrVA1[2]);
    
    if(fMemType == IMG_MEM_TYPE_PMEM ) {
      pImgCdp->setSrcAddrPA0(fDecDstBufAddrPA0[0], fDecDstBufAddrPA0[1], fDecDstBufAddrPA0[2]);      
      if(fScaleType == IMG_RESCALE_TYPE_LINK) 
         pImgCdp->setSrcAddrPA1(fDecDstBufAddrPA1[0], fDecDstBufAddrPA1[1], fDecDstBufAddrPA1[2]);
    }     
#endif

  return true ;


}





bool JpgDecHal::onConfigComp()
{
   
   JUINT32 decMode;
   ATRACE_CALL();


    if(fScaleType ==  IMG_RESCALE_TYPE_TILE ){
      decMode = ImageDecoder::IMG_DEC_MODE_TILE ;      
    }else{
      decMode = ImageDecoder::IMG_DEC_MODE_LINK ;

      for(fDecBRZ = 0; fDecBRZ <= 3 && (fJpgWidth>>fDecBRZ) > SCALER_LINK_MAX_WIDTH ; fDecBRZ++);
         
    }    
    pJpgDecComp->setBRZ(fDecBRZ);
      
    if(!pJpgDecComp->Config(decMode)){
        return code_return("JPEG Config fail,", __LINE__, false);
    }


    
    fImgMcuRowNum = pJpgDecComp->getMcuRow();
    fImgMcuHeight[0] = pJpgDecComp->getMcuHeightY();
    fImgMcuHeight[1] = 
    fImgMcuHeight[2] = pJpgDecComp->getMcuHeightC();
   

    fDecDstImgHeight[0] = pJpgDecComp->getLumaImgHeight(); 

    fDecDstImgHeight[1] = fDecDstImgHeight[2] = pJpgDecComp->getCbCrImgHeight();  

#if 0 //def TEST_LDVT    
    fTileBufRowNum = 16;//30;//fImgMcuRowNum; //
    fUseImgRszCmodel = 0;
    pImgCdp->setCmodel(fUseImgRszCmodel) ; //use software converter     
#else
    
    if( fDecDstImgHeight[0] >= fOutHeight ) {  //scale down
      //mcuRow = 12*inH/(outH*MCU_H)
      fTileBufRowNum = (12*fDecDstImgHeight[0])/(fOutHeight*fImgMcuHeight[0]) ;
      if (fImgDecType == IMG_DEC_TYPE_WEBP && fTileBufRowNum < 30) fTileBufRowNum  = 30;
      else if(fTileBufRowNum < 20 ) fTileBufRowNum  = 20;
      
    }else
      fTileBufRowNum = 12 ; //scale up

#endif   
   
    if(fScaleType == IMG_RESCALE_TYPE_LINK)
      fTileBufRowNum = 1;
    
    fDecOutFormat = pJpgDecComp->getOutFormat();
    
    //CDP source format
    if(fDecOutFormat == ImageDecoder::IMGDEC_3PLANE_YUV420)
      fScalerInFormat = ImgCdpPipe::SCALER_IN_3P_YUV420 ;
    else if(fDecOutFormat == ImageDecoder::IMGDEC_3PLANE_YUV422)
      fScalerInFormat = ImgCdpPipe::SCALER_IN_3P_YUV422 ;
    else if(fDecOutFormat == ImageDecoder::IMGDEC_3PLANE_GRAY)
      fScalerInFormat = ImgCdpPipe::SCALER_IN_1P_GRAY ;
    else if(fDecOutFormat == ImageDecoder::IMGDEC_3PLANE_YUV444)
      fScalerInFormat = ImgCdpPipe::SCALER_IN_3P_YUV444 ;
    else
      fScalerInFormat = ImgCdpPipe::SCALER_IN_3P_YUV422 ;


    if(fOutFormat == kABGR_8888_Format){
       fCdpDstFormat = ImgCdpPipe::SCALER_OUT_1P_ARGB8888 ; //this is ABGR8888
    }else if(fOutFormat == kARGB_8888_Format){ 
       fCdpDstFormat = ImgCdpPipe::SCALER_OUT_1P_ARGB8888 ; //TODO modify
    }else if(fOutFormat == kRGB_888_Format || fOutFormat == kBGR_888_Format){
       fCdpDstFormat = ImgCdpPipe::SCALER_OUT_1P_RGB888;  //eImgFmt_RGB888  ;
    }else if(fOutFormat == kRGB_565_Format){
       fCdpDstFormat = ImgCdpPipe::SCALER_OUT_1P_RGB565;  //eImgFmt_RGB565  ;
    }else if(fOutFormat == kYUY2_Pack_Format){
       fCdpDstFormat = ImgCdpPipe::SCALER_OUT_1P_YUV422;  //eImgFmt_YUY2 ;
    }else if(fOutFormat == kUYVY_Pack_Format){
       fCdpDstFormat = ImgCdpPipe::SCALER_OUT_1P_YUV422;  //eImgFmt_UYVY  ;
    }else if(fOutFormat == kYVU420_3PLANE_Format){ 
       fCdpDstFormat = ImgCdpPipe::SCALER_OUT_3P_YVU420 ; 
    }else if(fOutFormat == kYVU420_YV12_Format){ 
       fCdpDstFormat = ImgCdpPipe::SCALER_OUT_3P_YV12 ; 
    }else{
       fCdpDstFormat = ImgCdpPipe::SCALER_OUT_1P_ARGB8888 ; 
    }
    
    JPG_LOG("::[CONFIG_COMP] type %d, w %d, brz %d, sFMT %d->%d, rowBuf %d/%d, MCU_H %d %d!!\n", fScaleType, fJpgWidth, fDecBRZ ,fScalerInFormat,fOutFormat, fTileBufRowNum, fImgMcuRowNum, fImgMcuHeight[0], fImgMcuHeight[1]);  



    //=====================================================================
    //    step0: Config & Query Jpeg Decoder Output Buffer requirement
    //=====================================================================

#if 0 
    //set connection type to gdma -> tile -> fail
    if(fScaleType ==  IMG_RESCALE_TYPE_TILE ){
      fCdpMode = ImgCdpPipe::SCALER_MODE_TILE ;      
    }else{
      fCdpMode = ImgCdpPipe::SCALER_MODE_LINK ;
    }
    for(; !pImgCdp->setConnectType(fCdpMode) ;){
      if(fCdpMode == ImgCdpPipe::SCALER_MODE_LINK ){
        fScaleType ==  IMG_RESCALE_TYPE_TILE ;
        fCdpMode = ImgCdpPipe::SCALER_MODE_TILE ;      
      }else{
        return code_return("set cdp connection type fail, ",__LINE__,false);
      }      
    }
    
    if(fScaleType ==  IMG_RESCALE_TYPE_TILE ){
      decMode = ImageDecoder::IMG_DEC_MODE_TILE ;      
    }else{
      decMode = ImageDecoder::IMG_DEC_MODE_LINK ;
    }    
      
    pJpgDecComp->Config(decMode);    
#endif
    
    pJpgDecComp->setTileBufNum(fTileBufRowNum);    
    pJpgDecComp->setFirstDecRow(1);


    // buffer size
    fDecDstBufSize[0] = pJpgDecComp->getMinLumaBufSize()* (fTileBufRowNum + fTileBufRowMargin); 
    fDecDstBufSize[1] = fDecDstBufSize[2] = pJpgDecComp->getMinCbCrBufSize()* (fTileBufRowNum + fTileBufRowMargin);
    
    // buffer stride
    fDecDstBufStride[0] = pJpgDecComp->getMinLumaBufStride(); 
    fDecDstBufStride[1] = fDecDstBufStride[2] = pJpgDecComp->getMinCbCrBufStride();    

    // image width/height
    fDecDstImgWidth[0]  = pJpgDecComp->getLumaImgStride();
    fDecDstImgWidth[1] = fDecDstImgWidth[2] = pJpgDecComp->getCbCrImgStride();
    

    // !!!!!!!!!!!!!!!REMOVE_ME!!!!!!!!!!!!!!!


    if(!allocDecMEM()){
        return fail_return("JPEG Tile DecMEM alloc fail!!");
    }

    //==============================================
    //    step1: Config Jpeg Decoder 
    //==============================================
         

    pJpgDecComp->setSrcAddr(fSrcAddr);
    pJpgDecComp->setSrcSize( TO_FLOOR(fSrcBufferSize, (IMGDEC_MALLOC_M4U_BS_ALIGN_SIZE)), fSrcBsSize);
          
    pJpgDecComp->setDstBufSize(fDecDstBufSize[0],fDecDstBufSize[1]) ;
    pJpgDecComp->setDstAddr0((unsigned int)fDecDstBufAddrVA0[0], (unsigned int)fDecDstBufAddrVA0[1], (unsigned int)fDecDstBufAddrVA0[2]);
    
    if(fScaleType == IMG_RESCALE_TYPE_LINK) 
      pJpgDecComp->setDstAddr1((unsigned int)fDecDstBufAddrVA1[0], (unsigned int)fDecDstBufAddrVA1[1], (unsigned int)fDecDstBufAddrVA1[2]);

    if(fMemType == IMG_MEM_TYPE_PMEM){
      pJpgDecComp->setDstAddrPA0(fDecDstBufAddrPA0[0], fDecDstBufAddrPA0[1], fDecDstBufAddrPA0[2]);
      if(fScaleType == IMG_RESCALE_TYPE_LINK) 
         pJpgDecComp->setDstAddrPA1(fDecDstBufAddrPA1[0], fDecDstBufAddrPA1[1], fDecDstBufAddrPA1[2]);   
    }    

    if( fImgDecType == IMG_DEC_TYPE_WEBP ){ //fScaleType == IMG_RESCALE_TYPE_LINK &&
       if(!pJpgDecComp->StartConfig()){
         return code_return("decoder StartConfiguration fail,", __LINE__, false);
       }      
             
    }
      
    //==========================================================
    //    step2: Config Tile Mode & get ImgScalerPipe Tile Info
    //==========================================================

   
    //set configuration
    if(fScaleType == IMG_RESCALE_TYPE_LINK){
      bool srcJpg = (fImgDecType == IMG_DEC_TYPE_JPEG)? 1: 0 ;
      pImgCdp->setGtrSrcIsJpeg(srcJpg);
    }

    pImgCdp->setSrcImageColor(fScalerInFormat);
    //set tile only configure
    pImgCdp->setSrcImgMcuNum(fImgMcuRowNum);
    pImgCdp->setTileBufRowNum(fTileBufRowNum);
    pImgCdp->setSrcRowHeight(fImgMcuHeight[0],fImgMcuHeight[1]);
    pImgCdp->setCropImageSize(fJpgWidth >> fDecBRZ, fJpgHeight >> fDecBRZ);
    pImgCdp->setSrcImageSize(fDecDstImgWidth[0],  fDecDstImgHeight[0], fDecDstImgWidth[1], fDecDstImgHeight[1]);    
    pImgCdp->setSrcBufSize(fDecDstBufSize[0], fDecDstBufSize[1]);    
    pImgCdp->setSrcBufStride(fDecDstBufStride[0], fDecDstBufStride[1]);
    
#if 1    
    pImgCdp->setSrcAddr0((unsigned int)fDecDstBufAddrVA0[0], (unsigned int)fDecDstBufAddrVA0[1], (unsigned int)fDecDstBufAddrVA0[2]);

    if(fScaleType == IMG_RESCALE_TYPE_LINK) 
      pImgCdp->setSrcAddr1((unsigned int)fDecDstBufAddrVA1[0], (unsigned int)fDecDstBufAddrVA1[1], (unsigned int)fDecDstBufAddrVA1[2]);
    
    if(fMemType == IMG_MEM_TYPE_PMEM ) {
      pImgCdp->setSrcAddrPA0(fDecDstBufAddrPA0[0], fDecDstBufAddrPA0[1], fDecDstBufAddrPA0[2]);      
      if(fScaleType == IMG_RESCALE_TYPE_LINK) 
         pImgCdp->setSrcAddrPA1(fDecDstBufAddrPA1[0], fDecDstBufAddrPA1[1], fDecDstBufAddrPA1[2]);
    }
#else    
    pImgCdp->setSrcAddr0((unsigned int)fDecDstBufAddrVA1[0], (unsigned int)fDecDstBufAddrVA0[1], (unsigned int)fDecDstBufAddrVA0[2]);

    if(fScaleType == IMG_RESCALE_TYPE_LINK) 
      pImgCdp->setSrcAddr1((unsigned int)fDecDstBufAddrVA0[0], (unsigned int)fDecDstBufAddrVA1[1], (unsigned int)fDecDstBufAddrVA1[2]);
    
    if(fMemType == IMG_MEM_TYPE_PMEM ) {
      pImgCdp->setSrcAddrPA0(fDecDstBufAddrPA0[0], fDecDstBufAddrPA0[1], fDecDstBufAddrPA0[2]);      
      if(fScaleType == IMG_RESCALE_TYPE_LINK) 
         pImgCdp->setSrcAddrPA1(fDecDstBufAddrPA1[0], fDecDstBufAddrPA1[1], fDecDstBufAddrPA1[2]);
    }     
#endif

    pImgCdp->setDstImageColor(fCdpDstFormat);
    pImgCdp->setDstImageSize(fOutWidth, fOutHeight);    
    pImgCdp->setDstBufSize(fOutBufSize, fOutCbBufSize, fOutCrBufSize);
    pImgCdp->setDstAddr(fCdpDstBufAddr, (unsigned int)fDstCbAddr, (unsigned int)fDstCrAddr); 
    pImgCdp->setDstHandler((unsigned long)fProcHandler);
    //pImgCdp->setDstAddrPA(fOutBufAddrPA);
    
    if(fIonEn && fDstFD >= 0)
      pImgCdp->setDstFD(fDstFD);

/*                    
    if(fScaleType == IMG_RESCALE_TYPE_TILE ){
      if(!fUseImgRszCmodel){
         if(!pImgCdp->TileInit()){
           return code_return("Scaler Tile Init fail,", __LINE__, false);
         }
      }
    }
*/             
   
   
   return true ;
}

      

bool JpgDecHal::onStartTile()
{

    unsigned int decTileRow = 0 ;
    unsigned int totalTileRun = 0 ;    
    unsigned int tileRunCnt = 0;    
    
    ATRACE_CALL();


    //==========================================================
    //    step3: Decode MCU ROW for Image Scaler (WAIT_DONE)
    //==========================================================

                    

      if(!fUseImgRszCmodel){
         if(!pImgCdp->TileInit()){
           return code_return("Scaler Tile Init fail,", __LINE__, false);
         }
      }

	
    
    //totalTileRun = (fUseImgRszCmodel )? fImgMcuRowNum: pImgCdp->getTileTotalRun() ;
    
    
    //start jpeg
    if( ImageDecoder::IMG_DEC_RST_DONE != pJpgDecComp->Start()){
        return fail_return("JPEG Decoder Start fail");
    }    

   /*===========================================================*
    *    step4:                                                 *
    *      1) Trigger Image Scaler                              *
    *      2) get Tile number                                   *
    *      3) trigger Jpeg to decode reguire MCU row            *
    *===========================================================*/

    
    //for(;tileRunCnt < totalTileRun; )
    for( ; !pImgCdp->isScalingFinish(); )
    {
      
      //JPG_LOG("StartTile %d %d !!\n", tileRunCnt, totalTileRun);  
      
       if(fUseImgRszCmodel){
         if(tileRunCnt == 0){
            decTileRow = 0;
         }else
            decTileRow = 1;
       }else{
         
         if(tileRunCnt == 0){
           decTileRow  = pImgCdp->getTileEnd(tileRunCnt) - pImgCdp->getTileStart(tileRunCnt) +1 -1;
           JPG_LOG("decodeNextTile RunCnt %d, 0+%d->%d !!\n", tileRunCnt, decTileRow, pImgCdp->getTileEnd(tileRunCnt));    
         }else{
           decTileRow  = pImgCdp->getTileEnd(tileRunCnt) - pImgCdp->getTileEnd(tileRunCnt-1) ; 
           JPG_LOG("decodeNextTile RunCnt %d, %d+%d->%d !!\n", tileRunCnt, pImgCdp->getTileEnd(tileRunCnt-1), decTileRow, pImgCdp->getTileEnd(tileRunCnt));        
           
         }
       }
       {
         ATRACE_NAME("DECODE_ROW"); //android::Tracer::traceBegin(ATRACE_TAG_GRAPHICS,"DECODE_ROW");
         if(!pJpgDecComp->ResumeDecRow(decTileRow)){
             return fail_return("JPEG Decoder Decode fail");
         }          
         //android::Tracer::traceEnd(ATRACE_TAG_GRAPHICS);  
       }

{
   
       static unsigned int fileNameDecIdx = 0;
       if(dumpBin2File(DUMP_DEC_LVL_STEP,"dumpDecPipe_decStep", &fileNameDecIdx, fDecDstBufAddrVA0[0], fDecDstBufSize[0]))
         fileNameDecIdx++;      
}       
{
#if 0
       static unsigned int fileNameDecIdxU = 0;
       static unsigned int fileNameDecIdxV = 0;
       if(dumpBin2File(DUMP_DEC_LVL_STEP,"dumpDecPipe_U_decStep", &fileNameDecIdxU, fDecDstBufAddrVA0[1], fDecDstBufSize[1]))
         fileNameDecIdxU++;
       
       if(dumpBin2File(DUMP_DEC_LVL_STEP,"dumpDecPipe_V_decStep", &fileNameDecIdxV, fDecDstBufAddrVA0[2], fDecDstBufSize[1]))
         fileNameDecIdxV++;                 
#endif         
}       
       
       {
         ATRACE_NAME("MDP_SCALE"); //android::Tracer::traceBegin(ATRACE_TAG_GRAPHICS,"MDP_SCALE");
         if(!pImgCdp->Start()){
             return fail_return("JPEG ImageCDP Start fail");
         }     
         //android::Tracer::traceEnd(ATRACE_TAG_GRAPHICS); 
       }


{       
       static unsigned int fileNameDecIdx = 0;
       if(dumpBin2File(DUMP_DEC_LVL_SCALE,"dumpDecPipe_scaleStep", &fileNameDecIdx, (unsigned char *)fCdpDstBufAddr, fOutBufSize))
         fileNameDecIdx++;
     
}       
         
       tileRunCnt++;
              
             
            
       
    }




    //==========================================================
    //    step5: Trigger Scaler for Last tile Row & WAIT_DONE
    //==========================================================
    

    if(fUseImgRszCmodel){
      if(!pImgCdp->Start()){
          return fail_return("JPEG ImageCDP Start fail");
      } 
      tileRunCnt++;
      
      if(!pImgCdp->Wait()){
          return fail_return("JPEG ImageCDP Start fail");
      }
    }
    

    

    return true;

}


bool JpgDecHal::onStartGdma()
{

    ATRACE_CALL();  

    //=====================================================================
    //    step0: Config & Query Jpeg Decoder Output Buffer requirement
    //=====================================================================
    


    //trigger scaler first    
    if(!pImgCdp->Start()){
        return fail_return("ImgCDP Start fail");
    }        

    //==============================================
    //    step3: trigger Jpeg Decoder
    //==============================================

    if(ImageDecoder::IMG_DEC_RST_DONE != pJpgDecComp->Start()){
        return fail_return("JPEG Decoder Start fail");
    } 

    //==============================================
    //    step4: Wait Scaler Done
    //==============================================

    
    //wait scaler done
    if(!pImgCdp->Wait()){
        return fail_return("ImgCDP Wait TimeOut");
    }
    

    return true;

}


//maby no need
bool JpgDecHal::allocIOMEM()
{
   

   if(fMemType == IMG_MEM_TYPE_PMEM){
      
      
      JPG_DBG("Allocate Dst Pmem, 0x%x!!", fOutBufSize);
      //RAW output buffer
      JPEG_PMEM_ALLOC( fOutBufSize, fOutPmemVA, fOutBufAddrPA, fOutPmemFD ) ;
      
      if(fOutPmemVA) //if use PMEM
        fCdpDstBufAddr = (unsigned int)fOutPmemVA ; //(unsigned int)fDstAddr ;
      
      
      JPG_DBG("Allocate Destination Pmem, va:0x%x, pa:0x%x, size:%u", fOutPmemVA, fOutBufAddrPA, fOutBufSize);
    }else if(fMemType == IMG_MEM_TYPE_M4U){
       fCdpDstBufAddr  = (unsigned int)fDstAddr ; 
    }     
    

      
     return true;
}
 

bool JpgDecHal::start()
{
   
    unsigned int result =  false ;
    unsigned int draw_cnt= 0, draw_num = 4;
    unsigned int draw_base = fOutWidth*pixel_byte[fOutFormat]*(fOutHeight/2);
    struct timeval t1, t2;
    //Check param
    if(true != checkParam()) {
        return false;
    }
    ATRACE_CALL();

    //JPG_DBG("JpgDecPipe::start -> config jpeg path, type %d", fScaleType);
    JPG_DBG("Decoder Src Addr:0x%x, size (%x)%x, width/height:[%u, %u], type %d!!\n", (unsigned int)fSrcAddr, fSrcBsSize, fSrcBufferSize,fJpgWidth, fJpgHeight, fScaleType);
    JPG_DBG("Decoder Dst Addr:0x%x, size %x, width/height:[%u, %u], format:%u", (unsigned int)fDstAddr, fOutBufSize,fOutWidth, fOutHeight, fOutFormat);
    //JPG_DBG("Decoder Dither:%d, RangeDecode:%d [%d %d %d %d]", isDither, isRangeDecode, fLeft, fTop, fRight, fBottom);
    gettimeofday(&t1, NULL);

{ 
#ifdef ENABLE_IMG_CODEC_DUMP_RAW    
    dump_fmt = fOutFormat ;
#endif     
       static unsigned int fileNameSrcIdx = 0;
       if(dumpBin2File(DUMP_DEC_LVL_SRCBS,"dumpDecPipe_decSrc", &fileNameSrcIdx, fSrcAddr, fSrcBsSize))
         fileNameSrcIdx++;      
} 


    if( !allocIOMEM() )
      return false;

    //fScaleType = lockScaler(fScaleType);



    if( fScaleType == IMG_RESCALE_TYPE_LINK ){

      if(!onConfigComp()){
          return code_return("Pipe onConfigComp fail,", __LINE__, false);
      }		
      result = onStartGdma();
      
      {      
        unsigned int static index=0;
        if(dumpBin2File(DUMP_DEC_LVL_OUT,"dumpDecPipe_out", &index, fDstAddr, fOutBufSize))
          index++;        
      }  
    }else if( fScaleType == IMG_RESCALE_TYPE_TILE ){
      if(!onConfigCompTile()){
          return code_return("Pipe onConfigComp fail,", __LINE__, false);
      }    
      result = onStartTile(); //process one picture
      //freeDecMEM();
    }
    gettimeofday(&t2, NULL);
    JPG_DBG("::[IMGTIME] DecPipe rst %d, mode %d, time %u, pixels %dK(%d %d) -> %dK(%d %d), fbrz %d/%d, fmt %d->%d!!\n",
      result, fScaleType
     ,(t2.tv_sec - t1.tv_sec)*1000000 + (t2.tv_usec - t1.tv_usec)
     , fJpgWidth*fJpgHeight/(1024), fJpgWidth, fJpgHeight
     ,fOutWidth*fOutHeight/(1024), fOutWidth, fOutHeight
     ,fDecBRZ, fTotalBRZ, fDecOutFormat, fOutFormat); 
    
#if 0
      JPG_DBG("JpgDecPipe::[DRAW_LINE] draw_line [%d %d] in range addr %x %d %d %d", fOutWidth, draw_num, (unsigned int)fDstAddr, fOutWidth, fOutHeight, fOutFormat);      
      memset((unsigned char *)(fDstAddr+draw_base),0xff,draw_num*fOutWidth*pixel_byte[fOutFormat]);
#endif      

    if(true == result) {

    } else {
        return false;
    }
    

    return true;
}

 
