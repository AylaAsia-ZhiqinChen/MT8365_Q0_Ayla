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
#include <stdlib.h>
#include <cutils/log.h>
#include <string.h>
#include <mhal_jpeg.h>
#include "MediaHal.h"
//#include "jpeg_hal_dec.h"
#define JPEG_TESET_CNT 1
#define printf(...) \
        do { LOGD(__VA_ARGS__); \
        } while (0)
 
#undef LOG_TAG
#define LOG_TAG "jpeg_test"
#define JPEG_ROUND_N(X, N)   (((X) + (N-1)) & (~(N-1)))    //only for N is exponential of 2
//#define BUFFER_SIZE 739
#define BUFFER_SIZE (1920*1080*4)
#define BUFFER_SIZE (1920*1080*4)
#undef printf
#define printf(...) ALOGE(__VA_ARGS__)


//#define TO_CEIL(x,a) ( ((unsigned long)(x) + ((a)-1)) & ~((a)-1) )

#define IMGDEC_ALLOC(ptr, size, align, align_ptr) \
{  \
   ptr = (unsigned char *)memalign(64,size+align) ; \
   align_ptr = ptr;\
   memset(ptr, 0x0 , size); \
}

#define IMGDEC_FREE(ptr) \
{  \
   free(ptr);\
}

int64_t getTickCountMs()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)(tv.tv_sec*1000LL + tv.tv_usec/1000);
}
void* decodeJpeg_0(void* arg);
void* decodeJpeg_1(void* arg);
void* decodeJpeg_2(void* arg);
int encode_test();
pthread_t mJpegDecThread0,mJpegDecThread1,mJpegDecThread2;
const char *fileName = NULL;
char filebuf[255];
int test_cnt =1;
int status0 =0,status1=0,status2 =0;
int thread_num =0;// nice(10);
unsigned char *src_va0 = NULL;
unsigned char *src_va_org0 = NULL;
unsigned char *dst_buffer_org0 = NULL; 
unsigned char *dst_buffer0 = NULL; 
unsigned char *src_va1 = NULL;
unsigned char *src_va_org1 = NULL;
unsigned char *dst_buffer_org1 = NULL; 
unsigned char *dst_buffer1 = NULL;     
unsigned char *src_va2 = NULL;
unsigned char *src_va_org2 = NULL;
unsigned char *dst_buffer_org2 = NULL; 
unsigned char *dst_buffer2 = NULL;     
unsigned char *dst_buffer_org_g = NULL; 
unsigned char *dst_buffer_g = NULL;     
unsigned int _compare_dst = 0;

unsigned int _dst_w = 0;
unsigned int _dst_h = 0;
unsigned int _dstsize = 0;
int cMemCmp(char *ptsrc, char *ptdst, int size);
void* decodeJpeg_g();
int main(int i4Argc, const char* aszArgv[])
{
	int ret;
    FILE *fp;
    bool fgTestEnc =false;
    int64_t _t_time_1 = 0;
    int64_t _t_time_2 = 0;
    
	status0 =0;
	status1 =0;
	status2 =0;
	thread_num =0;
	src_va0 = NULL,	src_va_org0 = NULL;
	src_va1 = NULL,	src_va_org1 = NULL;
	dst_buffer_org0 = NULL,	dst_buffer0 = NULL; 
	dst_buffer_org1 = NULL,	dst_buffer1 = NULL;     

    thread_num =1;
     
    
    _compare_dst =0;
    printf("[TJPEG]test_cnt %d !!\n", test_cnt);     
	if(i4Argc >= 1)
   	{
   		printf("[TJPEG]aszArgv[1] %s !!\n", aszArgv[1]);  
   		//test_cnt = //atoi(aszArgv[1]);
   		if(aszArgv[1][0] == 'e')
   		{
   			printf("[TJPEG] test encoder !!\n");  
   			fgTestEnc = true;
   		}
        		
   	}
   	
   	if(i4Argc >= 2)
   	{
   	    fileName = aszArgv[2];
        printf("[TJPEG]test_file %s !!\n", fileName);   
        sprintf (filebuf, "/data/jpegtest/%s", fileName);  		
   	}
   	 if(i4Argc >= 3)
   	 {
   	    thread_num = 1;//atoi(aszArgv[3]);
        printf("[TJPEG]thread_num %d !!\n", thread_num);   
     }
#if 0
 	if(i4Argc >= 4)
   	 {
   	    _compare_dst =atoi(aszArgv[4]);
        printf("[TJPEG]_compare_dst %d !!\n", _compare_dst);   
     }
#endif
    if(fgTestEnc)
    {
    	encode_test();
    	return 0;
    }
 	//for thread 0
 	IMGDEC_ALLOC(src_va_org0, BUFFER_SIZE, 64, src_va0) ;
    if(src_va_org0 == NULL)
    {
        printf("[TJPEG]]Can not allocate src0 memory\n");
         return 0;
    }
    printf("[TJPEG]]open %s!!\n",filebuf);   		
    fp = fopen(filebuf, "r");
    if(fp== NULL)
    {
       printf("[TJPEG]open %s failed!!\n",filebuf); 
       return 0; 
    }
    fread(src_va0 , 1 , BUFFER_SIZE , fp);
    fclose(fp);   
  	IMGDEC_ALLOC(dst_buffer_org0, BUFFER_SIZE, 64, dst_buffer0) ;  
    if(dst_buffer_org0 == NULL)
    {
        printf("[TJPEG]Can not allocate dst0  memory\n");
        return 0;
    }  	 
    if(thread_num>1)
    {
	    //for thread 1	
	    IMGDEC_ALLOC(src_va_org1, BUFFER_SIZE, 64, src_va1) ;
	    if(src_va_org1 == NULL)
	    {
	        printf("[TJPEG]Can not allocate src1 memory\n");
	        return 0;
	    }	
	    IMGDEC_ALLOC(dst_buffer_org1, BUFFER_SIZE, 64, dst_buffer1) ;  
	    if(dst_buffer_org1 == NULL)
	    {
	        printf("[TJPEG]Can not allocate dst0  memory\n");
	        return 0;
	    } 
	     memcpy(src_va1,src_va0,BUFFER_SIZE);
    }
    //for thread 2	
    if(thread_num>2)
    { 	
	    IMGDEC_ALLOC(src_va_org2, BUFFER_SIZE, 64, src_va2) ;
	    if(src_va_org2 == NULL)
	    {
	        printf("[TJPEG]Can not allocate src1 memory\n");
	        return 0;
	    }	
	    IMGDEC_ALLOC(dst_buffer_org2, BUFFER_SIZE, 64, dst_buffer2) ;  
	    if(dst_buffer_org2 == NULL)
	    {
	        printf("[TJPEG]Can not allocate dst0  memory\n");
	        return 0;
	    } 
	     memcpy(src_va2,src_va0,BUFFER_SIZE);
    }
    _t_time_1 = getTickCountMs();
	
	if(_compare_dst)
	{
       IMGDEC_ALLOC(dst_buffer_org_g, BUFFER_SIZE, 64, dst_buffer_g) ;  
	    if(dst_buffer_org_g == NULL)
	    {
	        printf("[TJPEG]Can not allocate dst_buffer_org_g  memory\n");
	        return 0;
	    } 	
	     printf("[TJPEG] dst_buffer_g [0x%x]\n", dst_buffer_g);
	    decodeJpeg_g();
	}
 		  
  	ret = pthread_create (&mJpegDecThread0, NULL, &decodeJpeg_0, NULL);
  	if (ret)
    {
        printf ("[TJPEG]mJpegDecThread0 creation failure");
    }
    if(thread_num>1)
    {
	    ret = pthread_create (&mJpegDecThread1, NULL, &decodeJpeg_1, NULL);
	  	if (ret)
	    {
	        printf ("[TJPEG]mJpegDecThread1 creation failure");
	    }
    }
    else
    {
    	status1=3;
    }
    
    if(thread_num>2)
    {
	    ret = pthread_create (&mJpegDecThread2, NULL, &decodeJpeg_2, NULL);
	  	if (ret)
	    {
	        printf ("[TJPEG]mJpegDecThread2 creation failure");
	    }
    }
    else
    {
    	status2=3;
    }
    while(3!=status0 || status1!=3 || status2!=3)
    {
    	usleep(20);
    }
     _t_time_2 = getTickCountMs();
     printf("[TJPEG]decode Time TAVA YV12 [%lld]ms\n", (_t_time_2-_t_time_1)/(thread_num*test_cnt));     
     	   
//    FILE *fp;
    char buf0[255];
    sprintf (buf0, "/data/jpegtest/t0_out_%ld_%ld.yuv", _dst_w, _dst_h);
	printf("[TJPEG] test_piep_dump : %s!!\n",buf0);      
    fp = fopen(buf0, "wb");
    if(fp== NULL)
	{
		 printf("[TJPEG]open dst file failed!!\n");  
	}
    fwrite((void *)dst_buffer0, 1,_dstsize, fp); 
    fclose(fp);  
    if(thread_num>1)
    {
	    char buf1[255];
	    sprintf (buf1, "/data/jpegtest/t1_out_%ld_%ld.yuv", _dst_w, _dst_h);
		printf("[TJPEG] test_piep_dump : %s!!\n",buf1);      
	    fp = fopen(buf1, "wb");
	    if(fp== NULL)
		{
			 printf("[TJPEG]open dst file failed!!\n");  
		}
	    fwrite((void *)dst_buffer1, 1,_dstsize, fp); 
	    fclose(fp);  
	}
	if(thread_num>2)
    {
	    char buf2[255];
	    sprintf (buf2, "/data/jpegtest/t2_out_%ld_%ld.yuv", _dst_w, _dst_h);
		printf("[TJPEG] test_piep_dump : %s!!\n",buf2);      
	    fp = fopen(buf2, "wb");
	    if(fp== NULL)
		{
			 printf("[TJPEG]open dst file failed!!\n");  
		}
	    fwrite((void *)dst_buffer2, 1,_dstsize, fp); 
	    fclose(fp);  
	}
	IMGDEC_FREE(src_va_org0);
	IMGDEC_FREE(dst_buffer_org0);
	IMGDEC_FREE(src_va_org1);
	IMGDEC_FREE(dst_buffer_org1);    
}
void* decodeJpeg_0(void* arg)
{

	status0 =0;
    unsigned int dst_size;//, dst_size1 ;  
    MHAL_JPEG_DEC_SRC_IN srcParams = {0};
    MHAL_JPEG_DEC_START_IN inParams ;
    MHAL_JPEG_DEC_INFO_OUT decOutInfo ;
    void* procHandler = NULL;
  
    int64_t in_time_1 = 0;
    int64_t in_time_2 = 0;
    //int64_t _avtime_1 = 0;
    int64_t avtime_2 = 0;
    int cnt =0;
 	unsigned int w =0;
	unsigned int h = 0;
	unsigned int dst_w = 0;
	unsigned int dst_h = 0;
	int result =0;
    JPEG_OUT_FORMAT_ENUM dstFormat = JPEG_OUT_FORMAT_ARGB8888;
    printf("[TJPEG-%d]Enter ======decodeJpeg thread 0 !!\n",gettid()); 
    if(dstFormat ==  JPEG_OUT_FORMAT_ARGB8888)
    {
       printf("[TJPEG-%d]format  JPEG_OUT_FORMAT_ARGB8888, cnt[%d]---\n",gettid(),test_cnt);  
    }
    else if (dstFormat ==  JPEG_OUT_FORMAT_YV12)	
    {
      printf("[TJPEG-%d]format  JPEG_OUT_FORMAT_YV12, cnt[%d]---\n", gettid(),test_cnt);  
    }
    for(cnt =0; cnt<test_cnt; cnt++)
    {

	    memset(&srcParams  , 0, sizeof(MHAL_JPEG_DEC_SRC_IN));
	    memset(&inParams  , 0, sizeof(MHAL_JPEG_DEC_START_IN));
	    memset(&decOutInfo, 0, sizeof(MHAL_JPEG_DEC_INFO_OUT));
	    	
	    //printf("[TJPEG-%d]out_buffer : %x \n", (unsigned int)dst_buffer);
	    in_time_1 = getTickCountMs();
	    srcParams.srcBuffer = src_va0;
        srcParams.srcLength = BUFFER_SIZE;
        srcParams.srcFD = 0;
	    //mi_mHalJpgDecParser(&srcParams,src_va, BUFFER_SIZE, 0, MHAL_IMAGE_TYPE_JPEG);
	     mHalJpeg(MHAL_IOCTL_MJPEG_DEC_PARSER, (void*) &srcParams, sizeof(srcParams), NULL, 0, NULL);
	    if (result != MHAL_NO_ERROR)
        {
            printf("[TJPEG-%d] eMJpegParse failed:0x%X!",gettid(), result);
            //assert(0);
        }
         decOutInfo.jpgDecHandle = srcParams.jpgDecHandle;
	    //mi_mHalJpgDecGetInfo(&decOutInfo);
	    result = mHalJpeg(MHAL_IOCTL_MJPEG_DEC_GET_INFO, NULL, 0, (void *)&decOutInfo, sizeof(decOutInfo), NULL);
        if (result != MHAL_NO_ERROR)
        {
            printf("[TJPEG-%d] eMJpegParse get info failed:0x%X!",gettid(), result);
            //assert(0);
        }
        w = decOutInfo.srcWidth;
	    h = decOutInfo.srcHeight;
	    dst_w = ((decOutInfo.srcWidth + 15)/16) * 16;//501;//TO_CEIL(w, 4);      //w;//200;//
	    dst_h = ((decOutInfo.srcHeight + 15)/16) *16;//501;//TO_CEIL(h, 4);    //h;//200;//
	    _dst_w =dst_w;
	   	_dst_h =dst_h;
	   	dst_size    =JPEG_ROUND_N(dst_w, 32)*JPEG_ROUND_N(dst_h, 32)*4;
	    _dstsize =dst_size;
	   
	    inParams.dstFormat = dstFormat;
		inParams.jpgDecHandle = srcParams.jpgDecHandle;
	    inParams.srcBuffer = src_va0;
	    inParams.srcLength= BUFFER_SIZE;
	    inParams.srcBufSize = BUFFER_SIZE;
	    inParams.srcFD = 0;//srcFD;	    
	    inParams.dstWidth = dst_w;
	    inParams.dstHeight = dst_h;
	    inParams.dstVirAddr = (UINT8*) dst_buffer0;
	    inParams.dstPhysAddr = NULL;
	
	    inParams.doDithering = 0;//getDitherImage() ? 1 : 0;
	    inParams.doRangeDecode = 0;    
	    
	    
	    printf("[TJPEG-%d] file width/height: %d %d->%d %d!!\n",gettid(), w, h, dst_w, dst_h); 	
	    result = mHalJpeg(MHAL_IOCTL_MJPEG_DEC_START, (void*)&inParams, sizeof(inParams), NULL, 0, NULL);
        if (result == MHAL_NO_ERROR)
        {
	         in_time_2 = getTickCountMs();
	         printf("[TJPEG-%d]decode Time cnt[%d] YV12 [%lld]ms\n",gettid(), cnt,in_time_2 -in_time_1);   
	        //if(cnt>=1)
	         avtime_2 += (in_time_2 -in_time_1);    
	         if(_compare_dst)
	         {  
		         if(cMemCmp((char *)dst_buffer0, (char *)dst_buffer_g, dst_size))
		         {
		         	printf("[TJPEG-%d]compare failed,cnt[%d]~~\n",gettid(),cnt);
		         	char buf0[255];
		         	FILE *fp = NULL;
				    sprintf (buf0, "/data/jpegtest/t0_out_%ld_%ld.yuv.error_%ld", _dst_w, _dst_h,cnt);
					printf("[TJPEG-%d] test_piep_dump : %s!!\n",gettid(),buf0);      
				    fp = fopen(buf0, "wb");
				    if(fp== NULL)
					{
						 printf("[TJPEG-%d]open dst file failed!!\n",gettid());  
					}
				    fwrite((void *)dst_buffer0, 1,BUFFER_SIZE, fp); 
				    fclose(fp);  
		         }
		         else
		         {
		           printf("[TJPEG-%d]compare sucess,cnt[%d]~~\n",gettid(),cnt);
		         }
	         }
	    }
	    else
	    { 
	        printf("[TJPEG-%d]decode failed~~~\n",gettid());
	    }
	    
     }
	//printf("[TJPEG-%d]decode Time AVA-RGB [%lld]ms\n", _avtime_1/(test_cnt-1));   
	printf("[TJPEG-%d]decode Time EXIT AVA-YV12 [%lld]ms\n",gettid(), avtime_2/test_cnt);   
    status0 =3;
    return 0;
}
void* decodeJpeg_1(void* arg)
{
    FILE *fp;
	status1 =0;
 
    unsigned int dst_size;//, dst_size1 ;  
    //unsigned char *dst_buffer_org = NULL; 
    //unsigned char *dst_buffer = NULL; 
    MHAL_JPEG_DEC_SRC_IN srcParams = {0};
    MHAL_JPEG_DEC_START_IN inParams ;
    MHAL_JPEG_DEC_INFO_OUT decOutInfo ;
    void* procHandler = NULL;
  
    int64_t in_time_1 = 0;
    int64_t in_time_2 = 0;
    //int64_t _avtime_1 = 0;
    int64_t avtime_2 = 0;
    int cnt =0;
    unsigned int w =0;
	unsigned int h = 0;
	unsigned int dst_w = 0;
	unsigned int dst_h = 0;
	int result =0;
    
    printf("[TJPEG-%d]Enter ======decodeJpeg  thread 1  !!\n",gettid());  
    printf("[TJPEG-%d]format  JPEG_OUT_FORMAT_YV12, cnt[%d]---\n",gettid(),test_cnt);	   
  	for(cnt =0; cnt<test_cnt; cnt++)
    {

	    memset(&srcParams  , 0, sizeof(MHAL_JPEG_DEC_SRC_IN));
	    memset(&inParams  , 0, sizeof(MHAL_JPEG_DEC_START_IN));
	    memset(&decOutInfo, 0, sizeof(MHAL_JPEG_DEC_INFO_OUT));
	    //printf("[[TJPEG-%d]]out_buffer : %x \n", (unsigned int)dst_buffer);
	    in_time_1 = getTickCountMs();
	    srcParams.srcBuffer = src_va1;
        srcParams.srcLength = BUFFER_SIZE;
        srcParams.srcFD = 0;
	    //mi_mHalJpgDecParser(&srcParams,src_va, BUFFER_SIZE, 0, MHAL_IMAGE_TYPE_JPEG);
	     mHalJpeg(MHAL_IOCTL_MJPEG_DEC_PARSER, (void*) &srcParams, sizeof(srcParams), NULL, 0, NULL);
        if (result != MHAL_NO_ERROR)
        {
            printf("[TJPEG-%d] eMJpegParse failed:0x%X!",gettid(), result);
            //assert(0);
        }
         decOutInfo.jpgDecHandle = srcParams.jpgDecHandle;
	    //mi_mHalJpgDecGetInfo(&decOutInfo);
	    result = mHalJpeg(MHAL_IOCTL_MJPEG_DEC_GET_INFO, NULL, 0, (void *)&decOutInfo, sizeof(decOutInfo), NULL);
        if (result != MHAL_NO_ERROR)
        {
            printf("[TJPEG-%d] eMJpegParse get info failed:0x%X!",gettid(), result);
        }
        w = decOutInfo.srcWidth;
	    h = decOutInfo.srcHeight;
	    dst_w = ((decOutInfo.srcWidth + 15)/16) * 16;//501;//TO_CEIL(w, 4);      //w;//200;//
	    dst_h = ((decOutInfo.srcHeight + 15)/16) *16;//501;//TO_CEIL(h, 4);    //h;//200;//
	    dst_size    =JPEG_ROUND_N(dst_w, 32)*JPEG_ROUND_N(dst_h, 32)*3 >> 1;
	    _dstsize =dst_size;	   
	   	_dst_w =dst_w;
	   	_dst_h =dst_h;
	    inParams.dstFormat = JPEG_OUT_FORMAT_YV12;
		inParams.jpgDecHandle = srcParams.jpgDecHandle;
	    inParams.srcBuffer = src_va1;
	    inParams.srcLength= BUFFER_SIZE;
	    inParams.srcBufSize = BUFFER_SIZE;
	    inParams.srcFD = 0;//srcFD;	    
	    inParams.dstWidth = dst_w;
	    inParams.dstHeight = dst_h;
	    inParams.dstVirAddr = (UINT8*) dst_buffer1;
	    inParams.dstPhysAddr = NULL;
	
	    inParams.doDithering = 0;//getDitherImage() ? 1 : 0;
	    inParams.doRangeDecode = 0;    
	    
	    
	    printf("[TJPEG-%d] file width/height: %d %d->%d %d!!\n",gettid(), w, h, dst_w, dst_h); 	
	    result = mHalJpeg(MHAL_IOCTL_MJPEG_DEC_START, (void*)&inParams, sizeof(inParams), NULL, 0, NULL);
        if (result == MHAL_NO_ERROR)
        {
	        in_time_2 = getTickCountMs();
	         printf("[TJPEG-%d]decode Time cnt[%d] YV12 [%lld]ms\n",gettid(), cnt,in_time_2 -in_time_1);    
	        avtime_2 += (in_time_2 -in_time_1); 	  
	        if(_compare_dst)
	        {  
		        if(cMemCmp((char *)dst_buffer1, (char *)dst_buffer_g, dst_size))
		        {
		         	printf("[TJPEG-%d]compare failed,cnt[%d]~~\n",gettid(),cnt);
		         	char buf0[255];FILE *fp = NULL;
				    sprintf (buf0, "/data/jpegtest/t1_out_%ld_%ld.yuv.error_%ld", _dst_w, _dst_h,cnt);
					printf("[TJPEG-%d] test_piep_dump : %s!!\n",gettid(),buf0);      
				    fp = fopen(buf0, "wb");
				    if(fp== NULL)
					{
						 printf("[TJPEG-%d]open dst file failed!!\n",gettid());  
					}
				    fwrite((void *)dst_buffer1, 1,BUFFER_SIZE, fp); 
				    fclose(fp);  
		         }
		         else
		         {
		           printf("[TJPEG-%d]compare sucess,cnt[%d]~~\n",gettid(),cnt);
		         }
	         }  	
	    }
	    else
	    { 
	        printf("[TJPEG-%d]decode failed~~~\n",gettid());
	    }
     }
	printf("[TJPEG-%d]decode Time EXIT AVA-YV12 [%lld]ms\n",gettid(), avtime_2/test_cnt);   
	status1 =3;
    return 0;
}

void* decodeJpeg_2(void* arg)
{
    FILE *fp;
	status2 =0;
    unsigned int dst_size;//, dst_size1 ;  
    unsigned char *dst_buffer_org = NULL; 
    unsigned char *dst_buffer = NULL; 
    MHAL_JPEG_DEC_SRC_IN srcParams = {0};
    MHAL_JPEG_DEC_START_IN inParams ;
    MHAL_JPEG_DEC_INFO_OUT decOutInfo ;
    void* procHandler = NULL;
  
    int64_t in_time_1 = 0;
    int64_t in_time_2 = 0;
    //int64_t _avtime_1 = 0;
    int64_t avtime_2 = 0;
    int cnt =0;
    unsigned int w =0;
	unsigned int h = 0;
	unsigned int dst_w = 0;
	unsigned int dst_h = 0;
	int result =0;
    
    printf("[TJPEG-%d]Enter ======decodeJpeg  thread 2  !!\n",gettid());  
    printf("[TJPEG-%d]format  JPEG_OUT_FORMAT_YV12, cnt[%d]---\n",gettid(),test_cnt);	   
  	for(cnt =0; cnt<test_cnt; cnt++)
    {

	    memset(&srcParams  , 0, sizeof(MHAL_JPEG_DEC_SRC_IN));
	    memset(&inParams  , 0, sizeof(MHAL_JPEG_DEC_START_IN));
	    memset(&decOutInfo, 0, sizeof(MHAL_JPEG_DEC_INFO_OUT));
	    //printf("[[TJPEG-%d]]out_buffer : %x \n", (unsigned int)dst_buffer);
	    in_time_1 = getTickCountMs();
	    srcParams.srcBuffer = src_va2;
        srcParams.srcLength = BUFFER_SIZE;
        srcParams.srcFD = 0;
	    //mi_mHalJpgDecParser(&srcParams,src_va, BUFFER_SIZE, 0, MHAL_IMAGE_TYPE_JPEG);
	     mHalJpeg(MHAL_IOCTL_MJPEG_DEC_PARSER, (void*) &srcParams, sizeof(srcParams), NULL, 0, NULL);
	      if (result != MHAL_NO_ERROR)
        {
            printf("[TJPEG-%d] eMJpegParse failed:0x%X!",gettid(), result);
            //assert(0);
        }
         decOutInfo.jpgDecHandle = srcParams.jpgDecHandle;
	    //mi_mHalJpgDecGetInfo(&decOutInfo);
	    result = mHalJpeg(MHAL_IOCTL_MJPEG_DEC_GET_INFO, NULL, 0, (void *)&decOutInfo, sizeof(decOutInfo), NULL);
        if (result != MHAL_NO_ERROR)
        {
            printf("[TJPEG-%d] eMJpegParse get info failed:0x%X!",gettid(), result);
        }
        w = decOutInfo.srcWidth;
	    h = decOutInfo.srcHeight;
	    dst_w = ((decOutInfo.srcWidth + 15)/16) * 16;//501;//TO_CEIL(w, 4);      //w;//200;//
	    dst_h = ((decOutInfo.srcHeight + 15)/16) *16;//501;//TO_CEIL(h, 4);    //h;//200;//
	    dst_size    =JPEG_ROUND_N(dst_w, 32)*JPEG_ROUND_N(dst_h, 32)*3 >> 1;
	    _dstsize =dst_size;	   
	   	_dst_w =dst_w;
	   	_dst_h =dst_h;
	    inParams.dstFormat = JPEG_OUT_FORMAT_YV12;
		inParams.jpgDecHandle = srcParams.jpgDecHandle;
	    inParams.srcBuffer = src_va2;
	    inParams.srcLength= BUFFER_SIZE;
	    inParams.srcBufSize = BUFFER_SIZE;
	    inParams.srcFD = 0;//srcFD;	    
	    inParams.dstWidth = dst_w;
	    inParams.dstHeight = dst_h;
	    inParams.dstVirAddr = (UINT8*) dst_buffer2;
	    inParams.dstPhysAddr = NULL;
	
	    inParams.doDithering = 0;//getDitherImage() ? 1 : 0;
	    inParams.doRangeDecode = 0;    
	    
	    
	    printf("[TJPEG-%d] file width/height: %d %d->%d %d!!\n",gettid(), w, h, dst_w, dst_h); 	
	    result = mHalJpeg(MHAL_IOCTL_MJPEG_DEC_START, (void*)&inParams, sizeof(inParams), NULL, 0, NULL);
        if (result == MHAL_NO_ERROR)
        {
	        in_time_2 = getTickCountMs();
	         printf("[TJPEG-%d]decode Time cnt[%d] YV12 [%lld]ms\n",gettid(), cnt,in_time_2 -in_time_1);   
	        avtime_2 += (in_time_2 -in_time_1); 
	        if(_compare_dst)
	        {
		         if(cMemCmp((char *)dst_buffer2, (char *)dst_buffer_g, dst_size))
		         {
		         	printf("[TJPEG-%d]compare failed,cnt[%d]~~\n",gettid(),cnt);
		         	char buf0[255];FILE *fp = NULL;
				    sprintf (buf0, "/data/jpegtest/t2_out_%ld_%ld.yuv.error_%ld", _dst_w, _dst_h,cnt);
					printf("[TJPEG-%d] test_piep_dump : %s!!\n",gettid(),buf0);      
				    fp = fopen(buf0, "wb");
				    if(fp== NULL)
					{
						 printf("[TJPEG-%d]open dst file failed!!\n",gettid());  
					}
				    fwrite((void *)dst_buffer2, 1,BUFFER_SIZE, fp); 
				    fclose(fp);  
		         }
		         else
		         {
		           printf("[TJPEG-%d]compare sucess,cnt[%d]~~\n",gettid(),cnt);
		         }  	
	        }
	    }
	    else
	    { 
	        printf("[TJPEG-%d]decode failed~~~\n",gettid());
	    }
     }
	printf("[TJPEG-%d]decode Time EXIT AVA-YV12 [%lld]ms\n",gettid(), avtime_2/test_cnt);   
	status2 =3;
    return 0;
}



void* decodeJpeg_g()
{
    FILE *fp;
    unsigned int dst_size;//, dst_size1 ;  
    MHAL_JPEG_DEC_SRC_IN srcParams = {0};
    MHAL_JPEG_DEC_START_IN inParams ;
    MHAL_JPEG_DEC_INFO_OUT decOutInfo ;
    void* procHandler = NULL;
  
    int64_t in_time_1 = 0;
    int64_t in_time_2 = 0;
    //int64_t _avtime_1 = 0;
    int64_t avtime_2 = 0;
    //int cnt =0;
 	unsigned int w =0;
	unsigned int h = 0;
	unsigned int dst_w = 0;
	unsigned int dst_h = 0;
	int result =0;
    
    printf("TJPEG-%d]Enter ======decodeJpeg thread g JPEG_OUT_FORMAT_YV12!!\n",gettid());  
    //printf("TJPEG-%d]format  JPEG_OUT_FORMAT_YV12, cnt[%d]---\n",test_cnt);	   
    //for(cnt =0; cnt<test_cnt; cnt++)
    {

	    memset(&srcParams  , 0, sizeof(MHAL_JPEG_DEC_SRC_IN));
	    memset(&inParams  , 0, sizeof(MHAL_JPEG_DEC_START_IN));
	    memset(&decOutInfo, 0, sizeof(MHAL_JPEG_DEC_INFO_OUT));
	    //printf("[TJPEG-%d]out_buffer : %x \n", (unsigned int)dst_buffer);
	    in_time_1 = getTickCountMs();
	    srcParams.srcBuffer = src_va0;
        srcParams.srcLength = BUFFER_SIZE;
        srcParams.srcFD = 0;
	    //mi_mHalJpgDecParser(&srcParams,src_va, BUFFER_SIZE, 0, MHAL_IMAGE_TYPE_JPEG);
	     mHalJpeg(MHAL_IOCTL_MJPEG_DEC_PARSER, (void*) &srcParams, sizeof(srcParams), NULL, 0, NULL);
	      if (result != MHAL_NO_ERROR)
        {
            printf("[TJPEG-%d] eMJpegParse failed:0x%X!",gettid(), result);
            //assert(0);
        }
         decOutInfo.jpgDecHandle = srcParams.jpgDecHandle;
	    //mi_mHalJpgDecGetInfo(&decOutInfo);
	    result = mHalJpeg(MHAL_IOCTL_MJPEG_DEC_GET_INFO, NULL, 0, (void *)&decOutInfo, sizeof(decOutInfo), NULL);
        if (result != MHAL_NO_ERROR)
        {
            printf("[TJPEG-%d] eMJpegParse get info failed:0x%X!",gettid(), result);
            //assert(0);
        }
        w = decOutInfo.srcWidth;
	    h = decOutInfo.srcHeight;
	    dst_w = ((decOutInfo.srcWidth + 15)/16) * 16;//501;//TO_CEIL(w, 4);      //w;//200;//
	    dst_h = ((decOutInfo.srcHeight + 15)/16) *16;//501;//TO_CEIL(h, 4);    //h;//200;//
	    _dst_w =dst_w;
	   	_dst_h =dst_h;
	   	dst_size    =JPEG_ROUND_N(dst_w, 32)*JPEG_ROUND_N(dst_h, 32)*3 >> 1;
	    _dstsize =dst_size;
	   
	    inParams.dstFormat = JPEG_OUT_FORMAT_YV12;
		inParams.jpgDecHandle = srcParams.jpgDecHandle;
	    inParams.srcBuffer = src_va0;
	    inParams.srcLength= BUFFER_SIZE;
	    inParams.srcBufSize = BUFFER_SIZE;
	    inParams.srcFD = 0;//srcFD;	    
	    inParams.dstWidth = dst_w;
	    inParams.dstHeight = dst_h;
	    inParams.dstVirAddr = (UINT8*) dst_buffer_g;
	    inParams.dstPhysAddr = NULL;
	
	    inParams.doDithering = 0;//getDitherImage() ? 1 : 0;
	    inParams.doRangeDecode = 0;    
	    
	    
	    printf("TJPEG-%d] file width/height: %d %d->%d %d!!\n",gettid(), w, h, dst_w, dst_h); 	
	    result = mHalJpeg(MHAL_IOCTL_MJPEG_DEC_START, (void*)&inParams, sizeof(inParams), NULL, 0, NULL);
        if (result == MHAL_NO_ERROR)
        {
	         in_time_2 = getTickCountMs();
	         printf("TJPEG-%d]decode Time  YV12 [%lld]ms\n",gettid(),in_time_2 -in_time_1);   
	        //if(cnt>=1)
	        // avtime_2 += (in_time_2 -in_time_1);    
	    }
	    else
	    { 
	        printf("TJPEG-%d]decode failed~~~\n",gettid());
	    }
	    char buf0[255];
	    sprintf (buf0, "/data/jpegtest/tg_out_%ld_%ld.yuv", _dst_w, _dst_h);
	    printf("[TJPEG-%d] test_piep_dump : %s!!\n",gettid(),buf0); 
	    fp = fopen(buf0, "wb");
	    if(fp== NULL)
		{
			 printf("[TJPEG-%d]open dst file failed!!\n",gettid());  
		}
	    fwrite((void *)dst_buffer_g, 1,BUFFER_SIZE, fp); 
	    fclose(fp);       
     }
	//printf("[TJPEG-%d]decode Time AVA-RGB [%lld]ms\n", _avtime_1/(test_cnt-1));   
	//printf("TJPEG-%d]decode Time EXIT AVA-YV12 [%lld]ms\n", avtime_2/test_cnt);   
    return 0;
}

int cMemCmp(char *ptsrc, char *ptdst, int size)
{
	int cnt =0;
	for(cnt =0; cnt <size; cnt++)
	{
		if(ptsrc[cnt]!= ptdst[cnt])
	    {
	    	return 1;
		}
	}
	return 0;
}

#define SRC_HEX
typedef enum
{
    YUYV = 0,
    YVYU,
    NV12,
    NV21
} JpegDrvEncYUVFormat;
#include "jpeg_hal.h"
#define BUFFER_SIZE (1664*600)
#define HW_ALIGN (64)
#define MY_FGETC(pFile)  ( ( (t = fgetc(pFile)) >= 'a') ? t -'a' + 10 : t - '0' )
#define TO_CEIL(x,a) ( (((unsigned long)x) + (((unsigned long)a)-1)) & ~(((unsigned long)a)-1) )
int encode_test()
{
    int src_fd, dst_fd, dst_size;
    unsigned int src_pa = 0, dst_pa = 0; 
    unsigned int enc_size;
    unsigned char *src_va;
    unsigned char *dst_va;
    unsigned int dst_offset =0;

    unsigned int w, h;
    unsigned int srcFormat = YUYV ;  //NV12 ;//

    w = 800;//720;//48;
    h = 600;//480;//64;
    
    printf("bin hell world %d %d !!\n", w, h);     

    FILE *srcImageFile;
    unsigned long index;

    unsigned int raw_data_u, raw_data_v, raw_data_u1, raw_data_v1;
    unsigned int raw_data_y0, raw_data_y1, raw_data_y2,raw_data_y3;
    unsigned int write_data ;
    unsigned int d0 , d1, d2, d3, d4, d5, d6, d7, d8;
    unsigned int t ;
    unsigned char* srcAddrVA ;
    const char* srcImagePath = "/data/jpegtest/yuv_src.hex";

    unsigned int height = h;
    unsigned int imgStride = 0 ;
    unsigned int memStride = 0 ;
    unsigned int heightStride = TO_CEIL((h), (srcFormat == YUYV)? 8 : 16);
    int chromaFD ;
    unsigned int     srcBufferSize , chromaBufferSize ;
    unsigned int    chromaAddrPA = 0;
    unsigned char* chromaAddrVA = NULL; 
    unsigned int width = w;
    unsigned int width2;


    JpgEncHal* jpgEncoder = new JpgEncHal();

    if(!jpgEncoder->lock())
    {
        printf("can't lock resource");
        return -1;
    }


    if(srcFormat == YUYV || srcFormat == YVYU)
    {
      jpgEncoder->setEncSize(w,h, JpgEncHal::kENC_YUY2_Format) ;
      jpgEncoder->setQuality(90);  //jpgEncoder->setQuality(95);
    }else
    {
      jpgEncoder->setEncSize(w,h, JpgEncHal::kENC_NV12_Format) ;
      jpgEncoder->setQuality(90);  
    }

#if 1
    width2 = ((width+1)>>1) << 1;

    if (srcFormat == YUYV || srcFormat == YVYU)
    {
        imgStride = TO_CEIL((width2 << 1), 32) ; 
        heightStride = TO_CEIL((height), 8) ;  


    }
    else if (srcFormat == NV12 || srcFormat == NV21)
    {        
        imgStride = TO_CEIL((width2), 16) ; 
        heightStride = TO_CEIL((height), 16) ;         
    }
    memStride        = jpgEncoder->getSrcBufMinStride();
    srcBufferSize    = jpgEncoder->getSrcBufMinSize() + HW_ALIGN;
    chromaBufferSize = jpgEncoder->getSrcCbCrBufMinSize() + HW_ALIGN;

#else
    width2 = ((width+1)>>1) << 1;

    if (srcFormat == YUYV || srcFormat == YVYU)
    {
        imgStride = TO_CEIL((width2 << 1), 32) ; //(((width2 << 1)+31)&~31); // multiple of 32
        memStride = TO_CEIL((width2 << 1), 128) ; //(((width << 1)+127)&~127); // multiple of 128  
       
        
        heightStride = TO_CEIL((height), 8) ; //((height+7)&~7); // multiple of 8        

        srcBufferSize = memStride * heightStride;
    }
    else if (srcFormat == NV12 || srcFormat == NV21)
    {        
        imgStride = TO_CEIL((width2), 16) ; //((width2+15)&~15); // multiple of 16
        
        memStride = TO_CEIL((width2), 128) ; //((width2+127)&~127); // multiple of 128                
        
        heightStride = TO_CEIL((height), 16) ; //((height+15)&~15); // multiple of 16 
        
        srcBufferSize = memStride * heightStride;        
        chromaBufferSize = memStride * heightStride / 2;
    }
#endif

    src_va = (unsigned char *)malloc(srcBufferSize );
    src_va = (unsigned char*)TO_CEIL(src_va, 16) ;
  
    printf("go %d %x!!\n", __LINE__, src_va);     
    
    if(src_va == NULL)
    {
        printf("Can not allocate memory\n");
        return -1;
    }

    printf("go %d !!\n", __LINE__);  
   

#ifdef SRC_HEX

    srcAddrVA = src_va ;

    memset(srcAddrVA, 0, sizeof(char)*(srcBufferSize /*BUFFER_SIZE + 16*/));

    printf("go %d !!\n", __LINE__);  
  

    srcImageFile = fopen(srcImagePath, "rt");
    for (;srcImageFile == NULL;)
    {
        printf("No src image binary file.\n");
        for( unsigned int cnt=0; cnt<100000 ;cnt++ );
        srcImageFile = fopen(srcImagePath, "rt");
        return 0;
    }
    
    if (srcFormat == YUYV || srcFormat == YVYU)
    {

        for (unsigned int i=0; i<height; i++)
        {

            for(unsigned int xi =0;xi < (imgStride);xi+=4){
               d0 =  MY_FGETC(srcImageFile) ; 
               d1 =  MY_FGETC(srcImageFile) ; 
               d2 =  MY_FGETC(srcImageFile) ; 
               d3 =  MY_FGETC(srcImageFile) ; 
               d4 =  MY_FGETC(srcImageFile) ; 
               d5 =  MY_FGETC(srcImageFile) ; 
               d6 =  MY_FGETC(srcImageFile) ; 
               d7 =  MY_FGETC(srcImageFile) ;  
               d8 =  fgetc(srcImageFile) ;
               
              raw_data_v = (d0<<4) + d1 ;
              raw_data_y1 = (d2<<4) + d3 ;
              raw_data_u = (d4<<4) + d5 ;
              raw_data_y0 = (d6<<4) + d7 ;
              *(unsigned char *)(srcAddrVA+ i * memStride + xi    ) = raw_data_y0 ;
              *(unsigned char *)(srcAddrVA+ i * memStride + xi + 1) = (srcFormat == YUYV)? raw_data_u : raw_data_v ;
              *(unsigned char *)(srcAddrVA+ i * memStride + xi + 2) = raw_data_y1 ;
              *(unsigned char *)(srcAddrVA+ i * memStride + xi + 3) = (srcFormat == YUYV)? raw_data_v : raw_data_u;
            }

        }
    } else if(srcFormat == NV12 || srcFormat == NV21)
    {
    printf("go %d !!\n", __LINE__);  

      
        for (int i=0; i<heightStride; i++)
        {         

           for(int xi =0;xi < (imgStride);xi+=4)
           {
              d0 =  MY_FGETC(srcImageFile) ; 
              d1 =  MY_FGETC(srcImageFile) ; 
              d2 =  MY_FGETC(srcImageFile) ; 
              d3 =  MY_FGETC(srcImageFile) ; 
              d4 =  MY_FGETC(srcImageFile) ; 
              d5 =  MY_FGETC(srcImageFile) ; 
              d6 =  MY_FGETC(srcImageFile) ; 
              d7 =  MY_FGETC(srcImageFile) ; 
              d8 =  fgetc(srcImageFile) ;
              
             raw_data_y3 = (d0<<4) + d1 ;
             raw_data_y2 = (d2<<4) + d3 ;
             raw_data_y1 = (d4<<4) + d5 ;
             raw_data_y0 = (d6<<4) + d7 ;
             
             *(unsigned char *)(srcAddrVA+ i * memStride + xi    ) = raw_data_y0 ;
             *(unsigned char *)(srcAddrVA+ i * memStride + xi + 1) = raw_data_y1 ;
             *(unsigned char *)(srcAddrVA+ i * memStride + xi + 2) = raw_data_y2 ;
             *(unsigned char *)(srcAddrVA+ i * memStride + xi + 3) = raw_data_y3 ;
           }
              
        }    
    printf("go %d !!\n", __LINE__);  


        /* Chroma Buffer*/
        
        
        chromaBufferSize = memStride * heightStride / 2;
        chromaAddrVA = (unsigned char*)malloc(chromaBufferSize);
        chromaAddrVA = (unsigned char*)TO_CEIL(chromaAddrVA, 16) ;
        if(chromaAddrVA == NULL)
        {
            printf("Can not allocate src chroma buffer!!\n");
            //pmem_free(srcAddrVA, srcBufferSize, srcFD);
            return 0;
        }             
        memset(chromaAddrVA, 0, sizeof(char) * chromaBufferSize);
        
        for (int i=0; i<(heightStride>>1); i++)
        {        
            for(int xi =0;xi < (imgStride);xi+=4)
            {
               d0 =  MY_FGETC(srcImageFile) ; 
               d1 =  MY_FGETC(srcImageFile) ; 
               d2 =  MY_FGETC(srcImageFile) ; 
               d3 =  MY_FGETC(srcImageFile) ; 
               d4 =  MY_FGETC(srcImageFile) ; 
               d5 =  MY_FGETC(srcImageFile) ; 
               d6 =  MY_FGETC(srcImageFile) ; 
               d7 =  MY_FGETC(srcImageFile) ;  
               d8 =  fgetc(srcImageFile) ;
               
               raw_data_v1 = (d0<<4) + d1 ;
               raw_data_u1 = (d2<<4) + d3 ;
               raw_data_v = (d4<<4) + d5 ;
               raw_data_u = (d6<<4) + d7 ;
               
               *(unsigned char *)(chromaAddrVA + i * memStride + xi    ) = (srcFormat == NV12)?  raw_data_u : raw_data_v  ;
               *(unsigned char *)(chromaAddrVA + i * memStride + xi + 1) = (srcFormat == NV12)?  raw_data_v : raw_data_u  ;
               *(unsigned char *)(chromaAddrVA + i * memStride + xi + 2) = (srcFormat == NV12)? raw_data_u1 : raw_data_v1 ;
               *(unsigned char *)(chromaAddrVA + i * memStride + xi + 3) = (srcFormat == NV12)? raw_data_v1 : raw_data_u1 ;
            }  
        } 
    }
    printf("go %d !!\n", __LINE__);  



#else
    srcImageFile = fopen("/data/jpegtest/Before.raw", "r");
    fread(src_va , 1 , BUFFER_SIZE , srcImageFile);
    fclose(srcImageFile);
#endif 
    printf("go %d !!\n", __LINE__);     
    
    
    

//    fp = fopen("/data/jpegtest/ttt.raw", "w");
//    fwrite(src_va , 1 , 6144 , fp);
//    fclose(fp);
//

#if 1
    dst_size = BUFFER_SIZE;
    dst_va = (unsigned char *)malloc(dst_size);
    memset(dst_va, 0, sizeof(char) * dst_size);
    if(dst_va == NULL)
    {
        printf("Can not allocate memory\n");
        return -1;
    } 
    printf("src va:0x%x, pa:0x%x dst va:0x%x, pa:0x%x\n", (unsigned long)src_va , src_pa, (unsigned long)dst_va , dst_pa);
    dst_size = BUFFER_SIZE;
    dst_va = (unsigned char *)malloc(dst_size);
    memset(dst_va, 0xFF, dst_size);

    printf("src va:0x%x, pa:0x%x dst va:0x%x", (unsigned long)src_va , src_pa, (unsigned long)dst_va);
    dst_pa = (unsigned long)dst_va;
#endif


    dst_offset = 0 ;
    jpgEncoder->setSrcAddr((void *)src_va, (void *)chromaAddrVA);
    jpgEncoder->setSrcBufSize(jpgEncoder->getSrcBufMinStride() ,srcBufferSize, chromaBufferSize);
    jpgEncoder->setDstAddr((void *)dst_va + dst_offset);
    jpgEncoder->setDstSize(dst_size);    
    jpgEncoder->enableSOI(1);

    if(jpgEncoder->start(&enc_size))
    {
        FILE *fp;
        printf("encode SUCCESS, size %x~~~\n", enc_size);        
        fp = fopen("/data/jpegtest/ttt.jpg", "w");
        int index;
        for(index = 0 ; index < enc_size ; index++)
        {
            fprintf(fp, "%c", dst_va[index]);
        }
        fclose(fp);
    }
    else
    {
        printf("encode FAIL, size %x~~~\n", enc_size);
        FILE *fp;
        fp = fopen("/data/jpegtest/ttt.jpg", "w");
        int index;
        for(index = 0 ; (index + dst_offset)< enc_size ; index++)
        {
            fprintf(fp, "%c", dst_va[index+ dst_offset]);
        }
        fclose(fp);        
    }
    
    jpgEncoder->unlock();
//    free(src_va);
//    if(chromaAddrVA )
//      free(chromaAddrVA );
//    free(dst_va);
   
    delete jpgEncoder;
    return 0;
}