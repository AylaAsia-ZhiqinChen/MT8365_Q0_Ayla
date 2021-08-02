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

#include "Trace.h" 
//#define ATRACE_TAG ATRACE_TAG_GRAPHICS

#include <img_cdp_pipe.h>
#include "gdma_ctr_hal.h"



#include "m4u_lib.h"

#include "img_mmu.h"

#define SCALER_EP




#if 0    
  #include "IPipe.h"
//  #include "ICamIOPipe.h"
  #include "ICdpPipe.h"
//  #include "IPostProcPipe.h"  
//  #include <cutils/pmem.h>
  #include "isp_function.h"
//  #include "isp_drv.h"
//  #include "sensor_hal.h"
//  //#include "uvvf.h"//test
#endif



#include "IPipe.h"
#include "ICamIOPipe.h"
#include "ICdpPipe.h"
#include "IPostProcPipe.h"
//
#include <cutils/pmem.h>
#include "isp_drv.h"
#include "sensor_hal.h"
//#include "uvvf.h"//test


#include "res_mgr_drv.h"
#include "pipe_mgr_drv.h"
 

using namespace NSImageio ;
using namespace NSImageio::NSIspio;



JUINT32 *pImgiPattern, *pVipiPattern, *pVip2iPattern;

 

#define USE_PMEM

#ifdef USE_PMEM
#include <cutils/pmem.h>
//#include <cutils/memutil.h>
#endif

#ifdef LOG_TAG
#undef LOG_TAG
#endif
 
//#define JPEG_PROFILING   

#define LOG_TAG "ImgScaler"

#define IMGRSZ_W(fmt, arg...)    ALOGD(fmt, ##arg)
#define IMGRSZ_D(fmt, arg...)    //ALOGD(fmt, ##arg)
//#define IMGRSZ_D(fmt, arg...)

#define JPG_MDP_MAX_ELEMENT 3

#define JPEG_IRQ_TIMEOUT_DEC 5000  /*3000*/


//#define SCALER_TILE_BUF_REG_SIZE (36*1024)
#define SCALER_TILE_BUF_CFG_SIZE (10*1024)
//#define SCALER_TILE_BUF_CFG_SIZE (750*1024)
  


#undef bool
#define bool bool

//#define IMG_PTHREAD


static bool fail_return(const char msg[]) {
#if 1
    IMGRSZ_W("[ImageScaler] - %s", msg);
#endif
    return false;   // must always return false
}

static int code_return(const char msg[], int line,int code) {
#if 1
    IMGRSZ_W("[ImageScaler] - %s, L:%d!!\n", msg, line);
#endif
    return code;   // must always return code
}


#define SCALER_PMEM_ALLOC(size, mva, pa, fd)  \
{ \
    mva = (unsigned char *)pmem_alloc_sync(size, &fd);   \
    if(mva == NULL) {   \
        IMGRSZ_D("Can not allocate PMEM, L:%d!!\n", __LINE__);  \
        return SCALER_PIPE_RST_CFG_ERR ;  \
    } \
    pa = (JUINT32)pmem_get_phys(fd);       \
    IMGRSZ_D("Allocate Pmem, va:0x%x, pa:0x%x, size:%x, L:%d!!\n", mva, pa, size, __LINE__);  \
    memset(mva, 0, size);    \
}


#define SCALER_PMEM_FREE(va, size, fd) \
{  \
   if(va != NULL) { \
       pmem_free(va, size, fd); \
       va = NULL;   \
   }  \
}


#define SEL_MIN(a,b) ( ((a)<(b)) ? (a):(b) )
#define SEL_MAX(a,b) ( ((a)>(b)) ? (a):(b) )
#define CLIP_255(x) ( ((x) < 0) ? (0): ( SEL_MIN( (x), 255 ) ) )


#define DUMP_CDP_LVL_OUT 1
#define DUMP_CDP_LVL_STEP_IN 2
#define DUMP_CDP_LVL_STEP_OUT 3


int dump2file(unsigned int level, const char filename[], unsigned int *index, unsigned char *SrcAddr, unsigned int size)
{
   
   FILE *fp = NULL;
   FILE *fpEn = NULL;
   unsigned char* cptr ;
   const char tag[64] = "DUMP_CDP_PIPE";
   char filepath[128];
   char dumpEn[128] ; 

#ifndef ENABLE_IMG_CODEC_DUMP_RAW
   return false ;
#endif
   
   //sprintf(filepath, "/data/otis/dec_pipe_scaler_step_%04d.raw", fileNameIdx);   
   sprintf(  dumpEn, "//data//otis//%s_%d", tag, level);   
   sprintf(filepath, "//data//otis//%s_%04d.raw", filename, *index);   

   fpEn = fopen(dumpEn, "r");
   if(fpEn == NULL)
   {
       //IMGRSZ_W("Check Dump En is zero!!\n");
       return false;
   }
   fclose(fpEn);
      
   fp = fopen(filepath, "w");
   if (fp == NULL)
   {
       IMGRSZ_W("open Dump file fail: %s\n", filepath);
       return false;
   }

   IMGRSZ_W("\nDumpRaw -> %s, En %s, addr %x, size %x !!", filepath,dumpEn,(unsigned int)SrcAddr, size);                     
   cptr = (unsigned char*)SrcAddr ;
   for( unsigned int i=0;i<size;i++){  /* total size in comp */
     fprintf(fp,"%c", *cptr );  
     cptr++;
   }          
   
   fclose(fp); 
   //*index++;
   
   return true ;       
}


#define IMGDEC_ALLOC(ptr, size, align, align_ptr) \
{  \
   ptr = (ISPIO_TDRI_INFORMATION_STRUCT*)malloc(size+align) ; \
   align_ptr = (ISPIO_TDRI_INFORMATION_STRUCT*)TO_CEIL(ptr, align);    \
   /*IMGRSZ_W("[IMGDEC_ALLOC] memory_alloc addr %x(%x), size %x, align %x, L:%d!!\n", (unsigned int)(ptr), (unsigned int)(align_ptr), size,align,__LINE__);*/ \
   /*memset(ptr, 0 , size+align);*/ \
}

#define IMGDEC_FREE(ptr) \
{  \
   /*IMGRSZ_W("[IMGDEC_FREE] memory_free addr %x, L:%d!!\n", (unsigned int)(ptr), __LINE__);*/ \
   if(ptr != NULL)   \
     free(ptr) ;  \
   ptr = NULL ;   \
}


   

ImgCdpPipe::ImgCdpPipe()
{
   
    IMGRSZ_D("ImgCdpPipe::ImgCdpPipe");
      ATRACE_CALL();
    unsigned int i = 0;


    fSrcPmemVA = NULL;
    fDstPmemVA = NULL;
    fSrcCbPmemVA = NULL;
    fSrcCrPmemVA = NULL;


    fm4uCdpID = M4U_CLNTMOD_IMG ;    

    islock = false;
    isDither = false;


    fOutWidth = fOutHeight = 0;

    fDstBufAddr = NULL;
    fIsUseCmodel = 0;
    fCurRowStart = 
    fCurRowEnd   = 
    fNxtRowStart = 
    fNxtRowEnd   = 0;
    fTileRunCnt = 0;
    fTileGoCntInRow = 0;
    
    fGtrIsJpeg = SCALER_GDMA_SRC_JPEG;
    fGtrSrcIs422 = 0;
    fGtrUVcons = 0x9D ;
    fGtrIsGray = 0;

    fTileCfgBufSize = SCALER_TILE_BUF_CFG_SIZE ;
    
    fScalerMode = SCALER_MODE_TILE ;
    

    //fMemType = fMemTypeDefault = IMG_MEM_TYPE_PMEM ;
    //fMemType = fMemTypeDefault = IMG_MEM_TYPE_PHY ;
    fMemType = fMemTypeDefault = IMG_MEM_TYPE_M4U ;
    
    fTileTotalRun = 0;
    fDstFD = -1;
    fIonEn = 0;
    fSrcPlaneNum = 3;
    
    


    //memset( &imgi_ring, 0 , sizeof(PortInfo));
    //memset( &vipi_ring, 0 , sizeof(PortInfo));
    //memset(&vip2i_ring, 0 , sizeof(PortInfo));

    pResMgr = NULL;
    pPipeMgr = NULL;
    pCdpPipeRing = NULL;
    pM4uDrv = NULL;
    fIRQinRow = 1;
    fDstHandle = NULL;
    tTileCfgBuf = NULL ;
    tTileCfgBuf_org = NULL;
    fTileCfgCnt = 0;
    //for(i = 0;i< TILE_ROW_SIZE; i++)      
    //  memset(&tRowCfg[i],0,sizeof(TILE_ROW_DATA));
    for(i = 0;i<3; i++){
      fSrcBufAddr0[i] = 0;
      fSrcBufAddr1[i] = 0;
      fSrcBufSize[i] = 0;
      fSrcM4UAddrMva0[i] = 0;
      fSrcM4UAddrMva1[i] = 0;
    }
    fLockCnt = 0;
    fResetISP = 0;
    fCropInWidth  = 0;
    fCropInHeight = 0;
    
}



ImgCdpPipe::~ImgCdpPipe()
{
    IMGRSZ_D("ImgCdpPipe::~ImgCdpPipe");


    unlock();
    
}


void yuv2rgb(unsigned char *R, unsigned char *G, unsigned char *B, int Y,int U,int V)
{
  int C,D,E;

  C = Y - 16  ;
  D = U - 128 ;
  E = V - 128 ;


  *R = (unsigned char)(CLIP_255(( (298 * C)             + (409 * E) + 128) >> 8)) ;
  *G = (unsigned char)(CLIP_255(( (298 * C) - (100 * D) - (208 * E) + 128) >> 8)) ;
  *B = (unsigned char)(CLIP_255(( (298 * C) + (516 * D)             + 128) >> 8)) ;
   
   
   
}


typedef struct {
  
  unsigned int srcWidth;
  unsigned int srcHeight;
  unsigned int srcMemStride[3];

  unsigned char *srcAddr[3];
  unsigned char *dst_buf;

  unsigned int srcFormat;  //0: YUV420, 1: YUV422
  unsigned int dstFormat;  //0: ARGB8888, 1:RGB888, 2:RGB565
  unsigned int pixelSize ;
   
   
} CONV_YUV2RGB_DATA ;


bool convYUV2RGB(CONV_YUV2RGB_DATA* convY2R)
{

  int y, cb, cr;
  
  unsigned char* inptr0 ;
  unsigned char* inptr1 ;
  unsigned char* inptr2 ;
  
  unsigned char *outptr;
  int num_rows = convY2R->srcHeight ;
  unsigned int pixelSzie = convY2R->pixelSize ;
  unsigned int dstByteInRow = convY2R->srcWidth * convY2R->pixelSize ;
  unsigned char * output_buf = convY2R->dst_buf ;
  unsigned int row_ctr = 0;
  unsigned int num_cols = convY2R->srcWidth ;
  unsigned int input_row =0, input_row_uv = 0;
  
  unsigned int is422 = 0;
  unsigned int col = 0;
  unsigned char *R;
  unsigned char *G;
  unsigned char *B;
  
  if(convY2R->srcFormat == 1)
   is422 = 1;
    
  printf("\nYUV2RGB : %d %d ,dstByteInRow %d, stride %d %d", convY2R->srcWidth, convY2R->srcHeight,dstByteInRow, convY2R->srcMemStride[0], convY2R->srcMemStride[1]);

  while (--num_rows >= 0) {
   
    
    
    inptr0 = convY2R->srcAddr[0] + (input_row*convY2R->srcMemStride[0]);

      inptr1 = convY2R->srcAddr[1] + (input_row_uv*convY2R->srcMemStride[1]);
      inptr2 = convY2R->srcAddr[2] + (input_row_uv*convY2R->srcMemStride[2]);

    
    outptr = output_buf+(dstByteInRow*row_ctr);

    //printf("\nY2R: row %d %d->%d, src %x %x %x -> %x !!",input_row, input_row_uv, row_ctr, inptr0, inptr1, inptr2, outptr );          
    
    for (col = 0; col < num_cols; col++) {
      //printf(" %d", col);
      y  = inptr0[col];
      cb = inptr1[col/2];
      cr = inptr2[col/2];

            
      yuv2rgb( outptr, outptr+1, outptr+2, y, cb, cr);
      R = outptr+0;
      G = outptr+1;
      B = outptr+2;


      outptr += pixelSzie;
      
      /* Range-limiting is essential due to noise introduced by DCT losses. */
      //outptr[RGB_RED] =   range_limit[y + Crrtab[cr]];
      //outptr[RGB_GREEN] = range_limit[y +  ((int) RIGHT_SHIFT(Cbgtab[cb] + Crgtab[cr], SCALEBITS))];
      //outptr[RGB_BLUE] =  range_limit[y + Cbbtab[cb]];
      
      

    } 
    //printf("\nY2R: row %d %d->%d, src %x %x %x -> %x, YUV %d %d %d -> %d %d %d !!",input_row, input_row_uv, row_ctr, inptr0, inptr1, inptr2, outptr, y, cb, cr, *R, *G, *B );      
    

    row_ctr++;
    input_row++;    
    
    if(is422)    
      input_row_uv ++ ;
    else
      input_row_uv = input_row/2;    
  }   
      
   
  return true;   
}

 

bool ImgCdpPipe::lock(JUINT32 mode)
{
   if(!setConnectType(mode))
      return code_return("setConnectType fail!!",__LINE__,false);
   
   if(fScalerMode == SCALER_MODE_LINK ){
     if(GDMA_CTR_STATUS_OK != gdmaCtrLock(&decID)) {
         return fail_return("Jpeg Resizer resource is busy");
     }
   }
    islock = true;
    return true;
}

bool ImgCdpPipe::unlock()
{
    if(islock)
    {   
       ATRACE_CALL();
       if(fScalerMode == SCALER_MODE_LINK ){

         IMGRSZ_W("::gdmaCtrUnlock, L:%d!!\n", __LINE__);            
         //LINK mode, unlock gdma
         gdmaCtrUnlock(decID); 
       }      
       //if(fScalerMode == SCALER_MODE_TILE )
       {
         
         onHWScalerDeInit();  
           
       }

#if 1
{ 
        static unsigned int index = 0;
        if(dump2file(DUMP_CDP_LVL_OUT,"dumpCdpPipe_out", &index, (unsigned char*)fDstBufAddr, fDstBufSize))
         index++;
        
}        
#endif       
       freeIOMEM();
       
       islock = false;
    }

    
    return true;
}

bool ImgCdpPipe::checkParam()
{
    if(fScalerMode == SCALER_MODE_NONE || fScalerMode >= SCALER_MODE_SIZE)
    {
        IMGRSZ_W("Invalide scalerMode %d", fScalerMode);
        return false;
    }
      

    if(fSrcImgWidth[0] == 0 || fSrcImgHeight[0] == 0)
    {
        IMGRSZ_W("Invalide JPEG width/height %u/%u", fSrcImgWidth[0], fSrcImgHeight[0]);
        return false;
    }
    
    if(fOutWidth < 3 || fOutHeight < 3)
    {
        IMGRSZ_W("Invalide width/height %u/%u [Range 3~4095]", fOutWidth, fOutHeight);
        return false;
    }

    if(fOutWidth > 4095 || fOutHeight > 4095)
    {
        IMGRSZ_W("Invalide width/height %u/%u [Range 3~4095]", fOutWidth, fOutHeight);
        return false;
    }

    if(fSrcBufAddr0[0] == 0 )  
    {
        return fail_return("Invalide Address");
    }

    if( fDstFD < 0 && fDstBufAddr == 0 )  
    {
        return fail_return("Invalide Address");
    }    

    return true;
}

bool ImgCdpPipe::setConnectType(JUINT32 mode)
{ 
   
   if(mode >= SCALER_MODE_SIZE || mode == SCALER_MODE_NONE)
      return false;
      
   if( !onHWScalerCreate(mode) ){
     onHWScalerDeInit(); 
     return false;
   }else
     return true;
                                       
}

/*
enum EImageFormat
{
    eImgFmt_UNKNOWN         = 0x00000,   //unknow
    eImgFmt_BAYER8          = 0x00001,   //Bayer format, 8-bit
    eImgFmt_BAYER10         = 0x00002,   //Bayer format, 10-bit
    eImgFmt_BAYER12         = 0x00004,   //Bayer format, 12-bit
    eImgFmt_YV12            = 0x00008,   //420 format, 3 plane(YVU)
    eImgFmt_NV21            = 0x00010,   //420 format, 2 plane (VU)
    eImgFmt_NV21_BLK        = 0x00020,   //420 format block mode, 2 plane (UV)
    eImgFmt_NV12            = 0x00040,   //420 format, 2 plane (UV)
    eImgFmt_NV12_BLK        = 0x00080,   //420 format block mode, 2 plane (VU)
    eImgFmt_YUY2            = 0x00100,   //422 format, 1 plane (YUYV)
    eImgFmt_UYVY            = 0x00200,   //422 format, 1 plane (UYVY)
    eImgFmt_RGB565          = 0x00400,   //RGB 565 (16-bit), 1 plane
    eImgFmt_RGB888          = 0x00800,   //RGB 888 (24-bit), 1 plane
    eImgFmt_ARGB888         = 0x01000,   //ARGB (32-bit), 1 plane
    eImgFmt_JPEG            = 0x02000,   //JPEG format
    eImgFmt_YV16            = 0x04000,   //422 format, 3 plane
    eImgFmt_NV16            = 0x08000,   //422 format, 2 plane (UV)
    eImgFmt_NV61            = 0x10000,   //422 format, 2 plane (VU)
    eImgFmt_I420            = 0x20000,   //420 format, 3 plane(YUV)
};
*/





#define TILE_STRIDE_UNIT 256



int ImgCdpPipe::configCdpIO()
{

    ATRACE_CALL();
    EImageFormat   inputFormat;
    EImageFormat   outputFormat;   


    memset(&cfgCdpData, 0 , sizeof(CONFIG_RESIZER_DATA));
    
    for(int i = 0;i <3;i++){
      cfgCdpData.tSrcImgWidth[i]  = fSrcImgWidth[i];
      cfgCdpData.tSrcImgHeight[i] = fSrcImgHeight[i];
      cfgCdpData.tSrcBufStride[i] = fSrcBufStride[i];
      cfgCdpData.tSrcBufTotalSize[i]  = fSrcBufStride[i] * fSrcImgHeight[i];
      cfgCdpData.tSrcBufRingSize[i]   = fSrcBufStride[i] * fTileBufMcuRow * fRowHeight[i];
      cfgCdpData.tSrcBufVA[i] = fSrcBufAddr0[i] ;
      cfgCdpData.tSrcBufPA[i] = fSrcBufAddrPA0[i] ;
      IMGRSZ_D("ScalerInit: [%d] w %d, h %d,s %d, tSize %x, rSize %x, va %x, pa %x!! \n", i,fSrcImgWidth[i], fSrcImgHeight[i], fSrcBufStride[i]
      ,cfgCdpData.tSrcBufTotalSize[i], cfgCdpData.tSrcBufRingSize[i], fSrcBufAddr0[i], fSrcBufAddrPA0[i]);
    }
    
    
    cfgCdpData.tRingBufRowNum = fTileBufMcuRow;    
    cfgCdpData.tRowHeight     = fRowHeight[0];
    cfgCdpData.tDstImgWidth   = fOutWidth; 
    cfgCdpData.tDstImgHeight  = fOutHeight; 
    cfgCdpData.tDstBufSize    = fDstBufSize; 
    cfgCdpData.tDstBufVA      = fDstConfigAddr;   //(unsigned int)malloc(fDstBufSize);
    cfgCdpData.tDstBufPA      = fDstConfigAddrPA;      
    cfgCdpData.tCfgBufVA = (JUINT32)tTileCfgBuf;
    
    IMGRSZ_D("ScalerInit: crop %d %d, BufRow %d, MCU_Y %d, dst %d %d, bufSize %x, va %x->%x, pa %x, cfgVA %x!! ", fCropInWidth, fCropInHeight,
    fTileBufMcuRow, fRowHeight[0], fOutWidth, fOutHeight, fDstBufSize, fDstConfigAddr,cfgCdpData.tDstBufVA, fDstConfigAddrPA, (JUINT32)tTileCfgBuf );

  

    if(fSrcFormat == SCALER_IN_3P_YUV420){
      inputFormat = eImgFmt_I420 ; //420 format, 3 plane
    }else if(fSrcFormat == SCALER_IN_3P_YUV422){
      inputFormat = eImgFmt_YV16 ; //422 format, 3 plane
    }else if(fSrcFormat == SCALER_IN_1P_GRAY){
      inputFormat = eImgFmt_Y800;//eImgFmt_YV16 ; //422 format, 3 plane
    }else if (fSrcFormat == SCALER_IN_1P_RGB888){
      inputFormat = eImgFmt_RGB888 ;
      fSrcPlaneNum = 1;
    }else if (fSrcFormat == SCALER_IN_1P_ARGB8888){
      inputFormat = eImgFmt_ARGB888 ;
      fSrcPlaneNum = 1;
    }else if (fSrcFormat == SCALER_IN_1P_RGB565){
      inputFormat = eImgFmt_RGB565 ;
      fSrcPlaneNum = 1;
    }else{
       inputFormat = eImgFmt_YV16 ; //422 format, 3 plane
       return code_return("::[IOINIT] invalid Scaler Source Format, ", __LINE__, false);
    }
    
    if(fDstFormat == SCALER_OUT_1P_ARGB8888){
      outputFormat = eImgFmt_ARGB888;
    }else if(fDstFormat == SCALER_OUT_1P_RGB565){
      outputFormat = eImgFmt_RGB565;
    }else if(fDstFormat == SCALER_OUT_1P_RGB888){
      outputFormat = eImgFmt_RGB888;
    }else if(fDstFormat == SCALER_OUT_1P_YUV422){
      outputFormat = eImgFmt_YUY2;
    }else{
       outputFormat = eImgFmt_ARGB888;
       return code_return("::[IOINIT] invalid Scaler destination format, ", __LINE__, false);
    }


    IMGRSZ_D("ScalerInit: fmt src %d->0x%x, dst %d->%d, Mode %d!!\n", fSrcFormat, inputFormat, fDstFormat, outputFormat, fScalerMode);


    // IMGI
    imgi_ring.eImgFmt        = inputFormat ; //eImgFmt_I420;   //  Image Pixel Format
    imgi_ring.u4ImgWidth     = cfgCdpData.tSrcImgWidth[0] ;      //  Image Width
    imgi_ring.u4ImgHeight    = cfgCdpData.tSrcImgHeight[0];      //  Image Height
    imgi_ring.crop.x         = 0 ;//<< 8; //pdata->cropX;
    imgi_ring.crop.y         = 0 ;//<< 8; //pdata->cropY;
    imgi_ring.crop.w         = fCropInWidth ;//cfgCdpData.tSrcImgWidth[0] ;      //  Image Width   0;//pdata->cropW;
    imgi_ring.crop.h         = fCropInHeight;//cfgCdpData.tSrcImgHeight[0];      //  Image Height  0;//pdata->cropH;
    imgi_ring.u4Offset       = 0;                      
    imgi_ring.u4Stride[ESTRIDE_1ST_PLANE]       = cfgCdpData.tSrcBufStride[0];    //  yuv422 3 plane
    imgi_ring.u4Stride[ESTRIDE_2ND_PLANE]       = 0;
    imgi_ring.u4Stride[ESTRIDE_3RD_PLANE]       = 0;
    imgi_ring.type           = EPortType_Memory;           //  EPortType
    imgi_ring.index          = EPortIndex_IMGI;            //  port index
    imgi_ring.inout          = EPortDirection_In;          //  0:in/1:out
    imgi_ring.pipePass       = EPipePass_PASS2C;        //  select pass
    
    imgi_ring.u4BufSize               = (JUINT32)cfgCdpData.tSrcBufTotalSize[0]; //bytes;  //  Per buffer size
    imgi_ring.u4BufVA                 = (JUINT32)cfgCdpData.tSrcBufVA[0];    //  Vir Address of pool
    imgi_ring.u4BufPA                 = (JUINT32)cfgCdpData.tSrcBufPA[0];    //  Phy Address of pool
    imgi_ring.u4EnRingBuffer          = (fScalerMode == SCALER_MODE_TILE)? 1 : 0;
    imgi_ring.u4RingSize              = cfgCdpData.tSrcBufRingSize[0]/TILE_STRIDE_UNIT;
    imgi_ring.u4RingBufferMcuRowNo    = cfgCdpData.tRingBufRowNum;    //mcu row
    imgi_ring.u4RingBufferMcuHeight   = cfgCdpData.tRowHeight;    //mcu height
    imgi_ring.u4RingTdriBufOffset     = 0;        
    imgi_ring.u4RingConfBufVA         = (JUINT32)cfgCdpData.tCfgBufVA;
    imgi_ring.u4RingConfNumVA         = (JUINT32)&cfgCdpData.tCfgEntryNum;
    imgi_ring.u4RingConfVerNumVA      = (JUINT32)&cfgCdpData.tCfgTotalRun;
    imgi_ring.u4RingErrorControlVA    = (JUINT32)&cfgCdpData.tConfigResult;

    if(fSrcPlaneNum > 1){
       // VIPI
       vipi_ring.eImgFmt         = inputFormat ;             //  Image Pixel Format
       vipi_ring.u4ImgWidth      = cfgCdpData.tSrcImgWidth[1] ;      //  Image Width
       vipi_ring.u4ImgHeight     = cfgCdpData.tSrcImgHeight[1];      //  Image Height
       vipi_ring.crop.x          = 0;
       vipi_ring.crop.y          = 0;
       vipi_ring.crop.w          = cfgCdpData.tSrcImgWidth[1] ;
       vipi_ring.crop.h          = cfgCdpData.tSrcImgHeight[1];
       vipi_ring.u4Offset        = 0;                                   //
       vipi_ring.u4Stride[ESTRIDE_1ST_PLANE]        = 0;//cfgCdpData.tSrcBufStride[1];                 //  yuv422 one plane
       vipi_ring.u4Stride[ESTRIDE_2ND_PLANE]        = cfgCdpData.tSrcBufStride[1];
       vipi_ring.u4Stride[ESTRIDE_3RD_PLANE]        = 0;
       vipi_ring.type            = EPortType_Memory;                    //  EPortType
       vipi_ring.index           = EPortIndex_VIPI;                     //  port index
       vipi_ring.inout           = EPortDirection_In;                   //  0:in/1:out        
       vipi_ring.pipePass        = EPipePass_PASS2C;                    //  select pass        
       vipi_ring.u4BufSize       = (JUINT32) cfgCdpData.tSrcBufTotalSize[1];   //bytes;  //  Per buffer size
       vipi_ring.u4BufVA         = (JUINT32) cfgCdpData.tSrcBufVA[1];          //  Vir Address of pool
       vipi_ring.u4BufPA         = (JUINT32) cfgCdpData.tSrcBufPA[1];          //  Phy Address of pool
       vipi_ring.u4EnRingBuffer  =  (fScalerMode == SCALER_MODE_TILE)? 1 : 0;
       vipi_ring.u4RingSize      = cfgCdpData.tSrcBufRingSize[1]/TILE_STRIDE_UNIT;
       
       
       // VIP2I
       vip2i_ring.eImgFmt        = inputFormat;                //  Image Pixel Format
       vip2i_ring.u4ImgWidth     = cfgCdpData.tSrcImgWidth[2] ;        //  Image Width
       vip2i_ring.u4ImgHeight    = cfgCdpData.tSrcImgHeight[2];        //  Image Height
       vip2i_ring.crop.x         = 0;
       vip2i_ring.crop.y         = 0;
       vip2i_ring.crop.w         = cfgCdpData.tSrcImgWidth[1] ;
       vip2i_ring.crop.h         = cfgCdpData.tSrcImgHeight[1];
       vip2i_ring.u4Offset       = 0;                               //
       vip2i_ring.u4Stride[ESTRIDE_1ST_PLANE]       = 0;//cfgCdpData.tSrcBufStride[2];             //  yuv422 one plane
       vip2i_ring.u4Stride[ESTRIDE_2ND_PLANE]       = 0;
       vip2i_ring.u4Stride[ESTRIDE_3RD_PLANE]       = cfgCdpData.tSrcBufStride[2];
       vip2i_ring.type           = EPortType_Memory;                //  EPortType
       vip2i_ring.index          = EPortIndex_VIP2I;                //  port index
       vip2i_ring.inout          = EPortDirection_In;               //  0:in/1:out
       vip2i_ring.pipePass       = EPipePass_PASS2C;                //  select pass
       vip2i_ring.u4BufSize      = (JUINT32) cfgCdpData.tSrcBufTotalSize[2];  //bytes;  //  Per buffer size
       vip2i_ring.u4BufVA        = (JUINT32) cfgCdpData.tSrcBufVA[2];         //  Vir Address of pool
       vip2i_ring.u4BufPA        = (JUINT32) cfgCdpData.tSrcBufPA[2];         //  Phy Address of pool
       vip2i_ring.u4EnRingBuffer =  (fScalerMode == SCALER_MODE_TILE)? 1 : 0;
       vip2i_ring.u4RingSize     = cfgCdpData.tSrcBufRingSize[2]/TILE_STRIDE_UNIT;
    }


    dispo_ring.eImgFmt        = outputFormat ;//= eImgFmt_YUY2;            //  Image Pixel Format
    dispo_ring.u4ImgWidth     = cfgCdpData.tDstImgWidth;                                     //  Image Width
    dispo_ring.u4ImgHeight    = cfgCdpData.tDstImgHeight;                                     //  Image Height
    dispo_ring.eImgRot        = eImgRot_0;
    dispo_ring.eImgFlip       = eImgFlip_OFF;                          //
    dispo_ring.u4Stride[ESTRIDE_1ST_PLANE]       = cfgCdpData.tDstImgWidth;
    dispo_ring.u4Stride[ESTRIDE_2ND_PLANE]       = 0;
    dispo_ring.u4Stride[ESTRIDE_3RD_PLANE]       = 0;
    dispo_ring.type           = EPortType_DISP_RDMA;        //  EPortType
    dispo_ring.index          = EPortIndex_DISPO;           //  port index
    dispo_ring.inout          = EPortDirection_Out;         //  0:in/1:out
    dispo_ring.u4BufSize      = (JUINT32) cfgCdpData.tDstBufSize;   //bytes;  //  Per buffer size
    dispo_ring.u4BufVA        = (JUINT32) cfgCdpData.tDstBufVA;    //  Vir Address of pool
    dispo_ring.u4BufPA        = (JUINT32) cfgCdpData.tDstBufPA;    //  Phy Address of pool

   
   return true ;
}



int ImgCdpPipe::onHWScalerCreate(JUINT32 preferMode)
{

   scenario_init = eScenarioID_RESERVED ;
   ATRACE_CALL();
   
	 android::Tracer::traceBegin(ATRACE_TAG_GRAPHICS,"SCALER_CDP_CREATE");
   if( pResMgr == NULL){
     pResMgr = ResMgrDrv::CreateInstance();
     pResMgr->Init();
   }
   
#if 0 // def SCALER_EP
   
   ResMgrMode.Dev = RES_MGR_DRV_DEV_CAM;
   ResMgrMode.ScenSw = RES_MGR_DRV_SCEN_SW_NONE;
   ResMgrMode.ScenHw = RES_MGR_DRV_SCEN_HW_NONE;
   pResMgr->SetMode(&ResMgrMode);
   
#endif

   // config cdppipe for ring buffer
   pResMgr->GetMode(&ResMgrMode);

   if(preferMode == SCALER_MODE_LINK){
     if (ResMgrMode.ScenSw==RES_MGR_DRV_SCEN_SW_NONE) { //Marx suggest to check ScenSw
         scenario_init = eScenarioID_GDMA;//eScenarioID_VR;
     }else{      
       IMGRSZ_W("CdpCreate: Mgr Dev %d, Scen %d, L:%d!!\n", ResMgrMode.Dev, ResMgrMode.ScenHw,__LINE__);       
     }

   }else if(preferMode == SCALER_MODE_TILE || preferMode == SCALER_MODE_FRAME){
#if 0 //temporary disable CC mode
     if(ResMgrMode.Dev==RES_MGR_DRV_DEV_CAM && ResMgrMode.ScenHw==RES_MGR_DRV_SCEN_HW_VSS) {
         scenario_init = eScenarioID_VSS_CDP_CC;
     } else if (ResMgrMode.Dev==RES_MGR_DRV_DEV_CAM && ResMgrMode.ScenHw==RES_MGR_DRV_SCEN_HW_ZSD) {
         scenario_init = eScenarioID_ZSD_CDP_CC;
     } else if (ResMgrMode.Dev==RES_MGR_DRV_DEV_CAM && ResMgrMode.ScenHw==RES_MGR_DRV_SCEN_HW_NONE) {
#else
     if (ResMgrMode.ScenSw==RES_MGR_DRV_SCEN_SW_NONE) { //Marx suggest to check ScenSw
#endif     
         scenario_init = eScenarioID_VR;
     }else{
       IMGRSZ_W("CdpCreate: Mgr Dev %d, Scen %d, L:%d!!\n", ResMgrMode.Dev, ResMgrMode.ScenHw,__LINE__);
       //return return code_return(" tile mode busy, try another mode, ",__LINE__, false);
     }
   }


   
   if( scenario_init == eScenarioID_RESERVED){
      
      return code_return(" can't set Prefer mode , try another mode, ",__LINE__, false);
   }
   
   
   
   
   //======================================
   // create and init pipi manager
   //======================================
   if(pPipeMgr == NULL){
     pPipeMgr = PipeMgrDrv::CreateInstance();
     pPipeMgr->Init(); 
   }else{
     IMGRSZ_W("::onHWScalerCreate, pCdpPipeRing already create %x, L:%d!!\n", (unsigned int)pCdpPipeRing, __LINE__);     
   }

   if(!lockScaler() ){
       return code_return("::lockScaler fail, ",__LINE__,false) ;
   }

   //======================================
   // create and init cdp pipe 
   //======================================
   if(pCdpPipeRing == NULL){
     android::Tracer::traceEnd(ATRACE_TAG_GRAPHICS);  
	   android::Tracer::traceBegin(ATRACE_TAG_GRAPHICS,"SCALER_CDP_PIPE_CREATE");
      pCdpPipeRing = ICdpPipe::createInstance(scenario_init, eScenarioFmt_RAW /*eScenarioFmt_YUV*/); 
     android::Tracer::traceEnd(ATRACE_TAG_GRAPHICS);  
	   android::Tracer::traceBegin(ATRACE_TAG_GRAPHICS,"SCALER_CDP_PIPE_INIT");
      pCdpPipeRing->init();
     android::Tracer::traceEnd(ATRACE_TAG_GRAPHICS);  
	   android::Tracer::traceBegin(ATRACE_TAG_GRAPHICS,"SCALER_CDP_PIPE_RESET");
      //if(preferMode == SCALER_MODE_LINK)
      {
        IMGRSZ_D("::onHWScalerCreate, reset_ISP, L:%d!!\n", __LINE__);   
        pCdpPipeRing->sendCommand(EPIPECmd_ISP_RESET, 0, 0, 0);        
      }
      pCdpPipeRing->sendCommand(EPIPECmd_SET_CQ_CHANNEL,(MINT32)EPIPE_PASS2_CQ3,0,0);
   }else{
     IMGRSZ_W("::onHWScalerCreate, pCdpPipeRing already create %x, L:%d!!\n", (unsigned int)pCdpPipeRing, __LINE__);   
   }
   
   fScalerMode =  preferMode ;
    memset( &imgi_ring, 0 , sizeof(PortInfo));
    memset( &vipi_ring, 0 , sizeof(PortInfo));
    memset(&vip2i_ring, 0 , sizeof(PortInfo));
    //for(i = 0;i< TILE_ROW_SIZE; i++)                  
      memset(&tRowCfg[0],0,sizeof(TILE_ROW_DATA)*TILE_ROW_SIZE);   
      
	 android::Tracer::traceEnd(ATRACE_TAG_GRAPHICS);  

  return true;
   
   
}


int ImgCdpPipe::onHWScalerDeInit()
{
   IMGRSZ_W("::ScalerDeInit, reset_flag %d, L:%d!!\n", fResetISP,__LINE__);   
   
   ATRACE_CALL();
   if(fIsUseCmodel )
      return true;
   
   //Deinit CDP pipe if last Tile   
   //destroy cdp pipe 
   if(pCdpPipeRing != NULL){
	   android::Tracer::traceBegin(ATRACE_TAG_GRAPHICS,"SCALER_CDP_PIPE_Uninit");
     if( fScalerMode == SCALER_MODE_LINK && fResetISP){
         IMGRSZ_W("::ScalerDeInit, resetISP %d, L:%d!!\n",fResetISP, __LINE__);   
         pCdpPipeRing->sendCommand(EPIPECmd_ISP_RESET, 0, 0, 0);   
     }
     pCdpPipeRing->uninit();
     pCdpPipeRing->destroyInstance();
     pCdpPipeRing = NULL;
	   android::Tracer::traceEnd(ATRACE_TAG_GRAPHICS);  
   }


   if(pPipeMgr != NULL){
     if(!unlockScaler(1) ){
         return code_return("::unlockScaler fail, ",__LINE__,false) ;
     } 
	   android::Tracer::traceBegin(ATRACE_TAG_GRAPHICS,"SCALER_PIPE_MGR_Uninit");
     pPipeMgr->Uninit();
     pPipeMgr->DestroyInstance();
     pPipeMgr = NULL;
	   android::Tracer::traceEnd(ATRACE_TAG_GRAPHICS);  
   }

   if(pResMgr != NULL){
     android::Tracer::traceBegin(ATRACE_TAG_GRAPHICS,"SCALER_RES_MGR_Uninit");
     pResMgr->Uninit();
     pResMgr->DestroyInstance();
     pResMgr = NULL ;
	   android::Tracer::traceEnd(ATRACE_TAG_GRAPHICS);  
   }



   if(fScalerMode == SCALER_MODE_TILE){
     //if(tTileCfgBuf != NULL) free(tTileCfgBuf);
     //tTileCfgBuf = NULL;
     IMGDEC_FREE(tTileCfgBuf_org);
   }
   IMGRSZ_W("::ScalerDeInit done, L:%d!!\n", __LINE__);     
   return true; 
   
}


int ImgCdpPipe::onTileConfig()
{
   ATRACE_CALL();
    vector<PortInfo const*> vCdpRingInPorts(3);
    vector<PortInfo const*> vCdpRingOutPorts(3);

   if(fSrcPlaneNum > 1){
     vCdpRingInPorts.resize(3);
     vCdpRingInPorts.at(0) = &imgi_ring;
     vCdpRingInPorts.at(1) = &vipi_ring;
     vCdpRingInPorts.at(2) = &vip2i_ring;
   }else{
     vCdpRingInPorts.resize(1);
     vCdpRingInPorts.at(0) = &imgi_ring;
   }
   vCdpRingOutPorts.resize(1);
   vCdpRingOutPorts.at(0) = &dispo_ring;




   pCdpPipeRing->sendCommand(EPIPECmd_SET_CONFIG_STAGE,(MINT32)eConfigSettingStage_Init,0,0);   
   pCdpPipeRing->configPipe(vCdpRingInPorts, vCdpRingOutPorts);

   if( cfgCdpData.tConfigResult != TPIPE_MESSAGE_OK){
     IMGRSZ_W("Tile Config Fail %d!!\n", cfgCdpData.tConfigResult);
     return false ;  
   }
      


   return true;
}

bool ImgCdpPipe::lockScaler()
{
      
   EScenarioID scenario_curr  = eScenarioID_RESERVED;
   
   RES_MGR_DRV_MODE_STRUCT CurResMgrMode;
      
   // config cdppipe for ring buffer
   pResMgr->GetMode(&CurResMgrMode);
   


   if(CurResMgrMode.Dev != ResMgrMode.Dev || CurResMgrMode.ScenHw != ResMgrMode.ScenHw){
      IMGRSZ_W("TileRun Found Scenario Change %d %d->%d %d!!", ResMgrMode.Dev, ResMgrMode.ScenHw, CurResMgrMode.Dev, CurResMgrMode.ScenHw);
      return false;      
   }

   
   //init
   PIPE_MGR_DRV_LOCK_STRUCT PipeMgrLock;
   //PIPE_MGR_DRV_UNLOCK_STRUCT PipeMgrUnlock;
#if 1 //Marx suggest
    if ( RES_MGR_DRV_SCEN_SW_NONE== ResMgrMode.ScenSw ) {
        //full occcupy for GDMA and tile mode
        //camera wait till JPG unlock
        PipeMgrLock.PipeMask = PIPE_MGR_DRV_PIPE_MASK_CDP_LINK;
    }
    else {
        //cc
        PipeMgrLock.PipeMask = PIPE_MGR_DRV_PIPE_MASK_CDP_CONCUR;
    }
    //IMGRSZ_D("TileRun: PipeMask %d, L:%d!!\n", PipeMgrLock.PipeMask,__LINE__);       
#else
   PipeMgrLock.PipeMask = PIPE_MGR_DRV_PIPE_MASK_CDP_CONCUR;
#endif    
   PipeMgrLock.Timeout = 0;//5000;
   //PipeMgrUnlock.PipeMask = PIPE_MGR_DRV_PIPE_MASK_CDP_CONCUR;

   IMGRSZ_D("TileRun: Lock PipeMgr Cnt %d, L:%d!!\n", fLockCnt,__LINE__);
   if(!fLockCnt)
   {
     //try to lock pipeMgr
     if(!(pPipeMgr->Lock(&PipeMgrLock))) {
         return code_return("::LockMgr fail, ",__LINE__,false) ;
     }   
     fLockCnt++;
   }
   
   return true ;

      
      
}


bool ImgCdpPipe::unlockScaler(unsigned int isUnlockAll)
{

   PIPE_MGR_DRV_UNLOCK_STRUCT PipeMgrUnlock;

#if 1 //Marx suggest
    if ( RES_MGR_DRV_SCEN_SW_NONE== ResMgrMode.ScenSw ) {
       //direct link
       //camera wait till JPG unlock
        PipeMgrUnlock.PipeMask = PIPE_MGR_DRV_PIPE_MASK_CDP_LINK;              
    }
    else {
        //cc
        PipeMgrUnlock.PipeMask = PIPE_MGR_DRV_PIPE_MASK_CDP_CONCUR;       
    }
#else
    PipeMgrUnlock.PipeMask = PIPE_MGR_DRV_PIPE_MASK_CDP_CONCUR;
#endif

   if(fLockCnt){
      if(isUnlockAll){
      
        for(;fLockCnt > 0; fLockCnt--){
           //unlock pipeMgr
           if(!(pPipeMgr->Unlock(&PipeMgrUnlock))) {
               return false ;
           }
        }
      }else{
           //if(!(pPipeMgr->Unlock(&PipeMgrUnlock))) {
           //    return false ;
           //}
           //fLockCnt--;       
      }
   }
   return true ;
   
   
}   

int ImgCdpPipe::onTileRun(unsigned int tileOffset)
{
   
   ATRACE_CALL();
   
#ifdef TILE_1D   
   unsigned int tileOffset = 0;
   
   if(fTileRunCnt != tRowCfg[fTileRunCnt%TILE_ROW_SIZE].start_cnt ){
      IMGRSZ_W("TileRun Found Config Table Mismatch %d->%d!!", fTileRunCnt, tRowCfg[fTileRunCnt%TILE_ROW_SIZE].start_cnt);
      return false ;
   }
   tileOffset = tRowCfg[fTileRunCnt%TILE_ROW_SIZE].start_oft ;
#endif   

   imgi_ring.u4RingTdriBufOffset = tileOffset * sizeof(MINT32);

   IMGRSZ_D("TileRun: startC, Cnt %d/%d, Ofset %d, L:%d!!\n", fTileRunCnt,fTileTotalRun, tileOffset, __LINE__);

   if( !onCdpGo())
      return code_return("trigger CDP fail, ",__LINE__, false);

   if(!onCdpWaitIrq())
      return code_return("wait CDP fail, ",__LINE__, false);
   

    

#if 0   
   if( fTileCfgCnt < fTileTotalRun ){ 
     IMGRSZ_D("TileRun: Find Next Tile Window, L:%d!!\n", __LINE__);
     findNxtTileWindow( tRowCfg[(fTileCfgCnt-1)%TILE_ROW_SIZE].entry_nxt_start , fTileCfgCnt, &tRowCfg[fTileCfgCnt%TILE_ROW_SIZE]);
     fTileCfgCnt++;
   }
#endif   
   
   
   return true;
}


bool ImgCdpPipe::onCdpWaitIrq()
{
   ATRACE_CALL();
#ifdef TILE_1D
   PIPE_MGR_DRV_UNLOCK_STRUCT PipeMgrUnlock;
   PipeMgrUnlock.PipeMask = PIPE_MGR_DRV_PIPE_MASK_CDP_CONCUR;
   
   unsigned int irq_cnt = 0;

   do{
      
     IMGRSZ_D("TileRun: WaitIRQ(%d/%d), L:%d!!\n", irq_cnt, fIRQinRow, __LINE__);
     //wait CDP pipe done      
     if(!pCdpPipeRing->irq(EPipePass_PASS2C,EPIPEIRQ_PATH_DONE))
        return code_return("::wait_cdp_irq_fail, ",__LINE__,false);
   }while( ++irq_cnt < fIRQinRow );  //only tile may has multi-irq in Row

   IMGRSZ_D("TileRun: Unlock PipeMgr, L:%d!!\n", __LINE__);


   //unlock pipeMgr
   if(!(pPipeMgr->Unlock(&PipeMgrUnlock))) {
       return false ;
   }   
#else

   IMGRSZ_D("TileRun: WaitIRQ(%d/%d), L:%d!!\n", fTileGoCntInRow, fIRQinRow, __LINE__);
   //wait CDP pipe done      
   if(!pCdpPipeRing->irq(EPipePass_PASS2C,EPIPEIRQ_PATH_DONE)){
      fResetISP = 1;
      return code_return("::wait_cdp_irq_fail, ",__LINE__,false);
   }

   IMGRSZ_D("TileRun: Unlock PipeMgr, L:%d!!\n", __LINE__);

   if(!unlockScaler(0) ){
       return code_return("::unlockScaler fail, ",__LINE__,false) ;
   }         
#endif   


{
       static unsigned int fileNameIdx = 0;
       if(dump2file(DUMP_CDP_LVL_STEP_OUT,"dumpCdpPipe_OutStepAf", &fileNameIdx, (unsigned char*)fDstConfigAddr, fDstBufSize))
         fileNameIdx++;
     
}


   return true ;     
   
}


bool ImgCdpPipe::onCdpGo()
{
   
   
   vector<PortInfo const*> vCdpRingInPorts(3);
   vector<PortInfo const*> vCdpRingOutPorts(3);

   android::Tracer::traceBegin(ATRACE_TAG_GRAPHICS,"SCALER_CDP_CONFIG");

#ifdef TILE_1D 
{  
   EScenarioID scenario_curr  = eScenarioID_RESERVED;
   
   RES_MGR_DRV_MODE_STRUCT CurResMgrMode;


{
       static unsigned int fileNameIdx = 0;
       if(dump2file(DUMP_CDP_LVL_STEP_IN,"dumpCdpPipe_OutStepB4", &fileNameIdx, (unsigned char*)fDstConfigAddr, fDstBufSize))
         fileNameIdx++;
       
}       





   // config cdppipe for ring buffer
   pResMgr->GetMode(&CurResMgrMode);
   
   
      

   if(CurResMgrMode.Dev != ResMgrMode.Dev || CurResMgrMode.ScenHw != ResMgrMode.ScenHw){
      IMGRSZ_W("TileRun Found Scenario Change %d %d->%d %d!!", ResMgrMode.Dev, ResMgrMode.ScenHw, CurResMgrMode.Dev, CurResMgrMode.ScenHw);
      return false;      
   }
   





   //init
   PIPE_MGR_DRV_LOCK_STRUCT PipeMgrLock;
   //PIPE_MGR_DRV_UNLOCK_STRUCT PipeMgrUnlock;
   PipeMgrLock.PipeMask = PIPE_MGR_DRV_PIPE_MASK_CDP_CONCUR;
   PipeMgrLock.Timeout = 0;//5000;
   //PipeMgrUnlock.PipeMask = PIPE_MGR_DRV_PIPE_MASK_CDP_CONCUR;

   IMGRSZ_D("TileRun: Lock PipeMgr, L:%d!!\n", __LINE__);
   //try to lock pipeMgr
   if(!(pPipeMgr->Lock(&PipeMgrLock))) {
       return code_return("::LockMgr fail, ",__LINE__,false) ;
   }   
}
#else

   if(!lockScaler() ){
       return code_return("::lockScaler fail, ",__LINE__,false) ;
   }      
   
#endif   
 
   if(fSrcPlaneNum > 1){
     vCdpRingInPorts.resize(3);
     vCdpRingInPorts.at(0) = &imgi_ring;
     vCdpRingInPorts.at(1) = &vipi_ring;
     vCdpRingInPorts.at(2) = &vip2i_ring;
   }else{
     vCdpRingInPorts.resize(1);
     vCdpRingInPorts.at(0) = &imgi_ring;      
   }
   vCdpRingOutPorts.resize(1);
   vCdpRingOutPorts.at(0) = &dispo_ring;

   //IMGRSZ_D("TileRun: Send Command UpdateTrigger, L:%d!!\n", __LINE__);
   pCdpPipeRing->sendCommand(EPIPECmd_SET_CONFIG_STAGE,(MINT32)eConfigSettingStage_UpdateTrigger,0,0);
   //IMGRSZ_D("TileRun: ConfigPipe, L:%d!!\n", __LINE__);

   //configure pipe
   if(!pCdpPipeRing->configPipe(vCdpRingInPorts, vCdpRingOutPorts))
      return code_return("::configPipe_fail, ",__LINE__,false);

#if 0
   if(fScalerMode == SCALER_MODE_LINK ){
     //enable GDMA 
     pCdpPipeRing->sendCommand(EPIPECmd_SET_GDMA_LINK_EN,1,0,0);   
   }
#endif
      
   //IMGRSZ_D("TileRun: syncJpegPass2C, L:%d!!\n", __LINE__);
   android::Tracer::traceEnd(ATRACE_TAG_GRAPHICS);  
   android::Tracer::traceBegin(ATRACE_TAG_GRAPHICS,"SCALER_CDP_SYNC");
   if(!pCdpPipeRing->syncJpegPass2C())
      return code_return("::sync_b4_cdp_trigger_fail, ",__LINE__,false);
   android::Tracer::traceEnd(ATRACE_TAG_GRAPHICS);  
   android::Tracer::traceBegin(ATRACE_TAG_GRAPHICS,"SCALER_CDP_GO");
      

   IMGRSZ_D("TileRun: start, L:%d!!\n", __LINE__);

   
   //CDP pipe start
   if(!pCdpPipeRing->start())
     return code_return("::trigger_cdp_fail, ",__LINE__,false);
   
      
	  android::Tracer::traceEnd(ATRACE_TAG_GRAPHICS);        
   
   return true;
}

void* ImgCdpPipe::pt_TileStart(void *data)
{
  onTileStart();
  return NULL;   
}   

void* ipt_TileStart(void *data)
{
  //onTileStart();
  unsigned int i= 0;
  for(;i<2;i++){
    IMGRSZ_D("Tile: thread run i %d!!\n", i);    
    sleep(1);
  }
  
  return NULL;   
}   

   

bool ImgCdpPipe::onTileStart()
{
    unsigned int offset = 0;
    unsigned int idx =0;
    fTileGoCntInRow = 0;
    
    do{
      idx = fTileRunCnt%TILE_ROW_SIZE ;
       if(fTileRunCnt != tRowCfg[idx].start_cnt ){
          IMGRSZ_W("TileRun Found Config Table Mismatch %d->%d!!", fTileRunCnt, tRowCfg[idx].start_cnt);
          return false ;
       }
       offset = tRowCfg[idx].start_oft[fTileGoCntInRow] ;      
       
       if(!onTileRun(offset))
         return code_return("::onTileRun fail, ",__LINE__, false);
         
    }while( ++fTileGoCntInRow < fIRQinRow );

    if(fScalerMode == SCALER_MODE_TILE ){
      
      if( (fMemType == IMG_MEM_TYPE_PHY || fMemType == IMG_MEM_TYPE_PMEM) && (fIsUseCmodel == 0)){
        IMGRSZ_D("onTile Done memcpy L:%d: %x, %x, %x!!\n", __LINE__, fDstBufAddr, fDstPmemVA, fDstBufSize);
        memcpy( (unsigned char*)fDstBufAddr, (unsigned char*)fDstPmemVA, fDstBufSize );
      }
        
    }
    
    fTileRunCnt++;
    

      
      
    return true;

}


bool ImgCdpPipe::onSwStart()
{



      CONV_YUV2RGB_DATA convYR ;
      unsigned int srcRowOffset[3];
      unsigned int dstRowOffset;
      
      if(fSrcFormat == SCALER_IN_3P_YUV420){
        convYR.srcFormat = 0;  //0: YUV420, 1: YUV422, 2:GRAY
      }else if(fSrcFormat == SCALER_IN_3P_YUV422)
        convYR.srcFormat = 1;  
      else if(fSrcFormat == SCALER_IN_1P_GRAY)
         convYR.srcFormat = 2;  

      if( fDstFormat == SCALER_OUT_1P_ARGB8888 ){
        convYR.dstFormat = 0;  //0: ARGB8888, 1:RGB888, 2:RGB565
        convYR.pixelSize = 4;
      }else if( fDstFormat == SCALER_OUT_1P_RGB888 ){
         convYR.dstFormat = 1;
         convYR.pixelSize = 3;
      }else if( fDstFormat == SCALER_OUT_1P_RGB565 ){
         convYR.dstFormat = 2;
         convYR.pixelSize = 2;
      }

      srcRowOffset[0] = fCurRowStart * fSrcBufStride[0] * fRowHeight[0] ;
      srcRowOffset[1] = fCurRowStart * fSrcBufStride[1] * fRowHeight[1] ;
      srcRowOffset[2] = fCurRowStart * fSrcBufStride[2] * fRowHeight[2] ;
         dstRowOffset = fCurRowStart * fSrcImgWidth[0] * fRowHeight[0]* convYR.pixelSize ;
      
      convYR.srcWidth = fSrcImgWidth[0];
      convYR.srcHeight = fRowHeight[0];
      convYR.srcMemStride[0] = fSrcBufStride[0];
      convYR.srcMemStride[1] = fSrcBufStride[1];
      convYR.srcMemStride[2] = fSrcBufStride[2]; 
      
      convYR.srcAddr[0] = (unsigned char*)( fSrcBufAddr0[0] + srcRowOffset[0]);
      convYR.srcAddr[1] = (unsigned char*)( fSrcBufAddr0[1] + srcRowOffset[1]);
      convYR.srcAddr[2] = (unsigned char*)( fSrcBufAddr0[2] + srcRowOffset[2]);
      convYR.dst_buf = (unsigned char*)(fDstBufAddr + dstRowOffset);
      

                  
      IMGRSZ_D("IMG_RSZ_PATH_CMODEL:[%d, %d] %x %x %x %x !! \n", fTileRunCnt, fSrcImgMcuRow,convYR.srcAddr[0], convYR.srcAddr[1], convYR.srcAddr[2], convYR.dst_buf);      
      
      convYUV2RGB(&convYR);
      
      //update tile idx
      fCurRowStart = fNxtRowStart ;
      fCurRowEnd = fNxtRowEnd ;
      if(fNxtRowStart   < fSrcImgMcuRow){ 
         fNxtRowStart++ ; 
         fNxtRowEnd = fNxtRowStart+1; 
         }
      

       IMGRSZ_D("IMG_RSZ_PATH_CMODEL:[%d, %d] {%d, %d}, {%d, %d}!! \n", fTileRunCnt, fSrcImgMcuRow,fCurRowStart, fCurRowEnd, fNxtRowStart, fNxtRowEnd);      
      
       fTileRunCnt++;
      return true;      
 

}


//(00) start_no(0000) end_no(0002) stop_f(0001) offset(0000)
//(01) start_no(0002) end_no(0004) stop_f(0001) offset(0028)
//(02) start_no(0004) end_no(0006) stop_f(0001) offset(0056)

#if 0
 
//if there is next tile, go find the next tileRow Start & End index
bool ImgCdpPipe::findNxtTileWindow(JUINT32 entry_start, JUINT32 run_cnt, TILE_ROW_DATA *row_data )//,int *tRow_start, int *tRow_end)
{
   unsigned int tileCfg_item_init = entry_start;
   unsigned int tileCfg_item = tileCfg_item_init;
   unsigned int total_entry = cfgCdpData.tCfgEntryNum ;
   //cur_row->row_start = -1;
   //int tRow_start = -1;
   //int tRow_end = -1;
   

      
   
/*   s e f oft(package)
   +--------+
   | s e 0 t| (zero flag is optional, but first item offset is need to set to HW)
   |     0 -|
   +--------+
   |     1 -|
   +--------+
*/   
   
   /* find the next start_idx & end_idx */
    // now the pointer is point to first item of package, 
    // we need to sync the stop_flag (stop_flag ==1) to make sure the pointer to the lsst item of the package 
   if (tTileCfgBuf[tileCfg_item].tpipe_stop_flag == 0){
       for( ; tTileCfgBuf[tileCfg_item].tpipe_stop_flag != 1 ; tileCfg_item++ ){
         if(tileCfg_item >= total_entry ){
           IMGRSZ_W("TileRun: Error in Find next Tile Window start from index %d >= %d!!", tileCfg_item, cfgCdpData.tCfgEntryNum);
           return false;
         }       
       }
   }

   IMGRSZ_D("TileRun: Get Tile Info [%d] from index [%d %d] -> info [%d %d %d %d]!!", run_cnt, tileCfg_item_init, tileCfg_item, 
   tTileCfgBuf[tileCfg_item_init].mcu_buffer_start_no, tTileCfgBuf[tileCfg_item_init].mcu_buffer_end_no, 
   tTileCfgBuf[tileCfg_item_init].tpipe_stop_flag, tTileCfgBuf[tileCfg_item_init].dump_offset_no);   
   
   row_data->start_cnt        = run_cnt ;
   row_data->start_oft        = tTileCfgBuf[tileCfg_item_init].dump_offset_no ;
   row_data->row_start        = tTileCfgBuf[tileCfg_item_init].mcu_buffer_start_no ;
   row_data->row_end          = tTileCfgBuf[tileCfg_item_init].mcu_buffer_end_no ;
   row_data->entry_start      = tileCfg_item_init;
   row_data->entry_nxt_start  = tileCfg_item +1;
   row_data->irq_per_row      = row_data->entry_nxt_start - row_data->entry_start ;
   
   //*tEntry = tileCfg_item+1;
 
   return true;  
}

#else
 
//if there is next tile, go find the next tileRow Start & End index
bool ImgCdpPipe::findNxtTileWindow(JUINT32 entry_start, JUINT32 run_cnt, TILE_ROW_DATA *row_data )//,int *tRow_start, int *tRow_end)
{
   unsigned int tileCfg_item_init = entry_start;
   unsigned int tileCfg_item = tileCfg_item_init;
   unsigned int total_entry = cfgCdpData.tCfgEntryNum ;
   unsigned int irq_cnt = 0;
   unsigned int i= 0;
   unsigned int offset = 0;
   unsigned int tile_cnt =0;
   //cur_row->row_start = -1;
   //int tRow_start = -1;
   //int tRow_end = -1;
   

   if(entry_start >= total_entry ){
     IMGRSZ_W("TileRun: Error in Find next Tile Window start from index %d >= %d!!", entry_start, cfgCdpData.tCfgEntryNum);
     return false;
   }      
   
/*   s e f oft(package)
   +--------+
   | s e 0 t| (zero flag is optional, but first item offset is need to set to HW)
   |     0 -|
   +--------+
   |     1 -|
   +--------+
*/   
   
   /* find the next start_idx & end_idx */
    // now the pointer is point to first item of package, 
    // we need to sync the stop_flag (stop_flag ==1) to make sure the pointer to the lsst item of the package 
   //if (tTileCfgBuf[tileCfg_item].tpipe_stop_flag == 0){

       
       for( ; tileCfg_item < total_entry &&
              tTileCfgBuf[tileCfg_item_init].mcu_buffer_start_no ==  tTileCfgBuf[tileCfg_item].mcu_buffer_start_no &&
              tTileCfgBuf[tileCfg_item_init].mcu_buffer_end_no   ==  tTileCfgBuf[tileCfg_item].mcu_buffer_end_no 
            ; tileCfg_item++ )
       {
         
         //IMGRSZ_D("TileRun:[DEBUG] Tile Info [%d] item [%d %d %d %d]!!\n",tileCfg_item, 
         //tTileCfgBuf[tileCfg_item].mcu_buffer_start_no, tTileCfgBuf[tileCfg_item].mcu_buffer_end_no
         //,tTileCfgBuf[tileCfg_item].tpipe_stop_flag , tTileCfgBuf[tileCfg_item].dump_offset_no 
         //);
         
         if(tile_cnt == 0){
           row_data->start_oft[irq_cnt] = tTileCfgBuf[tileCfg_item].dump_offset_no ;            
         }
         tile_cnt++;
         
         if(tTileCfgBuf[tileCfg_item].tpipe_stop_flag == 1){

            if(irq_cnt == TILE_MAX_ROW_IRQ_SIZE)
               code_return("::IRQ Cnt hit MAX_IRQ_SIZE ",__LINE__, false);            
#if 0            
            row_data->start_oft[irq_cnt] = tTileCfgBuf[tileCfg_item].dump_offset_no ;            
#else
            
            tile_cnt = 0;            
#endif            
            irq_cnt ++ ;
         }

       }
   //}
          

   IMGRSZ_D("TileRun: Get Tile Info [%d] from index [%d %d] -> info [%d %d %d %d]!!", run_cnt, tileCfg_item_init, tileCfg_item, 
   tTileCfgBuf[tileCfg_item_init].mcu_buffer_start_no, tTileCfgBuf[tileCfg_item_init].mcu_buffer_end_no, 
   tTileCfgBuf[tileCfg_item_init].tpipe_stop_flag, tTileCfgBuf[tileCfg_item_init].dump_offset_no);  
#if 0
   if( irq_cnt > 1 ){   
      IMGRSZ_D("TileRun: ----> Row IRQ :: ");
     for(i=0; i < irq_cnt ;i++){
       IMGRSZ_D("[%d] %d, ", i , row_data->start_oft[i]);  
     }
   }
#endif
   
   row_data->start_cnt        = run_cnt ;
#ifdef TILE_1D   
   row_data->start_oft        = tTileCfgBuf[tileCfg_item_init].dump_offset_no ;
#endif   
   row_data->row_start        = tTileCfgBuf[tileCfg_item_init].mcu_buffer_start_no ;
   row_data->row_end          = tTileCfgBuf[tileCfg_item_init].mcu_buffer_end_no ;
   row_data->entry_start      = tileCfg_item_init;
   row_data->entry_nxt_start  = tileCfg_item ;
   row_data->irq_per_row      = irq_cnt ;

   if(entry_start == 0 && irq_cnt > 1) // only remember once
     fIRQinRow = irq_cnt ;
   else if(irq_cnt > 1 && run_cnt > 0 && fIRQinRow != irq_cnt){
      IMGRSZ_D("Warning: Row IRQ mismatch IRQ %d, [%d] %d!!\n", fIRQinRow, run_cnt,irq_cnt);
   }
      
   
   
   //*tEntry = tileCfg_item+1;
 
   return true;  
}

#endif

INT32 ImgCdpPipe::TileInit()
{
   unsigned int i =0;
   if(fScalerMode != SCALER_MODE_TILE){
      return true ;
   }
   
   ATRACE_CALL();
   fTileTotalRun= 0;
   
   if(!allocIOMEM())
     fail_return("::[INIT] Image Scaler allocate IOMEM fail");   
   
#if 0   
   tTileCfgBuf = (ISPIO_TDRI_INFORMATION_STRUCT*)malloc(fTileCfgBufSize);
#else
   IMGDEC_ALLOC(tTileCfgBuf_org, fTileCfgBufSize, 512, tTileCfgBuf) ;
#endif   

   if(tTileCfgBuf == NULL)
      return code_return("::[INIT] Allocate CfgBuf Fail, ", __LINE__,false);
   
   if(!configCdpIO())
      return code_return("::[INIT] config Scaler IO Fail, ", __LINE__,false);
   
   if(!onTileConfig())
      return code_return("::[INIT] config Tile Fail, ", __LINE__,false);
   
   
   fTileTotalRun = cfgCdpData.tCfgTotalRun ;
   
   if(fTileTotalRun > TILE_ROW_SIZE){
      IMGRSZ_W("::[INIT] goScalerNum is over max (%d) table size!!\n", fTileTotalRun, TILE_ROW_SIZE);
      return code_return("::[INIT] get cfgTable fail, ",__LINE__,false);
   }
#if 0
   for(i = 0 ;i < cfgCdpData.tCfgEntryNum;i++)
         IMGRSZ_D("TileRun:[DEBUG] Tile Info [%d] item [%d %d %d %d]!!\n",i, 
         tTileCfgBuf[i].mcu_buffer_start_no, tTileCfgBuf[i].mcu_buffer_end_no
         ,tTileCfgBuf[i].tpipe_stop_flag , tTileCfgBuf[i].dump_offset_no  ); 


#endif   


	  android::Tracer::traceBegin(ATRACE_TAG_GRAPHICS,"SCALER_BUILD_TABLE");   
   tRowCfg[0].entry_nxt_start = 0;
   fTileCfgCnt = 0;
   if( !findNxtTileWindow( tRowCfg[(fTileCfgCnt)%TILE_ROW_SIZE].entry_nxt_start , fTileCfgCnt, &tRowCfg[fTileCfgCnt%TILE_ROW_SIZE]) )
       return code_return("::[INIT] findGoFlag Fail, ", __LINE__,false);

   for( fTileCfgCnt = 1; (fTileCfgCnt < fTileTotalRun) /*&& (fTileCfgCnt < (TILE_ROW_SIZE/2))*/; fTileCfgCnt++){
     if( !findNxtTileWindow( tRowCfg[(fTileCfgCnt-1)%TILE_ROW_SIZE].entry_nxt_start , fTileCfgCnt, &tRowCfg[fTileCfgCnt%TILE_ROW_SIZE]) )
       return code_return("::[INIT] findGoFlag Fail, ", __LINE__,false);
   }
   android::Tracer::traceEnd(ATRACE_TAG_GRAPHICS);        
   if(fTileCfgCnt != fTileTotalRun){
      IMGRSZ_W("::[INIT] goScalerCnt %d is not match cfgStopNum %d!!\n", fTileCfgCnt, fTileTotalRun);
      return code_return("::[INIT] get cfgTable fail, ",__LINE__,false);
   }
      
   
   
    return true;   
}



bool ImgCdpPipe::alloc_m4u()
{

    unsigned int en_mci = 0;

	  android::Tracer::traceBegin(ATRACE_TAG_GRAPHICS,"SCALER_M4U_ALLOC");

    
    if(!imgMmu_create(&pM4uDrv, fm4uCdpID))
      return false; 
          
    if(!imgMmu_reset(&pM4uDrv, fm4uCdpID))
      return false;       

    //src back0 Y, Cb, Cr buffer
    if(!imgMmu_alloc_pa(&pM4uDrv, fm4uCdpID, (JUINT32) fSrcBufAddr0[0], fSrcBufSize[0], &fSrcM4UAddrMva0[0]))
      return false;
    if( fSrcBufAddr0[1] && fSrcBufSize[1] && (!imgMmu_alloc_pa(&pM4uDrv, fm4uCdpID, (JUINT32) fSrcBufAddr0[1], fSrcBufSize[1], &fSrcM4UAddrMva0[1])) )
      return false;   
    if( fSrcBufAddr0[2] && fSrcBufSize[2] && (!imgMmu_alloc_pa(&pM4uDrv, fm4uCdpID, (JUINT32) fSrcBufAddr0[2], fSrcBufSize[2], &fSrcM4UAddrMva0[2])) )
      return false;
      

    if(fScalerMode == SCALER_MODE_LINK){
      //src back1 Y, Cb, Cr buffer
      if(!imgMmu_alloc_pa(&pM4uDrv, fm4uCdpID, (JUINT32) fSrcBufAddr1[0], fSrcBufSize[0], &fSrcM4UAddrMva1[0]))
        return false;
      if(fSrcBufAddr0[1] && fSrcBufSize[1] && (!imgMmu_alloc_pa(&pM4uDrv, fm4uCdpID, (JUINT32) fSrcBufAddr1[1], fSrcBufSize[1], &fSrcM4UAddrMva1[1])) )
        return false;   
      if(fSrcBufAddr0[2] && fSrcBufSize[2] && (!imgMmu_alloc_pa(&pM4uDrv, fm4uCdpID, (JUINT32) fSrcBufAddr1[2], fSrcBufSize[2], &fSrcM4UAddrMva1[2])) )
        return false;
    }
      

    if( fIonEn && fDstFD >= 0 ){
      imgIon_open(&fIonDevFD );
      
      //dst
      imgIon_getVA(fDstFD, fDstBufSize, &fDstIonVA);
      if( !imgIon_getPA(fIonDevFD, fDstFD, fm4uCdpID, fDstIonVA, fDstBufSize, &fDstIonHdle, &fDstM4UAddrMva))
         return false;
      if(!imgMmu_pa_map_tlb(&pM4uDrv, fm4uCdpID , fDstBufSize, &fDstM4UAddrMva))
         return false;      
      
    }else{
      //dst DISPO buffer
      if(en_mci){
        if(!imgMmu_alloc_pa_mci(&pM4uDrv, fm4uCdpID, (JUINT32) fDstBufAddr, fDstBufSize, &fDstM4UAddrMva))
          return false;         
      }else{
        if(!imgMmu_alloc_pa(&pM4uDrv, fm4uCdpID, (JUINT32) fDstBufAddr, fDstBufSize, &fDstM4UAddrMva))
          return false;
         
      }
    }
    
    //config module port
    imgMmu_cfg_port(pM4uDrv, fm4uCdpID, M4U_PORT_IMGI);
    imgMmu_cfg_port(pM4uDrv, fm4uCdpID, M4U_PORT_VIPI);
    imgMmu_cfg_port(pM4uDrv, fm4uCdpID, M4U_PORT_VIP2I);
    if(en_mci){
      imgMmu_cfg_port_mci(pM4uDrv, fm4uCdpID, M4U_PORT_DISPO);
    }else{
      imgMmu_cfg_port(pM4uDrv, fm4uCdpID, M4U_PORT_DISPO);
    }
    
    if(!en_mci){
      //flush cache
      imgMmu_sync(pM4uDrv, fm4uCdpID, (JUINT32) fSrcBufAddr0[0], fSrcBufSize[0], SYNC_HW_READ);
      if(fSrcBufAddr0[1] && fSrcBufSize[1])  imgMmu_sync(pM4uDrv, fm4uCdpID, (JUINT32) fSrcBufAddr0[1], fSrcBufSize[1], SYNC_HW_READ);
      if(fSrcBufAddr0[2] && fSrcBufSize[2])  imgMmu_sync(pM4uDrv, fm4uCdpID, (JUINT32) fSrcBufAddr0[2], fSrcBufSize[2], SYNC_HW_READ);
      if(fScalerMode == SCALER_MODE_LINK){
        imgMmu_sync(pM4uDrv, fm4uCdpID, (JUINT32) fSrcBufAddr1[0], fSrcBufSize[0], SYNC_HW_READ);
        if(fSrcBufAddr1[1] && fSrcBufSize[1]) imgMmu_sync(pM4uDrv, fm4uCdpID, (JUINT32) fSrcBufAddr1[1], fSrcBufSize[1], SYNC_HW_READ);
        if(fSrcBufAddr1[2] && fSrcBufSize[2]) imgMmu_sync(pM4uDrv, fm4uCdpID, (JUINT32) fSrcBufAddr1[2], fSrcBufSize[2], SYNC_HW_READ);      
      }
    }
   if(!en_mci){
     if( fIonEn && fDstFD >= 0 ){
       imgIon_sync(fIonDevFD, fDstIonHdle);
     }else{
       imgMmu_sync(pM4uDrv, fm4uCdpID, (JUINT32) fDstBufAddr, fDstBufSize, SYNC_HW_WRITE);      
     }
   }
	 android::Tracer::traceEnd(ATRACE_TAG_GRAPHICS);  

   
   return true;


}

bool ImgCdpPipe::free_m4u()
{
   
   if(pM4uDrv)
   {
      imgMmu_dealloc_pa(pM4uDrv, fm4uCdpID, (JUINT32) fSrcBufAddr0[0], fSrcBufSize[0]   , &fSrcM4UAddrMva0[0] );
      imgMmu_dealloc_pa(pM4uDrv, fm4uCdpID, (JUINT32) fSrcBufAddr0[1], fSrcBufSize[1]   , &fSrcM4UAddrMva0[1] );
      imgMmu_dealloc_pa(pM4uDrv, fm4uCdpID, (JUINT32) fSrcBufAddr0[2], fSrcBufSize[2]   , &fSrcM4UAddrMva0[2] );
      if(fScalerMode == SCALER_MODE_LINK){
        imgMmu_dealloc_pa(pM4uDrv, fm4uCdpID, (JUINT32) fSrcBufAddr1[0], fSrcBufSize[0]   , &fSrcM4UAddrMva1[0] );
        imgMmu_dealloc_pa(pM4uDrv, fm4uCdpID, (JUINT32) fSrcBufAddr1[1], fSrcBufSize[1]   , &fSrcM4UAddrMva1[1] );
        imgMmu_dealloc_pa(pM4uDrv, fm4uCdpID, (JUINT32) fSrcBufAddr1[2], fSrcBufSize[2]   , &fSrcM4UAddrMva1[2] );
      }
      
      
      if(fIonEn && fDstFD >= 0){
         imgIon_sync(fIonDevFD, fDstIonHdle);         
         imgIon_freeVA(&fDstIonVA, fDstBufSize);         
         imgIon_close(fIonDevFD);
      }else{
        imgMmu_dealloc_pa(pM4uDrv, fm4uCdpID, (JUINT32) fDstBufAddr      , fDstBufSize         , &fDstM4UAddrMva       );
      }
      delete pM4uDrv;
   }   
   pM4uDrv = NULL;
   
   return true;
   
}



bool ImgCdpPipe::allocIOMEM()
{

    struct timeval t1, t2;


    if(fMemType == IMG_MEM_TYPE_PHY){

      // top don't handle input buffer, Image Allocate target memory
      SCALER_PMEM_ALLOC( fDstBufSize, fDstPmemVA, fDstBufAddrPA, fDstPmemFD ) ;      
      IMGRSZ_D("Allocate Destination Pmem, va:0x%x, pa:0x%x, size:%u", fDstPmemVA, fDstBufAddrPA, fDstBufSize);   
      
      fDstConfigAddr       = (JUINT32)fDstPmemVA;
      fDstConfigAddrPA  = (JUINT32)fDstBufAddrPA;
    
    }else if(fMemType == IMG_MEM_TYPE_PMEM){
      
      
      // check if top allocate, or UintTest (only Tile Mode, GDMA can't use PMEM as Src )
      //allocaate Y output buffer
      SCALER_PMEM_ALLOC( fSrcBufSize[0],   fSrcPmemVA, fSrcBufAddrPA0[0], fSrcPmemFD ) ;    
      memcpy( (unsigned char*)fSrcPmemVA, (unsigned char*)fSrcBufAddr0[0], fSrcBufSize[0] );
      IMGRSZ_D("Allocate Source Pmem, va:0x%x, pa:0x%x, size:%u", fSrcPmemVA, fSrcBufAddrPA0[0], fSrcBufSize[0]);
      
      
      //allocate CB output buffer 
      SCALER_PMEM_ALLOC( fSrcBufSize[1], fSrcCbPmemVA, fSrcBufAddrPA0[1], fSrcCbPmemFD ) ;
      memcpy( (unsigned char*)fSrcCbPmemVA, (unsigned char*)fSrcBufAddr0[1], fSrcBufSize[1] );
      
      //allocate CR output buffer 
      SCALER_PMEM_ALLOC( fSrcBufSize[2], fSrcCrPmemVA, fSrcBufAddrPA0[2], fSrcCrPmemFD ) ;
      memcpy( (unsigned char*)fSrcCrPmemVA, (unsigned char*)fSrcBufAddr0[2], fSrcBufSize[2] );
      
      
      // top don't handle Output buffer, Image Allocate target memory
      SCALER_PMEM_ALLOC( fDstBufSize, fDstPmemVA, fDstBufAddrPA, fDstPmemFD ) ;      
      IMGRSZ_D("Allocate Destination Pmem, va:0x%x, pa:0x%x, size:%u", fDstPmemVA, fDstBufAddrPA, fDstBufSize);   
      
      fDstConfigAddr       = (JUINT32)fDstPmemVA;
      fDstConfigAddrPA  = (JUINT32)fDstBufAddrPA;
   

   }else if(fMemType == IMG_MEM_TYPE_M4U){
        if(!alloc_m4u())
           return code_return(" allocate M4U fail, ",__LINE__, false);
     
        
        
        for(unsigned int i=0; i<3;i++){
         //fSrcBufAddr0[i] = fSrcBufAddr0[i];
         //fSrcBufAddr1[i] = fSrcBufAddr1[i];
         fSrcBufAddrPA0[i] = fSrcM4UAddrMva0[i];          
         fSrcBufAddrPA1[i] = fSrcM4UAddrMva1[i]; 
        }     
      if(fIonEn && fDstFD >= 0){ 
        fDstConfigAddr    = (JUINT32)fDstIonVA;
      }else{
        fDstConfigAddr    = (JUINT32)fDstBufAddr;
      }
      fDstConfigAddrPA  = (JUINT32)fDstM4UAddrMva;
     
      
   }

     

   return true ;      
      
}

bool ImgCdpPipe::freeIOMEM()
{
   
   ATRACE_CALL();
   IMGRSZ_D("::freeMEM, L:%d!!\n", __LINE__);

       if(fMemType == IMG_MEM_TYPE_PHY){
         
          SCALER_PMEM_FREE(  fTileCfgPmemVA,  fTileCfgBufSize,   fTileCfgPmemFD);
          SCALER_PMEM_FREE(  fDstPmemVA,  fDstBufSize   ,   fDstPmemFD);
       
       }else if( fMemType == IMG_MEM_TYPE_PMEM && (fIsUseCmodel == 0)){
          
          SCALER_PMEM_FREE(  fSrcPmemVA,  fSrcBufSize[0],   fSrcPmemFD);
          SCALER_PMEM_FREE(fSrcCbPmemVA,  fSrcBufSize[1], fSrcCbPmemFD);
          SCALER_PMEM_FREE(fSrcCrPmemVA,  fSrcBufSize[2], fSrcCrPmemFD);
          SCALER_PMEM_FREE(  fDstPmemVA,  fDstBufSize   ,   fDstPmemFD);
          
          SCALER_PMEM_FREE(  fTileCfgPmemVA,  fTileCfgBufSize,   fTileCfgPmemFD);
       
       }else if(fMemType == IMG_MEM_TYPE_M4U){
           
           if(!free_m4u())
             IMGRSZ_W("free M4U fail, L:%d!!\n", __LINE__);
       }   
   
   
   
   return true;
}

bool ImgCdpPipe::onFrameStart()
{
   


   if(fIsUseCmodel){
     //{0,1} => {1,2}
     fCurRowStart = 0 ; 
     fCurRowEnd   = 1 ;
     fNxtRowStart = 1 ;
     fNxtRowEnd   = 2 ;
   }


   if(!configCdpIO())
      return code_return("::[INIT] config Scaler IO Fail, ", __LINE__,false);
   
   if( !onCdpGo())
      return code_return("trigger CDP fail, ",__LINE__, false);


   
   //IMGRSZ_D("IMG_CDP_PATH cfg Tile [%d %d], [%d %d]!!", fCurRowStart, fCurRowEnd, fNxtRowStart, fNxtRowEnd);

   
   return true;   
   
}

bool ImgCdpPipe::configGDMA()
{
    
    ATRACE_CALL();
    GDMA_CTR_STATUS_ENUM result;
    GDMA_HAL_CTL_CONFIG cfgHalCtr ;

    memset(&cfgHalCtr, 0, sizeof(GDMA_HAL_CTL_CONFIG));
    
    
    if(fSrcFormat == SCALER_IN_3P_YUV422  ){
      fGtrSrcIs422 = 1;
      fGtrIsGray = 0;
    }else if(fSrcFormat == SCALER_IN_1P_GRAY){
      fGtrSrcIs422 = 1;
      fGtrIsGray = 1;
    }else{
      fGtrSrcIs422 = 0;
      fGtrIsGray = 0;
    }
    
    
    
    
    cfgHalCtr.gtSrcIs422    = fGtrSrcIs422       ; 
    cfgHalCtr.cbcrConstant  = fGtrUVcons         ;
    cfgHalCtr.isSrcGray     = fGtrIsGray         ;
    cfgHalCtr.isSrcJpeg     = fGtrIsJpeg         ;
    cfgHalCtr.gtSrcWidth    = fSrcImgWidth[0]    ;
    cfgHalCtr.gtSrcHeight   = fSrcImgHeight[0]   ;

    IMGRSZ_D("ConfigGDMA: is422 %d, gray %d, srcJpg %d, w/h %d %d!!\n",cfgHalCtr.gtSrcIs422, cfgHalCtr.isSrcGray, cfgHalCtr.isSrcJpeg, cfgHalCtr.gtSrcWidth, cfgHalCtr.gtSrcHeight);

                     
    for(int i=0;i<3;i++){
      cfgHalCtr.gtSrcBank0[i] = fSrcBufAddrPA0[i];
      cfgHalCtr.gtSrcBank1[i] = fSrcBufAddrPA1[i];
    }
    //IMGRSZ_D("ConfigGDMA:[%d] src %x %x!!\n",i, cfgHalCtr.gtSrcBank0[i], cfgHalCtr.gtSrcBank1[i]);
    IMGRSZ_D("ConfigGDMA:[0] %x %x %x [1] %x %x %x!!\n", cfgHalCtr.gtSrcBank0[0], cfgHalCtr.gtSrcBank0[1],cfgHalCtr.gtSrcBank0[2]
                                                       , cfgHalCtr.gtSrcBank1[0], cfgHalCtr.gtSrcBank1[1],cfgHalCtr.gtSrcBank1[2]);
    
    for(int i=0;i<2;i++){
      
      fGtrBufFstHeight[i] = fRowHeight[i] ;
      fGtrBufLastHeight[i] = ((fSrcImgHeight[i] % fRowHeight[i]) == 0 )? fRowHeight[i] : (fSrcImgHeight[i] % fRowHeight[i]);
      cfgHalCtr.gtSrcBufFstHeight[i]  = fGtrBufFstHeight[i] ;
      cfgHalCtr.gtSrcBufLastHeight[i] = fGtrBufLastHeight[i];
      
      if(fRowHeight[i] == 1)
        cfgHalCtr.gtSrcBufHeight[i] = 0 ;
      else if(fRowHeight[i] <= 2 )
        cfgHalCtr.gtSrcBufHeight[i] = 1 ;
      else if(fRowHeight[i] <= 4 )
        cfgHalCtr.gtSrcBufHeight[i] = 2 ;
      else if(fRowHeight[i] <= 8 )
        cfgHalCtr.gtSrcBufHeight[i] = 3 ;
      else if(fRowHeight[i] <= 16 )
        cfgHalCtr.gtSrcBufHeight[i] = 4 ;
      else if(fRowHeight[i] <= 32 )
        cfgHalCtr.gtSrcBufHeight[i] = 5 ;

      
    }
    IMGRSZ_D("ConfigGDMA: imgH %d %d, rowH[0] %d %d %d, [1] %d %d %d!!\n", fSrcImgHeight[0], fSrcImgHeight[1]
    ,fGtrBufFstHeight[0],fRowHeight[0],fGtrBufLastHeight[0]
    ,fGtrBufFstHeight[1],fRowHeight[1],fGtrBufLastHeight[1]);
                 
    cfgHalCtr.cropY_initOff  = 0;
    cfgHalCtr.cropY_Off      = 0;
    cfgHalCtr.cropC_initOff  = 0;
    cfgHalCtr.cropC_Off      = 0;
        

    
    result = gdmaCtrConfig(decID, &cfgHalCtr) ;
    if(GDMA_CTR_STATUS_OK != result) 
    {
        IMGRSZ_D("config GDMA result:%d !!\n", result);
        return fail_return("GDMA Config fail!!\n");
    }
    
    
                
    return true;
   
   
   
   
}


bool ImgCdpPipe::onGdmaStart()
{
  

#if 0
   if(fScalerMode == SCALER_MODE_LINK ){
     //enable GDMA 
     pCdpPipeRing->sendCommand(EPIPECmd_SET_GDMA_LINK_EN,1,0,0);   
   }
#endif

  if( !configGDMA())
   return fail_return("Image Scaler config GDMA fail");

  
  if( !onFrameStart())
   return fail_return("Image Scaler config GDMA fail");  
  

      
  return true ;      
      
}

static unsigned int pixel_byte[5] = {4,4,2,3,4};



#ifdef IMG_PTHREAD        
  pthread_t tid;
#endif

bool ImgCdpPipe::NStart()
{
   Start();
   return true;
       
}


bool ImgCdpPipe::Start()
{
    unsigned int ret = 0;
    unsigned int draw_cnt= 0, draw_num = 4;
    unsigned int draw_base = fOutWidth*pixel_byte[fDstFormat]*(fOutHeight/2);
    
    if( fScalerMode != SCALER_MODE_TILE || fTileRunCnt == 0){
      IMGRSZ_D("ReSizer Src Addr bank[0]:0x%x 0x%x 0x%x, [1]:0x%x 0x%x 0x%x !! ", fSrcBufAddr0[0], fSrcBufAddr0[1], fSrcBufAddr0[2], fSrcBufAddr1[0], fSrcBufAddr1[1], fSrcBufAddr1[2]);
      IMGRSZ_D("ReSizer Dst Addr:0x%x, w/h:[%d %d]->[%d %d], format:%d->%d", fDstBufAddr, fSrcImgWidth[0], fSrcImgHeight[0],fOutWidth,fOutHeight, fSrcFormat,fDstFormat);
      //IMGRSZ_D("ReSizer Dither:%d, RangeDecode:%d [%d %d %d %d]", isDither, isRangeDecode, fLeft, fTop, fRight, fBottom);
    }
    //Check param
    if(true != checkParam()) {
        return false;
    } 
    
    if( fScalerMode == SCALER_MODE_TILE ){
     
     
      
      if(fIsUseCmodel ){
        if(!onSwStart())
           return code_return("start sw_conv fail, ",__LINE__, false);         
      }else{

#ifdef IMG_PTHREAD
          
       pthread_create(&tid, NULL, ipt_TileStart, NULL);
       if(!onTileStart())
          return code_return("start CDP tail fail, ",__LINE__, false);       
       pthread_join(tid,NULL);          
#else         
        if(!onTileStart())
           return code_return("start CDP tail fail, ",__LINE__, false);
#endif           
      }
        
    }else if(fScalerMode == SCALER_MODE_LINK){

      if(!allocIOMEM())
        fail_return("Image Scaler allocate IOMEM fail");

      #if 0
         if(fScalerMode == SCALER_MODE_LINK ){
           //enable GDMA 
           pCdpPipeRing->sendCommand(EPIPECmd_SET_GDMA_LINK_EN,1,0,0);   
         }
      #endif

      if( !configGDMA())
       return fail_return("Image Scaler config GDMA fail");
            
      if( !onFrameStart())
       return fail_return("Image Scaler config GDMA fail");  
      
    }else if(fScalerMode == SCALER_MODE_FRAME){
      if(!allocIOMEM())
        fail_return("Image Scaler allocate IOMEM fail");


      if( !onFrameStart())
       return fail_return("Image Scaler config frame fail");        
       
      if(!onCdpWaitIrq())
        return code_return("wait CDP fail, ",__LINE__, false);
#if 0 //def JPEG_DEBUG_DRAW_LINE
      IMGRSZ_D("ReSizer::[DRAW_LINE] draw_line [%d %d] in range addr %x %d %d %d", fOutWidth, draw_num, (unsigned int)fDstBufAddr, fOutWidth, fOutHeight, fDstFormat);      
      memset((unsigned char *)(fDstBufAddr+draw_base),0xCD,draw_num*fOutWidth*pixel_byte[fDstFormat]);
#endif   
       
    }else{
       IMGRSZ_W("Unknow Scaler Operation Mode %d!!\n ", fScalerMode);  
       return false;  
    }



    return true;
}


bool ImgCdpPipe::Wait()
{
   if(fScalerMode == SCALER_MODE_LINK){
     if(!onCdpWaitIrq())
        return code_return("wait CDP fail, ",__LINE__, false);      
      return true;
   }
   //else if( fScalerMode == SCALER_MODE_TILE || fScalerMode == SCALER_MODE_FRAME){      
   //   return true;   
   //}
   
   return true;   
}   
