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
  
//#include <linux/ion.h>
#include <linux/ion_drv.h>
#include <ion/ion.h>
#include <ion.h>





#include "m4u_lib.h"
#include "img_mmu.h"

 
#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "IMG_MMU"

#if 1
#define IMGMMU_W(fmt, arg...)    ALOGW(LOG_TAG fmt, ##arg)
#define IMGMMU_D(fmt, arg...)    ALOGD(LOG_TAG fmt, ##arg)
#else
#define IMGMMU_W(fmt, arg...)    
#define IMGMMU_D(fmt, arg...)    
#endif


//M4U_CLNTMOD_VENC    = 0,	//0
//                         
//M4U_CLNTMOD_VDEC       ,
//M4U_CLNTMOD_ROT        ,
//M4U_CLNTMOD_OVL        ,
//M4U_CLNTMOD_WDMA       ,
//M4U_CLNTMOD_RDMA       , //5
//
//M4U_CLNTMOD_CMDQ       ,
//M4U_CLNTMOD_DBI        ,
//M4U_CLNTMOD_G2D        ,                             
//M4U_CLNTMOD_JPGDEC     ,
//M4U_CLNTMOD_JPGENC     , //10
//
//M4U_CLNTMOD_VIP        ,
//M4U_CLNTMOD_DISP       ,
//M4U_CLNTMOD_VID        ,
//M4U_CLNTMOD_GDMA       ,
//M4U_CLNTMOD_IMG        , //15
//
//M4U_CLNTMOD_LSCI       ,
//M4U_CLNTMOD_FLKI       ,
//M4U_CLNTMOD_LCEI       ,
//M4U_CLNTMOD_LCSO       ,
//M4U_CLNTMOD_ESFKO      , //20
//
//M4U_CLNTMOD_AAO        ,
//                       
//M4U_CLNTMOD_AUDIO      ,
//
//
//M4U_CLNTMOD_UNKNOWN,
//M4U_CLNTMOD_MAX

#ifdef IMGMMU_SUPPORT_M4U

bool imgMmu_create(MTKM4UDrv **ppM4uDrv, M4U_MODULE_ID_ENUM mID)
{
   
   if(*ppM4uDrv == NULL)
    *ppM4uDrv = new MTKM4UDrv();

   IMGMMU_D("::[CREATE] enable m4u %x, ID %d, L:%d,  JD(9),JE(10),GD(14),IMG(15),VD(1)!! \n", (unsigned int)*ppM4uDrv, mID, __LINE__);
   (*ppM4uDrv)->m4u_enable_m4u_func(mID);
   //(*ppM4uDrv)->m4u_disable_m4u_func(mID);
   
   return true ;
   
}

bool imgMmu_reset(MTKM4UDrv **ppM4uDrv, M4U_MODULE_ID_ENUM mID)
{

    if(*ppM4uDrv == NULL){
       IMGMMU_W("::[RESET] m4u is %x, mID %d, L:%d!! \n", (unsigned int)*ppM4uDrv, mID, __LINE__);
       return false;
    }
   
    if(M4U_STATUS_OK != (*ppM4uDrv)->m4u_reset_mva_release_tlb(mID))
    {
        IMGMMU_D("Can't reset mva\n");
        delete *ppM4uDrv;
        *ppM4uDrv = NULL;
        return false; 
    }   
   
   return true;   
}




bool imgMmu_alloc_pa_mci(MTKM4UDrv **ppM4uDrv, M4U_MODULE_ID_ENUM mID, JUINT32 va, JUINT32 size, JUINT32 *pa)
{
   int security = 0;
   int cache_coherent = 0;
    if(*ppM4uDrv == NULL || size == 0 || va == 0){
       IMGMMU_W("::[ALLOC] m4u is %x, mID %d, size %x, va %x, L:%d!! \n", (unsigned int)*ppM4uDrv, mID, size, va,__LINE__);
       return false;
    }
    {
      security = 1;
      cache_coherent = 1;
    }

    if(M4U_STATUS_OK != (*ppM4uDrv)->m4u_alloc_mva(mID, va, size, security, cache_coherent, pa) )
    {
        IMGMMU_W("::[ALLOC] mID %d alloc m4u va %x, size %x, L:%d!! \n",  mID, va, size, __LINE__);
        IMGMMU_D("Can not allocate mva\n");
        delete *ppM4uDrv;
        *ppM4uDrv = NULL;
        *pa = 0;
        return false;        
    }
     
    if(M4U_STATUS_OK != (*ppM4uDrv)->m4u_insert_tlb_range(mID, *pa, *pa + size - 1, RT_RANGE_HIGH_PRIORITY, 1))
    {
        IMGMMU_D("Can't insert tlb range\n");
        (*ppM4uDrv)->m4u_dealloc_mva(mID, va, size, *pa);
        delete *ppM4uDrv;
        *ppM4uDrv = NULL;
        *pa = 0;
        return false;  
    }

        IMGMMU_W("::[ALLOC] mID %d alloc m4u va %x, size %x, mva %x, L:%d!! \n",  mID, va, size, *pa,__LINE__);    
//    M4U_PORT_STRUCT m4uPort;
//
//    m4uPort.ePortID = pID;
//    m4uPort.Virtuality = 1;
//    m4uPort.Security = 0;
//    m4uPort.domain = 3;
//    m4uPort.Distance = 1;
//    m4uPort.Direction = 0;
//
//    if(M4U_STATUS_OK != (*ppM4uDrv)->m4u_config_port(&m4uPort))
//    {
//        IMGMMU_D("Can not config m4u port\n");
//        (*ppM4uDrv)->m4u_invalid_tlb_range(mID, *pa, *pa + size - 1);
//        (*ppM4uDrv)->m4u_dealloc_mva(mID, va, size, *pa);
//        delete *ppM4uDrv;
//        *ppM4uDrv = NULL;
//        return false;         
//    }
   return true ;
}
bool imgMmu_alloc_pa(MTKM4UDrv **ppM4uDrv, M4U_MODULE_ID_ENUM mID, JUINT32 va, JUINT32 size, JUINT32 *pa)
{
   int security = 0;
   int cache_coherent = 0;
    if(*ppM4uDrv == NULL || size == 0 || va == 0){
       IMGMMU_W("::[ALLOC] m4u is %x, mID %d, size %x, va %x, L:%d!! \n", (unsigned int)*ppM4uDrv, mID, size, va,__LINE__);
       return false;
    }
//    if(flag & IMGMMU_MASK_MCI){
//      security = 1;
//      cache_coherent = 1;
//    }
    if(M4U_STATUS_OK != (*ppM4uDrv)->m4u_alloc_mva(mID, va, size, security, cache_coherent, pa) )
    {
        IMGMMU_W("::[ALLOC] mID %d alloc m4u va %x, size %x, L:%d!! \n",  mID, va, size, __LINE__);
        IMGMMU_D("Can not allocate mva\n");
        delete *ppM4uDrv;
        *ppM4uDrv = NULL;
        *pa = 0;
        return false;        
    }
    if(M4U_STATUS_OK != (*ppM4uDrv)->m4u_insert_tlb_range(mID, *pa, *pa + size - 1, RT_RANGE_HIGH_PRIORITY, 1))
    {
        IMGMMU_D("Can't insert tlb range\n");
        (*ppM4uDrv)->m4u_dealloc_mva(mID, va, size, *pa);
        delete *ppM4uDrv;
        *ppM4uDrv = NULL;
        *pa = 0;
        return false;  
    }
        IMGMMU_W("::[ALLOC] mID %d alloc m4u va %x, size %x, mva %x, L:%d!!\n",  mID, va, size, *pa,__LINE__);    
        

   
   return true ;
   
}

bool imgMmu_cfg_port_mci(MTKM4UDrv *pM4uDrv, M4U_MODULE_ID_ENUM mID, M4U_PORT_ID_ENUM pID)
{
    if(pM4uDrv == NULL){
       IMGMMU_W("::[CONFIG] m4u is %x, mID %d, L:%d!! \n", (unsigned int)pM4uDrv, mID, __LINE__);
       return false;
    }
    M4U_PORT_STRUCT m4uPort;
    m4uPort.ePortID = pID;
    m4uPort.Virtuality = 1;
    m4uPort.Security = 1;
    m4uPort.domain = 3;
    m4uPort.Distance = 1;
    m4uPort.Direction = 0;
    if(M4U_STATUS_OK != pM4uDrv->m4u_config_port(&m4uPort))
    {
        IMGMMU_D("Can not config m4u port\n");
        return false;         
    }
   return true ;   
}
bool imgMmu_cfg_port(MTKM4UDrv *pM4uDrv, M4U_MODULE_ID_ENUM mID, M4U_PORT_ID_ENUM pID)
{

    if(pM4uDrv == NULL){
       IMGMMU_W("::[CONFIG] m4u is %x, mID %d, L:%d!! \n", (unsigned int)pM4uDrv, mID, __LINE__);
       return false;
    }

    M4U_PORT_STRUCT m4uPort;

    m4uPort.ePortID = pID;
    m4uPort.Virtuality = 1;
    m4uPort.Security = 0;
    m4uPort.domain = 3;
    m4uPort.Distance = 1;
    m4uPort.Direction = 0;

    if(M4U_STATUS_OK != pM4uDrv->m4u_config_port(&m4uPort))
    {
        IMGMMU_D("Can not config m4u port\n");
        //(*ppM4uDrv)->m4u_invalid_tlb_range(mID, *pa, *pa + size - 1);
        //(*ppM4uDrv)->m4u_dealloc_mva(mID, va, size, *pa);
        //delete *ppM4uDrv;
        //*ppM4uDrv = NULL;
        return false;         
    }

   
   return true ;   
   
}




void imgMmu_sync(MTKM4UDrv *pM4uDrv, M4U_MODULE_ID_ENUM mID, JUINT32 va, JUINT32 size, JUINT32 isWrite )
{
  if(pM4uDrv == NULL){
     IMGMMU_W("::[SYNC] m4u is %x, mID %d, L:%d!! \n", (unsigned int)pM4uDrv, mID, __LINE__);
     return ;
  }   
  
  if( isWrite )
    pM4uDrv->m4u_cache_sync(mID, M4U_CACHE_FLUSH_BEFORE_HW_WRITE_MEM, va, size);   
  else
    pM4uDrv->m4u_cache_sync(mID, M4U_CACHE_FLUSH_BEFORE_HW_READ_MEM, va, size);   
  
}   

void imgMmu_dealloc_pa(MTKM4UDrv *pM4uDrv, M4U_MODULE_ID_ENUM mID, JUINT32 va, JUINT32 size, JUINT32 *pa)
{

      IMGMMU_W("::[DEALLOC] mID %d alloc m4u va %x, size %x, mva %x, L:%d!! \n",  mID, va, size, *pa, __LINE__);         
      if(pM4uDrv == NULL){
         IMGMMU_W("::[DEALLOC] m4u is %x, mID %d, L:%d!! \n", (unsigned int)pM4uDrv, mID, __LINE__);
         return ;
      }     
      
      if(*pa != 0){
        pM4uDrv->m4u_invalid_tlb_range(mID, *pa, *pa + size - 1);
        pM4uDrv->m4u_dealloc_mva(mID, va, size, *pa);
        *pa = 0;
      }

}




//for ION flow, only map virtual pa tlb
bool imgMmu_pa_map_tlb(MTKM4UDrv **ppM4uDrv, M4U_MODULE_ID_ENUM mID, JUINT32 size, JUINT32 *pa)
{

    if(*ppM4uDrv == NULL || size == 0 ){
       IMGMMU_W("::[MAP] m4u is %x, mID %d, size %x, L:%d!! \n", (unsigned int)*ppM4uDrv, mID, size,__LINE__);
       return false;
    }
    //if(mID == M4U_CLNTMOD_JPGENC) mID = M4U_CLNTMOD_IMG ;
    if(M4U_STATUS_OK != (*ppM4uDrv)->m4u_insert_tlb_range(mID, *pa, *pa + size - 1, RT_RANGE_HIGH_PRIORITY, 1))
    {
        IMGMMU_D("Can't MAP tlb range, L:%d\n", __LINE__);
        delete *ppM4uDrv;
        *ppM4uDrv = NULL;
        *pa = 0;
        return false;  
    }

   return true ;
   
}

//for ION flow, only unmap virtual pa tlb
void imgMmu_pa_unmap_tlb(MTKM4UDrv *pM4uDrv, M4U_MODULE_ID_ENUM mID, JUINT32 size, JUINT32 *pa)
{
    if( pM4uDrv == NULL){
       IMGMMU_W("::[UNMAP] m4u is %x, mID %d, L:%d!! \n", (unsigned int)pM4uDrv, mID, __LINE__);
       return ;
    }
   
      if(*pa != 0){
         //if(mID == M4U_CLNTMOD_JPGENC) mID = M4U_CLNTMOD_IMG ;
        pM4uDrv->m4u_invalid_tlb_range(mID, *pa, *pa + size - 1);
        *pa = 0;
      }
}

#endif

#ifdef IMGMMU_SUPPORT_ION

bool imgIon_open(JINT32 *ionFD )
{

    *ionFD = mt_ion_open("img_mmu.cpp");  //open("/dev/ion", O_RDONLY);
    if( *ionFD < 0)
    {
      IMGMMU_W("IMG_ION: open fd fail!!\n");
      return false;
    }
    
    return true;
}

bool imgIon_freeVA(void **va, JUINT32 size)
{
  if(*va != NULL)
      munmap(*va, size);
  *va = NULL;
        
  return true;
          
}        


bool imgIon_close(JINT32 ionFD)
{

   close(ionFD);   
   return true;
}


bool imgIon_getPA(JINT32 ionFD, JINT32 bufFD, M4U_MODULE_ID_ENUM mID, void *va, JUINT32 size, JINT32 *bufHandle, JUINT32 *pa)
{
   

    // get handler
    ion_fd_data fd_data;
    fd_data.fd = bufFD;
    int err = 0;

   IMGMMU_D("::[ION_PA] ionFD %d, bufFD %d, mID %d, va %x, size %x, pa %x, L:%d!!\n", ionFD,bufFD, mID, va, size, *pa, __LINE__);
       
    if(ioctl(ionFD, ION_IOC_IMPORT, &fd_data))
    {
      IMGMMU_W("IMG_ION: IMPORT fail!!\n");
      return false;
    }

    *bufHandle = fd_data.handle;

    // config module ID to get physical Address
    ion_custom_data custom_data;
    ion_mm_data mm_data;

    custom_data.cmd = ION_CMD_MULTIMEDIA;
    custom_data.arg = (unsigned long)&mm_data;

    mm_data.mm_cmd = ION_MM_CONFIG_BUFFER;
    mm_data.config_buffer_param.handle = fd_data.handle;
    mm_data.config_buffer_param.eModuleID = mID;
    mm_data.config_buffer_param.security = 0;
    mm_data.config_buffer_param.coherent = 0;
    
    err = ioctl(ionFD, ION_IOC_CUSTOM, &custom_data) ;
    
    if(err != 0 && err != (-ION_ERROR_CONFIG_LOCKED) )
    {
      IMGMMU_W("IMG_ION: CONFIG module fail (err_no = %x)!!\n", err);
      return false;
    }
    
    // get physical address
    ion_sys_data    sys_data;

    custom_data.cmd = ION_CMD_SYSTEM;
    custom_data.arg = (unsigned long)&sys_data;
    
    sys_data.sys_cmd = ION_SYS_GET_PHYS;
    sys_data.get_phys_param.handle = fd_data.handle;

    if(ioctl(ionFD, ION_IOC_CUSTOM, &custom_data))
    {
      IMGMMU_W("IMG_ION: get physical fail!!\n");
      return false;
    }
   
   
   *pa = sys_data.get_phys_param.phy_addr;
   
   
   return true ;
   
   
}

bool imgIon_getVA(JINT32 bufFD, JUINT32 size, void **va)
{

   IMGMMU_D("::[ION_VA] bufFD %d, va %x, size %x, L:%d!!\n", bufFD,*va, size, __LINE__);

    // get virtual address
    *va = mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED, bufFD, 0);
    if((*va == NULL) || (*va == (void*)(-1)))
    {
      IMGMMU_W("IMG_ION: MMAP fail, fd %d, va %x, size %x!!\n", bufFD, *va, size);
      *va = NULL;
      return false ;
    }

       
    return true ;
   
}


bool imgIon_sync(JINT32 ionFD, JINT32 bufHandle)
{
   
    ion_custom_data custom_data;
    ion_sys_data    sys_data;

    custom_data.cmd = ION_CMD_SYSTEM;
    custom_data.arg = (unsigned long)&sys_data;
    sys_data.sys_cmd = ION_SYS_CACHE_SYNC;
    sys_data.cache_sync_param.handle = bufHandle;
    sys_data.cache_sync_param.sync_type = ION_CACHE_FLUSH_ALL;//ION_CACHE_FLUSH_BY_RANGE;

    IMGMMU_D("::[ION_SYNC] ionFD %d, handle %p, L:%d!!\n", ionFD, bufHandle,  __LINE__);
       
    ioctl(ionFD, ION_IOC_CUSTOM, &custom_data);
   
    return true;
}

#endif

