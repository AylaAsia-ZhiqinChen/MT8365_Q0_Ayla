/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
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

#include <stdio.h>
#include <cutils/log.h>
#include <cutils/pmem.h>
//#include <cutils/memutil.h>

#include <stdio.h>
#include <cutils/log.h>
#include <cutils/pmem.h>
//#include <cutils/memutil.h>

#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/mm.h>
#include <linux/slab.h>


#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sched.h>
#include <fcntl.h>


#include <linux/ion.h>
#include <ion_drv.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


//#include "img_dec_comp.h"
//#include "jpeg_dec_comp.h"
//#include "webp_dec_comp.h"
#include "gfmt_hal.h"
#include "img_mmu.h"


#define xlog(...) \
        do { \
            LOGD(__VA_ARGS__); \
        } while (0)

#undef LOG_TAG
#define LOG_TAG "jpeg_test"

extern unsigned char jpg_file[];

//#define TEST_PMEM

//#define BUFFER_SIZE 739
#define BUFFER_SIZE (1920*1088*4)

#undef xlog
#define xlog(...) printf(__VA_ARGS__)

#define SEL_MIN(a,b) ( ((a)<(b)) ? (a):(b) )
#define SEL_MAX(a,b) ( ((a)>(b)) ? (a):(b) )
#define CLIP_255(x) ( ((x) < 0) ? (0): ( SEL_MIN( (x), 255 ) ) )

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

#ifdef TEST_ION
bool alloc_ion (int ionFD, ion_allocation_data *ion_alloc_data, ion_fd_data *fd_data, unsigned int size, unsigned char **src_va)
{



      ion_alloc_data->len = size ;
      ion_alloc_data->align =  256 ;
      ion_alloc_data->flags =  1 << ION_HEAP_ID_MULTIMEDIA ;

        
      if(ioctl(ionFD, ION_IOC_ALLOC, ion_alloc_data))
      {
        printf("IMG_ION: CONFIG module fail!!\n");
        return false;
      }
        

      fd_data->handle = ion_alloc_data->handle ;

      if(ioctl(ionFD, ION_IOC_MAP, fd_data))
      {
        printf("IMG_ION: MAP FD fail!!\n");
        return false;
      }

      xlog("src fd %d , handle: 0x%x, L:%d!!\n", fd_data->fd , ion_alloc_data->handle, __LINE__);


      // get virtual address
      *src_va = (unsigned char *)mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd_data->fd, 0);
      if((*src_va == NULL) || (*src_va == (void*)(-1)))
      {
        xlog("IMG_ION: MMAP fail, fd %d, va %x, size %x!!\n", fd_data->fd, *src_va, size);
        *src_va = NULL;
        return false ;
      }
   
   
   
   return true;
   
   
}

//bool free_ion(int ionFD, ion_allocation_data *ion_alloc_data)
bool free_ion(int ionFD, ion_allocation_data *ion_alloc_data, ion_fd_data *fd_data, unsigned int size, unsigned char **src_va)
{
    
    ion_handle_data handle_data;

      if(*src_va != NULL)
          munmap(*src_va, size);
      *src_va = NULL; 
   
      
      
    if(fd_data->fd > 0)close(fd_data->fd);
    
    handle_data.handle = ion_alloc_data->handle ;
    if(ioctl(ionFD, ION_IOC_FREE, &handle_data))
    {
      printf("IMG_ION: FREE handle fail, L:%d!!\n", __LINE__);
      return false;
    }
   return true;
}
#endif

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
    
   //printf("\nYUV2RGB : %d %d ,dstByteInRow %d, stride %d %d", convY2R->srcWidth, convY2R->srcHeight,dstByteInRow, convY2R->srcMemStride[0], convY2R->srcMemStride[1]);

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
#define JPEG_PMEM_ALLOC(size, va, pa, fd)  \
{ \
    va = (unsigned char *)pmem_alloc_sync(size, &fd);   \
    if(va == NULL) {   \
        printf("Can not allocate PMEM, L:%d!!\n", __LINE__);  \
        return false ;  \
    } \
    pa = (UINT32)pmem_get_phys(fd);       \
    memset(va, 0x00, sizeof(char)*size);    \
}
  

#define JPEG_PMEM_FREE(va, size, fd) \
{  \
   if(va != NULL) { \
       pmem_free(va, size, fd); \
       va = NULL;   \
   }  \
}

#define TO_CEIL(x,a) ( ((unsigned int)(x) + ((a)-1)) & ~((a)-1) )

#define IMGDEC_ALLOC(ptr, size, align, align_ptr) \
{  \
   ptr = (unsigned char *)malloc(size+align) ; \
   align_ptr = (unsigned char *)TO_CEIL(ptr, align);    \
   memset(ptr, 0 , size+align); \
}








int main()
{
    FILE *fp;
    int src_fd;
    int src_uv_fd;
    unsigned long index;
    unsigned char *src_va = NULL;
    unsigned char *alloc_src_va = NULL;    
    
    unsigned char *src_uv_va = NULL;
    unsigned char *alloc_src_uv_va = NULL;    
    unsigned int src_pa  = 0;    
    unsigned int src_uv_pa  = 0;

    enum MEM_TYPE{
      MEM_TYPE_PMEM, 
      MEM_TYPE_M4U, 
      MEM_TYPE_ION,       
    };

    unsigned int    colorBufferPA[3];
    unsigned char*  colorBufferVA[3];
    int             colorBufferFD[3];
    unsigned int    bufferSizeArray[3];    

    unsigned int dst_size, dst_size1 ;  
    unsigned int dst_stride, dst_stride1 ;
    unsigned int img_stride, img_stride1 ;
    unsigned char *alloc_dst_buffer = NULL; 
    unsigned char *alloc_dst_buffer1  = NULL; 
    unsigned char *alloc_dst_buffer2 = NULL ;
    unsigned char *dst_buffer = NULL ; 
    unsigned char *dst_buffer1 = NULL ; 
    unsigned char *dst_buffer2 = NULL ;
    unsigned int testTile = 1;
    unsigned int tileRowNum = 0;
    unsigned int dst_buffer_pa = 0;
    unsigned int dst_buffer1_pa = 0;
    unsigned int dst_buffer2_pa = 0;
    int dst0_fd;
    int dst1_fd;
    int dst2_fd;

    unsigned int test_type = MEM_TYPE_M4U ;//MEM_TYPE_ION;//MEM_TYPE_PMEM;//
    int ionFD ;    

    ion_allocation_data src_ion_alloc_data ;
    ion_allocation_data dst_ion_alloc_data ;
    ion_allocation_data dst_ion_alloc_data1 ;
    ion_allocation_data dst_ion_alloc_data2 ;
    ion_fd_data src_fd_data ;
    ion_fd_data dst_fd_data ;
    ion_fd_data dst_fd_data1 ;
    ion_fd_data dst_fd_data2 ;   
    unsigned int w = 1920;//176;//jpgDec->getJpgWidth(); 
    unsigned int h = 1088;//144;//jpgDec->getJpgHeight();
    
    
    unsigned int in_buffer_size = w*h;//BUFFER_SIZE; 
    unsigned int in_buffer_uv_size = TO_CEIL(w/2, 16)*TO_CEIL(h/2, 16)*2;//BUFFER_SIZE; 
    int gfmtID = 0;

    MTKM4UDrv *pM4uDrv = NULL;
    
    //M4U_MODULE_ID_ENUM fm4uVDecID;
    M4U_MODULE_ID_ENUM fm4uFmtID =  M4U_CLNTMOD_GDMA ;    
    unsigned int fFmtSrcAddrPA[2] ;
    unsigned int fFmtDstAddrPA0[3] ;
    
   long timeout_msec = 5000 ;
   GFMT_RESULT_ENUM fmt_rst ;
    
    
    
   
    printf("go %d !!\n", __LINE__);  

//#if 0
//    JpgDecComp* jpgDec = new JpgDecComp();
//#else
//
//    ImageDecoder* jpgDec = new WebpDecComp();
//#endif    

    printf("go %d !!\n", __LINE__);  


    if(GFMT_STATUS_OK != gfmtLock(&gfmtID))
    {
        xlog("can't lock resource");
        return -1;
    }
    printf("go %d !!\n", __LINE__);  


    //src_va = (unsigned char *)pmem_alloc(in_buffer_size , &src_fd);
    //if(src_va == NULL)
    //{
    //    xlog("Can not allocate memory\n");
    //    return -1;
    //}

    if(test_type == MEM_TYPE_PMEM){
      JPEG_PMEM_ALLOC(in_buffer_size   , src_va   , src_pa   , src_fd);
      JPEG_PMEM_ALLOC(in_buffer_uv_size, src_uv_va, src_uv_pa, src_uv_fd);
    }else if(test_type == MEM_TYPE_M4U){ 
      IMGDEC_ALLOC(alloc_src_va   , in_buffer_size   , 256, src_va   );        
      IMGDEC_ALLOC(alloc_src_uv_va, in_buffer_uv_size, 256, src_uv_va);       
    }else if(test_type == MEM_TYPE_ION){
#ifdef TEST_ION      
      ionFD = open("/dev/ion", O_RDONLY);
      if( ionFD < 0)
      {
        printf("IMG_ION: open fd fail!!\n");
        return false;
      }
      //alloc_ion (int ionFD, ion_allocation_data *ion_alloc_data, ion_fd_data *fd_data, unsigned int size, unsigned char **src_va);
      alloc_ion (ionFD, &src_ion_alloc_data, &src_fd_data, in_buffer_size, &src_va);
#endif            
    }

    printf("go %d !!\n", __LINE__);  
    
    
    fp = fopen("/data/otis/src_blk_y.bin", "r");
    fread(src_va , 1 , in_buffer_size , fp);
    fclose(fp);
    
    printf("go %d !!\n", __LINE__);  
    fp = fopen("/data/otis/src_blk_c.bin", "r");
    fread(src_uv_va , 1 , in_buffer_uv_size , fp);
    fclose(fp);

 
    unsigned int HeaderSize = 0;//24;


    printf("go %d !!\n", __LINE__);  
    

    //unsigned int w = 1920;//jpgDec->getJpgWidth();
    //unsigned int h = 1088;//jpgDec->getJpgHeight();
    
    tileRowNum = h/16; //68;////jpgDec->getMcuRow();

    xlog("[test] file width/height: %d %d, tileRow %d!!\n", w, h, tileRowNum);


    
    
    dst_size    = w*h;              //jpgDec->getMinLumaBufSize()* tileRowNum ; 
    dst_size1   = TO_CEIL(w/2, 16)*TO_CEIL(h/2, 16);      //jpgDec->getMinCbCrBufSize()* tileRowNum ;
    
    dst_stride  = TO_CEIL(w,16);    //jpgDec->getMinLumaBufStride(); 
    dst_stride1 = TO_CEIL(w/2,16);     //jpgDec->getMinCbCrBufStride();    
    img_stride  = TO_CEIL(w,16);    //jpgDec->getLumaImgStride();
    img_stride1 = TO_CEIL(w/2,16);     //jpgDec->getCbCrImgStride();
    
    xlog("[test] min_size   : %x %x\n", dst_size, dst_size1);
    xlog("[test] min_stride : %d %d\n", dst_stride, dst_stride1);
    xlog("[test] min_img_st : %d %d\n", img_stride, img_stride1);
    if(test_type == MEM_TYPE_PMEM){    
      JPEG_PMEM_ALLOC(dst_size , dst_buffer,  dst_buffer_pa , dst0_fd);
      JPEG_PMEM_ALLOC(dst_size1, dst_buffer1, dst_buffer1_pa, dst1_fd);
      JPEG_PMEM_ALLOC(dst_size1, dst_buffer2, dst_buffer2_pa, dst2_fd);  

    }else if(test_type == MEM_TYPE_M4U){

 
      IMGDEC_ALLOC(alloc_dst_buffer , dst_size , 256, dst_buffer )
      IMGDEC_ALLOC(alloc_dst_buffer1, dst_size1, 256, dst_buffer1)
      IMGDEC_ALLOC(alloc_dst_buffer2, dst_size1, 256, dst_buffer2)    

    }else if (test_type == MEM_TYPE_ION){
#ifdef TEST_ION      
      alloc_ion (ionFD, &dst_ion_alloc_data , &dst_fd_data , dst_size , &dst_buffer );      
      alloc_ion (ionFD, &dst_ion_alloc_data1, &dst_fd_data1, dst_size1, &dst_buffer1);      
      alloc_ion (ionFD, &dst_ion_alloc_data2, &dst_fd_data2, dst_size1, &dst_buffer2);      
#endif      
    }
    
    xlog("[test] out_buffer : %x %x %x\n", (unsigned int)dst_buffer, (unsigned int)dst_buffer1, (unsigned int)dst_buffer2);
    
    //alloca M4U
    if(test_type == MEM_TYPE_PMEM){
      fFmtSrcAddrPA[0] = src_pa ;
      fFmtSrcAddrPA[1] = src_uv_pa ;
      fFmtDstAddrPA0[0] = dst_buffer_pa ;
      fFmtDstAddrPA0[1] = dst_buffer1_pa ;
      fFmtDstAddrPA0[2] = dst_buffer2_pa ;
    
    }else if(test_type == MEM_TYPE_M4U){
    
    
//        if(!imgMmu_create(&pM4uDrv, fm4uVDecID))
//          return false; 
        if(!imgMmu_create(&pM4uDrv, fm4uFmtID))
          return false;       
              
//        if(!imgMmu_reset(&pM4uDrv, fm4uVDecID))
//          return false;       
        if(!imgMmu_reset(&pM4uDrv, fm4uFmtID))
          return false;       
    
        //src bitstream
        if(!imgMmu_alloc_pa(&pM4uDrv, fm4uFmtID, (UINT32) src_va, in_buffer_size, &fFmtSrcAddrPA[0]))
          return false;    
        if(!imgMmu_alloc_pa(&pM4uDrv, fm4uFmtID, (UINT32) src_uv_va, in_buffer_uv_size, &fFmtSrcAddrPA[1]))
          return false;              
        
        //dst bank 0 Luma, Cb, Cr
        if(!imgMmu_alloc_pa(&pM4uDrv, fm4uFmtID, (UINT32) dst_buffer, dst_size, &fFmtDstAddrPA0[0]))
          return false;
        if(!imgMmu_alloc_pa(&pM4uDrv, fm4uFmtID, (UINT32) dst_buffer1, dst_size1, &fFmtDstAddrPA0[1]))
          return false;
        if(!imgMmu_alloc_pa(&pM4uDrv, fm4uFmtID, (UINT32) dst_buffer2, dst_size1, &fFmtDstAddrPA0[2]))
          return false;
    
//        if(fDecType == IMG_DEC_MODE_LINK){
//          
//          //dst bank 1 Luma, Cb, Cr
//          if(!imgMmu_alloc_pa(&pM4uDrv, fm4uFmtID, (UINT32) fDecOutBufAddr1[0], fDecDstBufSize[0], &fFmtDstAddrPA0[0]))
//            return false;
//          if(!imgMmu_alloc_pa(&pM4uDrv, fm4uFmtID, (UINT32) fDecOutBufAddr1[1], fDecDstBufSize[1], &fFmtDstAddrPA0[1]))
//            return false;
//          if(!imgMmu_alloc_pa(&pM4uDrv, fm4uFmtID, (UINT32) fDecOutBufAddr1[2], fDecDstBufSize[2], &fFmtDstAddrPA0[2]))
//            return false;      
//          
//        }
        
        
//        //interal working buffer
//        // VDEC output MVA
//        if(!imgMmu_alloc_pa(&pM4uDrv, fm4uVDecID, (UINT32)    fDecDstMallocYAddr, fDecBufSize[0], &fDecDstM4UAddrMva[0]))
//          return false;    
//        if(!imgMmu_alloc_pa(&pM4uDrv, fm4uVDecID, (UINT32) fDecDstMallocCbCrAddr, fDecBufSize[1], &fDecDstM4UAddrMva[1]))
//          return false;    
        
//        // FMT Source MVA        
//        if(!imgMmu_alloc_pa(&pM4uDrv, fm4uFmtID, (UINT32)    fDecDstMallocYAddr, fDecBufSize[0], &fFmtSrcM4UAddrMva[0]))
//          return false;
//        if(!imgMmu_alloc_pa(&pM4uDrv, fm4uFmtID, (UINT32) fDecDstMallocCbCrAddr, fDecBufSize[1], &fFmtSrcM4UAddrMva[1]))
//          return false;    
        
        
        
//        //config VDEC module port
//        imgMmu_cfg_port(pM4uDrv, fm4uVDecID, M4U_PORT_HW_VDEC_MC_EXT     );
//        imgMmu_cfg_port(pM4uDrv, fm4uVDecID, M4U_PORT_HW_VDEC_PP_EXT     );
//        imgMmu_cfg_port(pM4uDrv, fm4uVDecID, M4U_PORT_HW_VDEC_AVC_MV_EXT );
//        imgMmu_cfg_port(pM4uDrv, fm4uVDecID, M4U_PORT_HW_VDEC_PRED_RD_EXT);
//        imgMmu_cfg_port(pM4uDrv, fm4uVDecID, M4U_PORT_HW_VDEC_PRED_WR_EXT);
//        imgMmu_cfg_port(pM4uDrv, fm4uVDecID, M4U_PORT_HW_VDEC_VLD_EXT    );
//        imgMmu_cfg_port(pM4uDrv, fm4uVDecID, M4U_PORT_HW_VDEC_VLD2_EXT   );
        
        //config FMT read write port
        imgMmu_cfg_port(pM4uDrv, fm4uFmtID, M4U_PORT_GDMA_SMI_RD);
        imgMmu_cfg_port(pM4uDrv, fm4uFmtID, M4U_PORT_GDMA_SMI_WR);
        
        //flush cache
        imgMmu_sync(pM4uDrv, fm4uFmtID, (UINT32) src_va, in_buffer_size, SYNC_HW_READ);            
        imgMmu_sync(pM4uDrv, fm4uFmtID, (UINT32) src_uv_va, in_buffer_uv_size, SYNC_HW_READ);    
        imgMmu_sync(pM4uDrv, fm4uFmtID, (UINT32) dst_buffer , dst_size, SYNC_HW_WRITE);    
        imgMmu_sync(pM4uDrv, fm4uFmtID, (UINT32) dst_buffer1, dst_size1, SYNC_HW_WRITE);    
        imgMmu_sync(pM4uDrv, fm4uFmtID, (UINT32) dst_buffer2, dst_size1, SYNC_HW_WRITE);
        
//        if(fDecType == IMG_DEC_MODE_LINK){
//          imgMmu_sync(pM4uDrv, fm4uFmtID, (UINT32) fDecOutBufAddr1[0], fDecDstBufSize[0], SYNC_HW_WRITE);    
//          imgMmu_sync(pM4uDrv, fm4uFmtID, (UINT32) fDecOutBufAddr1[1], fDecDstBufSize[1], SYNC_HW_WRITE);    
//          imgMmu_sync(pM4uDrv, fm4uFmtID, (UINT32) fDecOutBufAddr1[2], fDecDstBufSize[2], SYNC_HW_WRITE);      
//        }
    
    
       
       //return true;
    }
    
    
#if 0
    jpgDec->setDstBufSize(dst_size, dst_size1) ;
    jpgDec->setDstAddr0((unsigned int)dst_buffer, (unsigned int) dst_buffer1, (unsigned int) dst_buffer2);
    if(test_type == MEM_TYPE_PMEM){    
      if(dst_buffer_pa && dst_buffer1_pa && dst_buffer2_pa)
        jpgDec->setDstAddrPA0((unsigned int)dst_buffer_pa, (unsigned int) dst_buffer1_pa, (unsigned int) dst_buffer2_pa);
    }
#endif    
   
    //config_gfmt( 0, 0 ) ;
{
   GFMT_HAL_CONFIG gfmt_data;
   GFMT_STATUS_ENUM ret ;
   unsigned int srcRowIdx = 0;
   unsigned int dstRowIdx = 0;
   unsigned int fMinLumaBufStride = w ;
   unsigned int fMinCbCrBufStride = TO_CEIL( (w/2), 16);
   unsigned int fFmtSrcBufStride[2];
   unsigned int fFmtDstBufStride[3];
   unsigned int fFmtMBrowHeight[2] ;
   unsigned int fFmtSrcMBrowSize[2];
   unsigned int fFmtDstMBrowSize[3] ;
   
   fFmtMBrowHeight[0] = 16 ;
   fFmtMBrowHeight[1] = 8 ;
   
     fFmtSrcBufStride[0] = 
     fFmtSrcBufStride[1] = fMinLumaBufStride;   
     
     fFmtDstBufStride[0] = fMinLumaBufStride;
     fFmtDstBufStride[1] = 
     fFmtDstBufStride[2] = fMinCbCrBufStride;
   
   
   fFmtSrcMBrowSize[0] = fFmtSrcBufStride[0]* fFmtMBrowHeight[0];   
   fFmtSrcMBrowSize[1] = fFmtSrcBufStride[1]* fFmtMBrowHeight[1];
   
   fFmtDstMBrowSize[0] = fFmtDstBufStride[0]* fFmtMBrowHeight[0];
   fFmtDstMBrowSize[1] = 
   fFmtDstMBrowSize[2] = fFmtDstBufStride[1]* fFmtMBrowHeight[1];
   
   
   memset(&gfmt_data, 0 , sizeof(GFMT_HAL_CONFIG) );

      
   gfmt_data.gfLinkGDMA        = 0;//(fDecType == IMG_DEC_MODE_LINK)? 1: 0;           // 0: 420 3p video format,  1: 420 3p mcu row
	 gfmt_data.webpEn            = 0;
   gfmt_data.fieldCompactOuput = 1;           // 0: frame out, 1: field compact 
   gfmt_data.isTopField        = 0;           // 0: bottom field, 1: top field

   gfmt_data.gfDstBufHeight[0] = 2 ;          //Y// 0: 4 lines 1: 8 lines 2: 16 lines 3: 32 lines
   gfmt_data.gfDstBufHeight[1] = 2 ;          //C// 0: 2 lines 1: 4 lines 2: 8 lines 3: 16 lines


   gfmt_data.webpMcuHeight_Y = 2;             // 0: 4 lines 1: 8 lines 2: 16 lines 3: 32 lines
   gfmt_data.webpMcuHeight_C = 2;             // 0: 2 lines 1: 4 lines 2: 8 lines 3: 16 lines
   gfmt_data.burst = 8;//2;//                       // 1,2,4,8 bytes 
   gfmt_data.gfSrcBufStride_Y  = fFmtSrcBufStride[0] ;//<< ((gfmt_data.fieldCompactOuput)? 1:0);          // 16-bytes align
   gfmt_data.gfSrcBufStride_C  = fFmtSrcBufStride[1] ;//<< ((gfmt_data.fieldCompactOuput)? 1:0);  
   gfmt_data.gfDstBufStride_Y  = fFmtDstBufStride[0] ;          // 16-bytes align
   gfmt_data.gfDstBufStride_C  = fFmtDstBufStride[1] ;    

   gfmt_data.gfSrcBufAddr_Y = fFmtSrcAddrPA[0] + srcRowIdx*fFmtSrcMBrowSize[0];    
   gfmt_data.gfSrcBufAddr_C = fFmtSrcAddrPA[1] + srcRowIdx*fFmtSrcMBrowSize[1];
  
  
  for( int i = 0; i<3 ;i++){
    gfmt_data.gfDstBufBank0[i] = fFmtDstAddrPA0[i] + dstRowIdx*fFmtDstMBrowSize[i];   
    //gfmt_data.gfDstBufBank1[i] = fFmtDstAddrPA1[i] + dstRowIdx*fFmtDstMBrowSize[i];
  }
   
   gfmt_data.gfSrcWidth     = w;//(fDecType == IMG_DEC_MODE_TILE )? fImgWidth     :      fImgWidth;     //yWidth; 
   gfmt_data.gfSrcHeight    = h;//(fDecType == IMG_DEC_MODE_TILE )? fFmtMBrowHeight[0] : fImgHeight;    //yHeight;

   gfmt_data.rangeMapEn     = 0;
   gfmt_data.rangeReduceEn  = 0;
   gfmt_data.rangeMapY      = 0;
   gfmt_data.rangeMapC      = 0;


   gfmt_data.go_flag        = 1;   
     

   xlog("CONFIG_FMT: src w/h %d %d, Addr %x %x, go(%d)!!\n", gfmt_data.gfSrcWidth, gfmt_data.gfSrcHeight,gfmt_data.gfSrcBufAddr_Y, gfmt_data.gfSrcBufAddr_C,gfmt_data.go_flag );
   xlog("CONFIG_FMT: Addr dst0 %x %x %x, dst1 %x %x %x!!\n", 
   gfmt_data.gfDstBufBank0[0], gfmt_data.gfDstBufBank0[1], gfmt_data.gfDstBufBank0[2],
   gfmt_data.gfDstBufBank1[0], gfmt_data.gfDstBufBank1[1], gfmt_data.gfDstBufBank1[2] );
   xlog("CONFIG_FMT: Stride src %d %d, dst %d %d %d!!\n", 
   gfmt_data.gfSrcBufStride_Y, gfmt_data.gfSrcBufStride_C, gfmt_data.gfDstBufStride_Y, gfmt_data.gfDstBufStride_C, gfmt_data.gfDstBufStride_C );
   
   
   ret = gfmtConfig(gfmtID, &gfmt_data) ;
   
   
   if(ret!= GFMT_STATUS_OK){
     xlog("FAIL CONFIG FMT ret %d!!\n",ret);
     //return false;//fail_return("config FMT fail!!\n");   
   }


   //return true ;
}    


    if(GFMT_STATUS_OK == gfmtWaitIRQ(gfmtID, timeout_msec, &fmt_rst))
    {
      //for(int i =1; i<tileRowNum ;i++)
      //  jpgDec->ResumeDecRow(1);
       printf("go %d, rst %d !!\n", __LINE__, fmt_rst);  
            
    }
    else
    {
        xlog("Wait FMT IRQ failed~~~\n"); 
    }
    
for(int ii =0;ii<2;ii++)
{ 
    FILE *fp;
    if(ii==1)
      fp = fopen("/data/otis/ttt.y.raw", "w");
    else
      fp = fopen("/data/otis/ttt.raw", "w");
    unsigned int decDstBufSize[3] ;
    unsigned char* targetAddr[3];
    unsigned char* cptr ;        

    targetAddr[0] = dst_buffer ;
    targetAddr[1] = dst_buffer1 ;
    targetAddr[2] = dst_buffer2 ;
    decDstBufSize[0] = dst_size;
    decDstBufSize[1]  = decDstBufSize[2] = dst_size1;
    for( int j=0;j<3;j++){ /* runcomp 0~2 */
      cptr = targetAddr[j] ;
      for( int i=0;i<decDstBufSize[j];i++){  /* total size in comp */
        fprintf(fp,"%c", *cptr );  
        cptr++;
      }

#if 1
     
     if(ii==1){            
            fclose(fp);
            if( j==0 )
               fp = fopen("/data/otis/ttt.cb.raw", "w"); 
            else if(j == 1)
               fp = fopen("/data/otis/ttt.cr.raw", "w"); 
     }      
#endif   
      
    }                
    fclose(fp);
}
#if 0
{
    FILE *fp;   
    unsigned char* cptr ;        

    fp = fopen("/data/otis/convARGB.raw", "w");
    
    CONV_YUV2RGB_DATA convYR ;


    unsigned char* raw_buffer = (unsigned char *)malloc(768*480*4);
    memset(raw_buffer,0,sizeof(768*480*4));
    
    convYR.srcWidth = w;
    convYR.srcHeight = h;
    convYR.srcMemStride[0] = dst_stride;
    convYR.srcMemStride[1] = dst_stride1;
    convYR.srcMemStride[2] = dst_stride1;
    
    convYR.srcAddr[0] = dst_buffer;
    convYR.srcAddr[1] = dst_buffer1;
    convYR.srcAddr[2] = dst_buffer2;
    convYR.dst_buf = (unsigned char*)raw_buffer;
    
    convYR.srcFormat = 1;  //0: YUV420, 1: YUV422
    convYR.dstFormat = 0;  //0: ARGB8888, 1:RGB888, 2:RGB565
    convYR.pixelSize = 4;
    
    convYUV2RGB(&convYR);
    

      cptr = raw_buffer ;
      for( int i=0;i<768*480*4;i++){  /* total size in comp */
        fprintf(fp,"%c", *cptr );  
        cptr++;
      }    
      fclose(fp);     

}
#endif


    printf("go %d !!\n", __LINE__);  
    

    
    gfmtUnlock(gfmtID);       //jpgDec->unlock();
    printf("go %d !!\n", __LINE__);  
    
    //delete jpgDec;
    printf("go %d !!\n", __LINE__);  

    if(test_type == MEM_TYPE_M4U){
    
       
    
       if(pM4uDrv)
       {
          imgMmu_dealloc_pa(pM4uDrv, fm4uFmtID, (UINT32) src_va           , in_buffer_size   , &fFmtSrcAddrPA[0]       );
          imgMmu_dealloc_pa(pM4uDrv, fm4uFmtID, (UINT32) src_uv_va           , in_buffer_uv_size   , &fFmtSrcAddrPA[1]       );
          imgMmu_dealloc_pa(pM4uDrv, fm4uFmtID, (UINT32) dst_buffer  , dst_size,  &fFmtDstAddrPA0[0]   );
          imgMmu_dealloc_pa(pM4uDrv, fm4uFmtID, (UINT32) dst_buffer1 , dst_size1, &fFmtDstAddrPA0[1]   );
          imgMmu_dealloc_pa(pM4uDrv, fm4uFmtID, (UINT32) dst_buffer2 , dst_size1, &fFmtDstAddrPA0[2]   );
          //if( fDecType == IMG_DEC_MODE_LINK ){
          //  imgMmu_dealloc_pa(pM4uDrv, fm4uFmtID, (UINT32) fDecOutBufAddr1[0] , fDecDstBufSize[0], &fFmtDstM4UAddrMva1[0]   );
          //  imgMmu_dealloc_pa(pM4uDrv, fm4uFmtID, (UINT32) fDecOutBufAddr1[1] , fDecDstBufSize[1], &fFmtDstM4UAddrMva1[1]   );
          //  imgMmu_dealloc_pa(pM4uDrv, fm4uFmtID, (UINT32) fDecOutBufAddr1[2] , fDecDstBufSize[2], &fFmtDstM4UAddrMva1[2]   );
          //}
          // free working buffer mva
          //imgMmu_dealloc_pa(pM4uDrv, fm4uVDecID, (UINT32)    fDecDstMallocYAddr , fDecBufSize[0] , &fDecDstM4UAddrMva[0]   );
          //imgMmu_dealloc_pa(pM4uDrv, fm4uVDecID, (UINT32) fDecDstMallocCbCrAddr , fDecBufSize[1] , &fDecDstM4UAddrMva[1]   );
          //imgMmu_dealloc_pa(pM4uDrv,  fm4uFmtID, (UINT32)    fDecDstMallocYAddr , fDecBufSize[0] , &fFmtSrcM4UAddrMva[0]   );
          //imgMmu_dealloc_pa(pM4uDrv,  fm4uFmtID, (UINT32) fDecDstMallocCbCrAddr , fDecBufSize[1] , &fFmtSrcM4UAddrMva[1]   );
          
          delete pM4uDrv;
       }   
       pM4uDrv = NULL;
       
     
       
       //return true; 
    } 



    if(test_type == MEM_TYPE_PMEM){
      JPEG_PMEM_FREE(src_va, in_buffer_size, src_fd);
      JPEG_PMEM_FREE(src_uv_va, in_buffer_uv_size, src_uv_fd);
      
      JPEG_PMEM_FREE(dst_buffer , dst_size , dst0_fd);
      JPEG_PMEM_FREE(dst_buffer1, dst_size1, dst1_fd);
      JPEG_PMEM_FREE(dst_buffer2, dst_size1, dst2_fd);
    }else if(test_type == MEM_TYPE_ION){
#ifdef TEST_ION      
      free_ion (ionFD, &src_ion_alloc_data , &src_fd_data, in_buffer_size, &src_va);
      free_ion (ionFD, &dst_ion_alloc_data , &dst_fd_data , dst_size , &dst_buffer );      
      free_ion (ionFD, &dst_ion_alloc_data1, &dst_fd_data1, dst_size1, &dst_buffer1);      
      free_ion (ionFD, &dst_ion_alloc_data2, &dst_fd_data2, dst_size1, &dst_buffer2);  
      if(ionFD > 0)close(ionFD);       
#endif         
    }
 
    
    return 0;
}

unsigned char jpg_file[739]=
{0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46, 0x49, 0x46, 0x00, 0x01, 
 0x01, 0x01, 0x00, 0x60, 0x00, 0x60, 0x00, 0x00, 0xFF, 0xDB, 0x00, 0x43, 
 0x00, 0x08, 0x06, 0x06, 0x07, 0x06, 0x05, 0x08, 0x07, 0x07, 0x07, 0x09, 
 0x09, 0x08, 0x0A, 0x0C, 0x14, 0x0D, 0x0C, 0x0B, 0x0B, 0x0C, 0x19, 0x12, 
 0x13, 0x0F, 0x14, 0x1D, 0x1A, 0x1F, 0x1E, 0x1D, 0x1A, 0x1C, 0x1C, 0x20, 
 0x24, 0x2E, 0x27, 0x20, 0x22, 0x2C, 0x23, 0x1C, 0x1C, 0x28, 0x37, 0x29, 
 0x2C, 0x30, 0x31, 0x34, 0x34, 0x34, 0x1F, 0x27, 0x39, 0x3D, 0x38, 0x32, 
 0x3C, 0x2E, 0x33, 0x34, 0x32, 0xFF, 0xDB, 0x00, 0x43, 0x01, 0x09, 0x09, 
 0x09, 0x0C, 0x0B, 0x0C, 0x18, 0x0D, 0x0D, 0x18, 0x32, 0x21, 0x1C, 0x21, 
 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 
 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 
 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 
 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 
 0x32, 0x32, 0xFF, 0xC0, 0x00, 0x11, 0x08, 0x00, 0x10, 0x00, 0x0C, 0x03, 
 0x01, 0x22, 0x00, 0x02, 0x11, 0x01, 0x03, 0x11, 0x01, 0xFF, 0xC4, 0x00, 
 0x1F, 0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 
 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0xC4, 0x00, 0xB5, 0x10, 0x00, 
 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 
 0x00, 0x01, 0x7D, 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 
 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 
 0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 0xD1, 0xF0, 0x24, 
 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x25, 
 0x26, 0x27, 0x28, 0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 
 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 
 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 
 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x83, 0x84, 0x85, 0x86, 
 0x87, 0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 
 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 
 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 
 0xDA, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 
 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFF, 0xC4, 0x00, 
 0x1F, 0x01, 0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 
 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0xC4, 0x00, 0xB5, 0x11, 0x00, 
 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 
 0x01, 0x02, 0x77, 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 
 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22, 0x32, 0x81, 0x08, 
 0x14, 0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0, 0x15, 
 0x62, 0x72, 0xD1, 0x0A, 0x16, 0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 
 0x19, 0x1A, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x35, 0x36, 0x37, 0x38, 0x39, 
 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 
 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 
 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84, 
 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 
 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 
 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 
 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 
 0xD8, 0xD9, 0xDA, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 
 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFF, 0xDA, 0x00, 
 0x0C, 0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00, 0x3F, 0x00, 0xB3, 
 0x7F, 0xE2, 0x07, 0xF0, 0xEC, 0x76, 0x10, 0xAD, 0xBC, 0xD2, 0xFF, 0x00, 
 0xA2, 0xA3, 0x31, 0x8A, 0x4D, 0xA5, 0x42, 0xAA, 0x8E, 0x87, 0xAF, 0x5A, 
 0xD4, 0xBB, 0xD4, 0xCB, 0x8B, 0x79, 0x65, 0x25, 0x9A, 0x48, 0x83, 0x02, 
 0xE0, 0x67, 0x19, 0x35, 0x93, 0xE2, 0xBF, 0x0F, 0xDE, 0xEA, 0x17, 0xA1, 
 0xAD, 0x65, 0x81, 0x02, 0x44, 0x61, 0x29, 0x30, 0x23, 0x3E, 0xE0, 0xD5, 
 0x4F, 0x13, 0x5C, 0xFD, 0x96, 0xEE, 0xD2, 0x20, 0x7A, 0x5B, 0x0E, 0x9F, 
 0xEF, 0xB5, 0x78, 0x39, 0x8E, 0x17, 0x2F, 0x9E, 0x55, 0x84, 0x9D, 0x05, 
 0x1F, 0x6A, 0xFE, 0x26, 0x9E, 0xBB, 0x5F, 0x51, 0xF0, 0xBC, 0x30, 0xF8, 
 0xAC, 0xE9, 0xFB, 0x29, 0xB7, 0x37, 0xED, 0x5C, 0xD5, 0xDD, 0x97, 0xBE, 
 0xB9, 0x6C, 0xB6, 0xD9, 0x9F, 0xFF, 0xD9};
