/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/

#define LOG_TAG "Drv/IMem"
//
#include <utils/Errors.h>
#include <cutils/log.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <utils/threads.h>
#include <cutils/atomic.h>
#if defined (__ISP_USE_PMEM__)     //seanlin fix if using PMEM
   #include <cutils/pmem.h>
#endif
//
#include "imem_drv_imp.h"
//
#include "camera_isp.h"
//#include "mm_proprietary.h"
#include <linux/mman-proprietary.h>
//
#define ISP_DRV_DEV_NAME  "/dev/camera-isp"
//
#define _ION_DEVICE_NAME_ "/dev/ion"
//
//#define IMEM_DRV_DELAY usleep(50000);
#define IMEM_DRV_DELAY

#define USE_ION_CARVEOUT_DEBUG 0
//-----------------------------------------------------------------------------
IMemDrv* IMemDrv::createInstance()
{
    DBG_LOG_CONFIG(drv, imem_drv);
    return IMemDrvImp::getInstance();
}
//-----------------------------------------------------------------------------
IMemDrvImp::IMemDrvImp()
    :
#if defined(__ISP_USE_STD_M4U__) || defined(__ISP_USE_ION__)     //seanlin fix if using PMEM
     mpM4UDrv(NULL),
#endif
     mInitCount(0),
     mIspFd(-1),
     mLocal_InitCount(0)
{
    IMEM_DBG("getpid[0x%08x],gettid[0x%08x] ", getpid() ,gettid());

}
//-----------------------------------------------------------------------------
IMemDrvImp::~IMemDrvImp()
{
    IMEM_DBG("");
}
//-----------------------------------------------------------------------------
IMemDrv* IMemDrvImp::getInstance(void)
{
    static IMemDrvImp singleton;
    IMEM_DBG("singleton[0x%lx],getpid[0x%08x],gettid[0x%08x] ",(unsigned long)(&singleton),getpid() ,gettid());
    IMEM_DRV_DELAY
    return &singleton;
}
//-----------------------------------------------------------------------------
void IMemDrvImp::destroyInstance(void)
{
}
//-----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////
///We do increase global and local count first, then judge we have to initial m4uDrv/ion_dev and m4uPort according
///   local count and global count repectively.
MBOOL IMemDrvImp::init(void)
{
    MBOOL Result = MTRUE;
    MINT32 ret = 0;
    ISP_REF_CNT_CTRL_STRUCT ref_cnt;
    //
    Mutex::Autolock lock(mLock);
    //
#if defined(_use_kernel_ref_cnt_)
    //
    if ( mIspFd < 0 )
    {
        mIspFd = open(ISP_DRV_DEV_NAME, O_RDONLY);
        if (mIspFd < 0)    // 1st time open failed.
        {
            IMEM_ERR("ISP kernel open fail, errno(%d):%s.", errno, strerror(errno));
            Result = MFALSE;
            goto EXIT;
        }
        IMEM_INF("HMyo init_.mIspFd(%d)",mIspFd);
    }
    //
    IMEM_DBG("use kernel ref. cnt.mIspFd(%d)",mIspFd);
    ///////////////////////////////////////////////
    //increase global and local count first
    ref_cnt.ctrl = ISP_REF_CNT_INC;
    ref_cnt.id = ISP_REF_CNT_ID_IMEM;
   ref_cnt.data_ptr = (MINT32*)&mInitCount;
    ret = ioctl(mIspFd,ISP_REF_CNT_CTRL,&ref_cnt);
    if(ret < 0)
    {
        IMEM_ERR("ISP_REF_CNT_INC fail(%d),mIspFd(%d)[errno(%d):%s] \n",ret, mIspFd, errno, strerror(errno));
        Result = MFALSE;
        goto EXIT;
    }
    android_atomic_inc(&mLocal_InitCount);
    IMEM_DBG("#flag2# mInitCount(%d),mInitCount>0 and run _use_kernel_ref_cnt_\n",mInitCount);
#else
    IMEM_DBG("mInitCount(%d) ", mInitCount);
    IMEM_DRV_DELAY
    android_atomic_inc(&mInitCount);
    //IMEM_DBG("#flag3# mInitCount(%d),mInitCount>0 and run w\o _use_kernel_ref_cnt_\n",mInitCount);
#endif
    //////////////////////////////////////////
    //init. buf_map
    //erase all
    buf_map.clear();   //actually do nothing.
    //
#if defined (__ISP_USE_PMEM__)
        //
#elif defined (__ISP_USE_STD_M4U__) || defined (__ISP_USE_ION__)
        //////////////////////////////////////////////////////
        // we initial m4udrv and open ion device when local count is 1,
        // and config m4v ports when global count is 1
        if(mLocal_InitCount==1)
        {
           mpM4UDrv = new MTKM4UDrv();
       #if defined (__ISP_USE_ION__)
              mIonDrv = mt_ion_open("imem_drv.cpp");
              if (mIonDrv < 0)
              {
                 IMEM_ERR("ion device open FAIL ");
                 return MFALSE;
              }
              IMEM_INF("open ion id(%d).\n", mIonDrv);
       #endif
        if(mInitCount==1)
           {
              mpM4UDrv->m4u_enable_m4u_func(M4U_PORT_CAM_IMGI);
              mpM4UDrv->m4u_enable_m4u_func(M4U_PORT_CAM_IMGO);
              mpM4UDrv->m4u_enable_m4u_func(M4U_PORT_CAM_IMG2O);
              mpM4UDrv->m4u_enable_m4u_func(M4U_PORT_CAM_LSCI);
              mpM4UDrv->m4u_enable_m4u_func(M4U_PORT_CAM_ESFKO);
              mpM4UDrv->m4u_enable_m4u_func(M4U_PORT_CAM_AAO);
              //
              if(ret != M4U_STATUS_OK)
              {
                 IMEM_ERR("m4u_enable_m4u_func fail");
                 goto EXIT;
              }
              M4U_PORT_STRUCT port;
               if(USE_ION_CARVEOUT_DEBUG)
               {
                    port.Virtuality = 0;
                }
                else
                {
                    port.Virtuality = 1;
                }

                port.Security = 0;
                port.domain = 3;
                port.Distance = 1;
                port.Direction = 0; //M4U_DMA_READ_WRITE
                port.ePortID =M4U_PORT_CAM_IMGI;
                ret = mpM4UDrv->m4u_config_port(&port);
                port.ePortID = M4U_PORT_CAM_IMGO;
                ret = mpM4UDrv->m4u_config_port(&port);
                port.ePortID = M4U_PORT_CAM_IMG2O;
                ret = mpM4UDrv->m4u_config_port(&port);
                port.ePortID = M4U_PORT_CAM_LSCI;
                ret = mpM4UDrv->m4u_config_port(&port);

                port.ePortID = M4U_PORT_CAM_ESFKO;
                ret = mpM4UDrv->m4u_config_port(&port);
                port.ePortID = M4U_PORT_CAM_AAO;
                ret = mpM4UDrv->m4u_config_port(&port);
        }//match if global count
    }//match if local count
#endif

//
    EXIT:
    if(!Result)
    {
    }
    return Result;
}
//-----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////
///We do decrease global and local count first, then judge we have to uninit m4uDrv/ion_dev and m4uPort according
///   local count and global count repectively.
MBOOL IMemDrvImp::uninit(void)
{
    MBOOL Result = MTRUE;
    MINT32 ret = 0;
    ISP_REF_CNT_CTRL_STRUCT ref_cnt;
    //
    Mutex::Autolock lock(mLock);
    //
#if defined(_use_kernel_ref_cnt_)
    if(mIspFd < 0)
    {
        IMEM_ERR("mIspFd < 0 \n");
        goto EXIT;
    }
    ///////////////////////////////////////////////
    //decrease global and local count first
    // More than one user
    ref_cnt.ctrl = ISP_REF_CNT_DEC;
    ref_cnt.id = ISP_REF_CNT_ID_IMEM;
    ref_cnt.data_ptr = (MINT32*)&mInitCount;
    ret = ioctl(mIspFd,ISP_REF_CNT_CTRL,&ref_cnt);
    if(ret < 0)
    {
        IMEM_ERR("ISP_REF_CNT_DEC fail(%d),mIspFd(%d) [errno(%d):%s] \n",ret,mIspFd, errno, strerror(errno));
        Result = MFALSE;
        goto EXIT;
    }
    android_atomic_dec(&mLocal_InitCount);
    //
#else
    ///IMEM_DBG("mInitCount(%d)",mInitCount);
    // More than one user
    android_atomic_dec(&mInitCount);
    //IMEM_INF("-flag2- mInitCount(%d)\n",mInitCount);
#endif
    IMEM_DBG("-flag3- mInitCount(%d),mLocal_InitCount(%d)\n",mInitCount,mLocal_InitCount);

#if defined (__ISP_USE_PMEM__)
    //
#elif defined (__ISP_USE_STD_M4U__) || defined (__ISP_USE_ION__)
    //////////////////////////////////////////////////////
    // we delete m4udrv and close ion device when local count is 1,
    // and unconfig m4v ports when global count is 1
    if ( mLocal_InitCount <= 0 )
    {
       #if defined (__ISP_USE_ION__)
          // we have to handle local ion drv here
          // if process A open ionID, then process B open ionID before process A call ImemDrv_uninit,
          // process A would not do close ionID.
          if (mIonDrv)
          {
            IMEM_INF("close ion id(%d).\n", mIonDrv);
            ion_close(mIonDrv);
          }
       #endif
       //IMEM_INF("-!!!- mInitCount(%d)\n", mInitCount);
       if(mInitCount<=0)
       {
           M4U_PORT_STRUCT port;
           port.Virtuality = 0;
           port.Security = 0;
           port.domain = 3;
           port.Distance = 1;
           port.Direction = 0; //M4U_DMA_READ_WRITE
           port.ePortID =M4U_PORT_CAM_IMGI;
           ret = mpM4UDrv->m4u_config_port(&port);;
           port.ePortID = M4U_PORT_CAM_IMGO;
           ret = mpM4UDrv->m4u_config_port(&port);
           port.ePortID =M4U_PORT_CAM_IMG2O;
           ret = mpM4UDrv->m4u_config_port(&port);
           port.ePortID = M4U_PORT_CAM_LSCI;
           ret = mpM4UDrv->m4u_config_port(&port);
           port.ePortID = M4U_PORT_CAM_ESFKO;
           ret = mpM4UDrv->m4u_config_port(&port);
           port.ePortID = M4U_PORT_CAM_AAO;
           ret = mpM4UDrv->m4u_config_port(&port);
           mpM4UDrv->m4u_disable_m4u_func(M4U_PORT_CAM_IMGI);
           mpM4UDrv->m4u_disable_m4u_func(M4U_PORT_CAM_IMGO);
           mpM4UDrv->m4u_disable_m4u_func(M4U_PORT_CAM_IMG2O);
           mpM4UDrv->m4u_disable_m4u_func(M4U_PORT_CAM_ESFKO);
           mpM4UDrv->m4u_disable_m4u_func(M4U_PORT_CAM_AAO);
       }
       delete mpM4UDrv;
       mpM4UDrv = NULL;
    }
#endif

    EXIT:

#if defined(_use_kernel_ref_cnt_)
    //local ==0, global !=0 del m4u object only
    if ( mLocal_InitCount <= 0 ) {
        if ( mIspFd >= 0 ) {
       close(mIspFd);
       mIspFd = -1;
       IMEM_DBG("mIspFd(%d)",mIspFd);
         }
    }
#endif
    return Result;
}
//-----------------------------------------------------------------------------
MBOOL IMemDrvImp::reset(void)
{
    IMEM_DBG("");
    //erase all
    buf_map.clear(); //actually do nothing.
    //
    return MTRUE;
}
//-----------------------------------------------------------------------------
MINT32 IMemDrvImp::allocVirtBuf(
    IMEM_BUF_INFO* pInfo)
{
#if defined (__ISP_USE_PMEM__)
    IMEM_DBG("__ISP_USE_PMEM__");
    //
    //pInfo->type = BUF_TYPE_PMEM;
    pInfo->virtAddr= (MUINT32) ::pmem_alloc_sync(pInfo->size, &pInfo->memID);
    //
    IMEM_DBG("memID[0x%x]",pInfo->memID);
#elif defined (__ISP_USE_STD_M4U__)
    IMEM_DBG("__ISP_USE_STD_M4U__");
    //
    //pInfo->type = BUF_TYPE_STD_M4U;
    pInfo->memID = (MINT32)(IMEM_MIN_ION_FD-1);
    pInfo->virtAddr = ::memalign(L1_CACHE_BYTES, pInfo->size); //32Bytes align(from 89)
    //pInfo->virtAddr = (MUINT32)::memalign(L1_CACHE_BYTES, (pInfo->size+ (L1_CACHE_BYTES)) & ~(L1_CACHE_BYTES -1)); //32Bytes align
    //will call allocM4UMemory function(we show information there)
    //IMEM_INF("[Std M4U] mID[0x%x]/size[0x%x]/VA[0x%x]",pInfo->memID,pInfo->size,pInfo->virtAddr);
#elif defined (__ISP_USE_ION__)
    IMEM_DBG("__ISP_USE_ION__");
    //
    ion_user_handle_t pIonHandle;
    MINT32 IonBufFd;

    struct ion_fd_data fd_data;
    int ion_prot_flags = pInfo->useNoncache ? 0 : (ION_FLAG_CACHED | ION_FLAG_CACHED_NEEDS_SYNC);
    //a. Allocate a buffer
    if(USE_ION_CARVEOUT_DEBUG)
    {
        if(ion_alloc(
            mIonDrv,
            pInfo->size,
            0, //alignment
            ION_HEAP_CARVEOUT_MASK,
            ion_prot_flags,
            &pIonHandle))
        {
            IMEM_ERR("ion_alloc fail, size(0x%x)",pInfo->size);
            return -1;
        }
    }
    else
    {
        if(ion_alloc_mm(
            mIonDrv,
            pInfo->size,
            0, //alignment
            ion_prot_flags,
            &pIonHandle))
        {
            IMEM_ERR("ion_alloc_mm fail");
            IMEM_ERR("cBuf ID[0x%x]/size[0x%x]",pInfo->memID,pInfo->size);
            return -1;
        }
    }

    //b. Map a new fd for client.
    if(ion_share(
        mIonDrv,
        pIonHandle,
        &IonBufFd))
    {
        IMEM_ERR("ion_share fail");
        IMEM_ERR("cBuf ID[0x%x]/size[0x%x]",pInfo->memID,pInfo->size);
        return -1;
    }
    pInfo->memID = (MINT32)IonBufFd; // Tianshu suggest to keep this fd
    //c. Map FD to a virtual space.
    pInfo->virtAddr = (MUINTPTR)ion_mmap(mIonDrv, NULL, pInfo->size, PROT_READ|PROT_WRITE, MAP_SHARED, IonBufFd, 0);
    //pInfo->virtAddr = (MUINT32)ion_mmap(mIonDrv, NULL, pInfo->size, PROT_READ|PROT_WRITE, MAP_SHARED, IonBufFd, 0);
    if (!pInfo->virtAddr)
    {
        IMEM_ERR("Cannot map ion buffer.");
        IMEM_ERR("cBuf ID[0x%x]/size[0x%x]",pInfo->memID,pInfo->size);
        return -1;
    }
    //
    IMEM_DBG("ionFd[0x%x]",pInfo->memID);
#endif
    IMEM_DBG("mID[0x%x]/size[0x%x]/VA[0x%lx]",pInfo->memID,pInfo->size,(unsigned long)pInfo->virtAddr);  //kk test
    return 0;
}
//-----------------------------------------------------------------------------
MINT32 IMemDrvImp::freeVirtBuf(
    IMEM_BUF_INFO* pInfo)
{
    IMEM_INF("mID[0x%x]/size[0x%x]/VA[0x%lx]/PA[0x%lx]",pInfo->memID,pInfo->size,(unsigned long)pInfo->virtAddr,(unsigned long)pInfo->phyAddr);
    IMEM_DRV_DELAY

#if defined (__ISP_USE_PMEM__)
    //
    //if ( BUF_TYPE_PMEM != pInfo->type ) {
    //    IMEM_ERR("ERROR:mem type(%d)",pInfo->type);
    //    return -1;
    //}
    //
    IMEM_DBG("pmem ID[0x%x]",pInfo->memID);
    //
    if( pInfo->virtAddr != 0 )
    {
        ::pmem_free(
            (MUINT8*)(pInfo->virtAddr),
            pInfo->size,
            pInfo->memID);
    }
//#elif defined (__ISP_USE_STD_M4U__) || defined (__ISP_USE_ION__)
#else
    //
    //if ( BUF_TYPE_STD_M4U == pInfo->type ) {
    if ( IMEM_MIN_ION_FD > pInfo->memID ) {
        if ( pInfo->virtAddr != 0 ) {
            ::free((MUINT8 *)pInfo->virtAddr);
        }

    }
    //
#if defined (__ISP_USE_ION__)
    //
    else /*if ( BUF_TYPE_ION == pInfo->type )*/ {
        ion_user_handle_t IonHandle;
        MINT32 IonBufFd;

        //struct ion_handle_data handle_data;
        //struct ion_fd_data fd_data;
        //
        //if ( BUF_TYPE_ION != pInfo->type ) {
        //    IMEM_ERR("ERROR:mem type(%d)",pInfo->type);
        //    return -1;
        //}
        //
        IMEM_DBG("ionFd[0x%x]",pInfo->memID);
        //a. get handle of ION_IOC_SHARE from fd_data.fd
        IonBufFd = pInfo->memID;
        if(ion_import(
            mIonDrv,
            IonBufFd,
            &IonHandle))
        {
            IMEM_ERR("ion_import fail(0x%x)",IonBufFd);
            IMEM_ERR("cBuf ID[0x%x]/size[0x%x]/VA[0x%lx]/PA[0x%lx]/S[%d]/C[%d]",pInfo->memID,pInfo->size,(unsigned long)pInfo->virtAddr,(unsigned long)pInfo->phyAddr,pInfo->bufSecu,pInfo->bufCohe);
            return -1;
        }
        //free for IMPORT ref cnt
        if(ion_free(
            mIonDrv,
            IonHandle))
        {
            IMEM_ERR("ion_free fail");
            IMEM_ERR("cBuf ID[0x%x]/size[0x%x]/VA[0x%lx]/PA[0x%lx]/S[%d]/C[%d]",pInfo->memID,pInfo->size,(unsigned long)pInfo->virtAddr,(unsigned long)pInfo->phyAddr,pInfo->bufSecu,pInfo->bufCohe);
            return -1;
        }
        //b. pair of mmap
        ion_munmap(mIonDrv, (char*)pInfo->virtAddr, pInfo->size);
        //c. pair of ION_IOC_SHARE
        ion_share_close(mIonDrv, pInfo->memID);
        //d. pair of ION_IOC_ALLOC
        if(ion_free(
            mIonDrv,
            IonHandle))
        {
            IMEM_ERR("ion_free fail");
            IMEM_ERR("cBuf ID[0x%x]/size[0x%x]/VA[0x%lx]/PA[0x%lx]/S[%d]/C[%d]",pInfo->memID,pInfo->size,(unsigned long)pInfo->virtAddr,(unsigned long)pInfo->phyAddr,pInfo->bufSecu,pInfo->bufCohe);
            return -1;
        }
    }
#else
    else{
        IMEM_ERR("cBuf ID[0x%x]/size[0x%x]/VA[0x%x]/PA[0x%x]/S[%d]/C[%d]",pInfo->memID,pInfo->size,pInfo->virtAddr,pInfo->phyAddr,pInfo->bufSecu,pInfo->bufCohe);
        return -1;
    }
#endif

#endif

    return 0;
}
//-----------------------------------------------------------------------------
MINT32  IMemDrvImp::mapPhyAddr(
    IMEM_BUF_INFO* pInfo)
{
    stIMEM_MAP_INFO map_info;

    //IMEM_DBG("memID[0x%x]/size[0x%x]/vAddr[0x%x],S/C(%d/%d)",pInfo->memID,pInfo->size,pInfo->virtAddr,pInfo->bufSecu,pInfo->bufCohe);
    IMEM_DRV_DELAY

    //check mapping
    if ( 0 ==  buf_map.count(pInfo->virtAddr) ) {//this judgement actually do nothing, we always do the following operations.
        IMEM_DBG(" NO Mapped");
        //
#if     defined(__ISP_USE_PMEM__)
        //
        //if ( BUF_TYPE_PMEM == pInfo->type ) {
            pInfo->phyAddr = (MUINT32)::pmem_get_phys(pInfo->memID);
        //}

//#elif   defined(__ISP_USE_STD_M4U__) || defined (__ISP_USE_ION__)
#else
        //
        //if ( BUF_TYPE_STD_M4U == pInfo->type ) {
        if ( IMEM_MIN_ION_FD > pInfo->memID ) {
            //
            MUINT32 phyAddr = 0;
            this->allocM4UMemory(pInfo->virtAddr,pInfo->size,&phyAddr,pInfo->memID);
            pInfo->phyAddr = (MUINTPTR)phyAddr;
        }
#if defined(__ISP_USE_ION__)
        else /*if ( BUF_TYPE_ION == pInfo->type )*/  {
            //
            ion_user_handle_t IonHandle;
            MINT32 IonBufFd;
            MINT32 err;

            //struct ion_handle_data handle_data;
            struct ion_custom_data custom_data;
            struct ion_mm_data mm_data;
            struct ion_sys_data sys_data;
            //struct ion_fd_data fd_data;
            //
            //a. get handle of ION_IOC_SHARE from IonBufFd
            IonBufFd = pInfo->memID;
            if(ion_import(
                mIonDrv,
                IonBufFd,
                &IonHandle))
            {
                IMEM_ERR("ion_import fail");
                IMEM_ERR("cBuf ID[0x%x]/size[0x%x]/vAddr[0x%lx],S/C(%d/%d)",pInfo->memID,pInfo->size,(unsigned long)pInfo->virtAddr,pInfo->bufSecu,pInfo->bufCohe);
                return -1;
            }
            //
            mm_data.mm_cmd = ION_MM_CONFIG_BUFFER;
            mm_data.config_buffer_param.handle = IonHandle;//allocation_data.handle;
            ////need to check this one (for different ports?)///
            mm_data.config_buffer_param.eModuleID =M4U_PORT_CAM_IMGI;////seanlin for 6582M4U_CLNTMOD_IMG;    // 1;
            mm_data.config_buffer_param.security = 0;
            mm_data.config_buffer_param.coherent = 0;   //1;
            //mm_data.config_buffer_param.security = pInfo->bufSecu;
            //mm_data.config_buffer_param.coherent = pInfo->bufCohe;   //1;

            err = ion_custom_ioctl(mIonDrv, ION_CMD_MULTIMEDIA, &mm_data);
            if(err == (-ION_ERROR_CONFIG_LOCKED))
            {
                IMEM_DBG("IOCTL[ION_IOC_CUSTOM] Double config after map phy address");
            }
               else if(err != 0)
            {
                IMEM_ERR("IOCTL[ION_IOC_CUSTOM] ION_CMD_MULTIMEDIA Config Buffer failed!");
                IMEM_ERR("cBuf ID[0x%x]/size[0x%x]/vAddr[0x%lx],S/C(%d/%d)",pInfo->memID,pInfo->size,(unsigned long)pInfo->virtAddr,pInfo->bufSecu,pInfo->bufCohe);
                return -1;
            }
            //
            sys_data.sys_cmd = ION_SYS_GET_PHYS;
            sys_data.get_phys_param.handle = IonHandle;  //allocation_data.handle;
            if(ion_custom_ioctl(mIonDrv, ION_CMD_SYSTEM, &sys_data))
            {
                IMEM_ERR("IOCTL[ION_IOC_CUSTOM] Config Buffer failed!");
                IMEM_ERR("cBuf ID[0x%x]/size[0x%x]/vAddr[0x%lx],S/C(%d/%d)",pInfo->memID,pInfo->size,(unsigned long)pInfo->virtAddr,pInfo->bufSecu,pInfo->bufCohe);
                return -1;
            }
            //
            pInfo->phyAddr = sys_data.get_phys_param.phy_addr;
            //free for IMPORT ref cnt
            if(ion_free(
                mIonDrv,
                IonHandle))
            {
                IMEM_ERR("ion_free fail");
                IMEM_ERR("cBuf ID[0x%x]/size[0x%x]/vAddr[0x%lx],S/C(%d/%d)",pInfo->memID,pInfo->size,(unsigned long)pInfo->virtAddr,pInfo->bufSecu,pInfo->bufCohe);
                return -1;
            }
            //
            IMEM_DBG("Physical address=0x%08X len=0x%lx", sys_data.get_phys_param.phy_addr, sys_data.get_phys_param.len);

        }
#else
        else {
            IMEM_ERR("cBuf ID[0x%x]/size[0x%x]/vAddr[0x%x],S/C(%d/%d)",pInfo->memID,pInfo->size,pInfo->virtAddr,pInfo->bufSecu,pInfo->bufCohe);
            return -1;
        }
#endif

#endif
        //
        map_info.pAddr = pInfo->phyAddr;
        map_info.size = pInfo->size;
        ///we do not check whether the mapping relationship of VA/PA exists or not. -> "buf_map" is no more used.
        //ION mapping should be fast enough,so for ION mapping everytime.
        //buf_map.insert(pair<int, stIMEM_MAP_INFO>(pInfo->virtAddr, map_info));


        //buf_map.insert(map<int, int> :: value_type(virtAddr, phyAddr)) ;
        //buf_map[virtAddr] = phyAddr;
    }
    else {
        IMEM_DBG(" Already Mapped");
        map_info = buf_map[pInfo->virtAddr];
        pInfo->phyAddr = map_info.pAddr;
    }
    //
    IMEM_DBG("mID(0x%x),size(0x%x),VA(0x%lx),PA(0x%lx),S/C(%d/%d)",pInfo->memID,pInfo->size,(unsigned long)pInfo->virtAddr,(unsigned long)pInfo->phyAddr,pInfo->bufSecu,pInfo->bufCohe);

    return 0;
}
//-----------------------------------------------------------------------------
MINT32  IMemDrvImp::unmapPhyAddr(
    IMEM_BUF_INFO* pInfo)
{
    stIMEM_MAP_INFO map_info;

    //mapping
#if 0
    if ( 0 !=  buf_map.count(pInfo->virtAddr) ) {
        //
        map_info = buf_map[pInfo->virtAddr];
#else
    if ( 1 ) {
        //
        map_info.size = pInfo->size;
        map_info.pAddr = pInfo->phyAddr;
#endif
        //
        IMEM_DBG("[IMEM_BUFFER] - unmap<memID(0x%x),size(0x%x),virtAddr(0x%lx),phyAddr(0x%lx)>",pInfo->memID,pInfo->size,(unsigned long)pInfo->virtAddr,(unsigned long)pInfo->phyAddr);
        IMEM_DRV_DELAY
        //
#if     defined(__ISP_USE_PMEM__)
        //NOT support for PMEM
        IMEM_DBG("BUF_TYPE_PMEM: free by virtMem free");
#else
        //
        if ( IMEM_MIN_ION_FD > pInfo->memID )
        {//m4u
            IMEM_DBG("BUF_TYPE_STD_M4U");
            IMEM_DRV_DELAY
            pInfo->phyAddr = map_info.pAddr;
            pInfo->size = map_info.size;
            this->freeM4UMemory(pInfo->virtAddr,pInfo->phyAddr,pInfo->size,pInfo->memID);
        }

        else
        {//using ion
            IMEM_DBG("BUF_TYPE_ION: free by virtMem free");
        }
#endif
        //
        buf_map.erase(pInfo->virtAddr);  //actually do nothing.

    }

    return 0;
}

//-----------------------------------------------------------------------------
MINT32  IMemDrvImp::doIonCacheFlush(MINT32 memID,eIONCacheFlushType flushtype)
{
    flushtype;
    #if defined(__ISP_USE_STD_M4U__) || defined (__ISP_USE_ION__)
    ion_user_handle_t IonHandle;
    MINT32 IonBufFd;
    MINT32 err;
    IMEM_DBG("+");
    //a. get handle of ION_IOC_SHARE from IonBufFd and increase handle ref count
    IonBufFd = memID;
    if(ion_import(mIonDrv,IonBufFd,&IonHandle))
    {
        IMEM_ERR("ion_import fail,memID(0x%x)",IonBufFd);
        return -1;
    }

    //b. cache sync by range
    struct ion_sys_data sys_data;
    sys_data.sys_cmd=ION_SYS_CACHE_SYNC;
    sys_data.cache_sync_param.handle=IonHandle;
    sys_data.cache_sync_param.sync_type=ION_CACHE_FLUSH_BY_RANGE;
    if(ion_custom_ioctl(mIonDrv, ION_CMD_SYSTEM,&sys_data))
    {
        IMEM_ERR("CAN NOT DO SYNC, memID/(0x%x)",memID);
           if(ion_free(mIonDrv,IonHandle))
    {
        IMEM_ERR("ion_free fail");
        return -1;
    }
        return -1;
    }

    //c. decrease handle ref count
    if(ion_free(mIonDrv,IonHandle))
    {
        IMEM_ERR("ion_free fail");
        return -1;
    }
    IMEM_DBG("-");
    #endif
    return 0;
}

//-----------------------------------------------------------------------------
MINT32 IMemDrvImp::cacheFlushAll(
    void)
{

#if defined(__ISP_USE_STD_M4U__) || defined (__ISP_USE_ION__)
    IMEM_DBG("BUF_TYPE_STD_M4U || __ISP_USE_ION__");
    IMEM_DRV_DELAY
    ////need to check this one (for different ports?)///
    ///it seems that doing cache_flush_all is not relative to the input module id. (ref to m4u_lib.cpp)
    mpM4UDrv->m4u_cache_flush_all(M4U_PORT_CAM_IMGI);////seanlin for 6582 M4U_CLNTMOD_IMG);
#endif

#if 0 //defined (__ISP_USE_ION__)
    IMEM_DBG("BUF_TYPE_ION");
    IMEM_DRV_DELAY

    struct ion_custom_data custom_data;
    struct ion_sys_data sys_data;
    //
    struct ion_fd_data fd_data;
    //
    //a. get handle of ION_IOC_SHARE from fd_data.fd
    fd_data.fd = pInfo->memID;
    if (ioctl(mIonDrv, ION_IOC_IMPORT, &fd_data))
    {
        IMEM_ERR("IOCTL[ION_IOC_FREE] failed!");
        return 0;
    }
    //free for IMPORT ref cnt
    handle_data.handle = fd_data.handle;
    if (ioctl(mIonDrv, ION_IOC_FREE, &handle_data))
    {
        IMEM_ERR("IOCTL[ION_IOC_FREE] failed!");
        return -1;
    }

    //
    custom_data.cmd = ION_CMD_SYSTEM;
    custom_data.arg = &sys_data;
    sys_data.sys_cmd = ION_SYS_CACHE_SYNC;
    sys_data.cache_sync_param.handle = allocation_data.handle;
    sys_data.cache_sync_param.sync_type = ION_CACHE_FLUSH_ALL;
    if (ioctl(mIonDrv, ION_IOC_CUSTOM, &custom_data))
    {
        IMEM_ERR("IOCTL[ION_IOC_CUSTOM] Cache sync failed!\n");
        return 0;
    }
#endif


    return 0;
}


//-----------------------------------------------------------------------------
MINT32 IMemDrvImp::cacheSyncbyRange(IMEM_CACHECTRL_ENUM ctrl,IMEM_BUF_INFO* pInfo)
{
#if defined(__ISP_USE_STD_M4U__) || defined (__ISP_USE_ION__)
    MINT32 err=0;
    IMEM_DBG("+, c/m/va/sz/pa(%d/0x%x/0x%lx/0x%x/0x%lx)",ctrl,pInfo->memID,(unsigned long)pInfo->virtAddr,pInfo->size,(unsigned long)pInfo->phyAddr);

    if( IMEM_MIN_ION_FD > pInfo->memID)
    {
        switch(ctrl)
        {
            case IMEM_CACHECTRL_ENUM_FLUSH:
                mpM4UDrv->m4u_cache_sync(M4U_PORT_CAM_IMGI,M4U_CACHE_FLUSH_BEFORE_HW_WRITE_MEM,pInfo->virtAddr,pInfo->size);
                break;
            case IMEM_CACHECTRL_ENUM_INVALID:
                mpM4UDrv->m4u_cache_sync(M4U_PORT_CAM_IMGI,M4U_CACHE_INVALID_AFTER_HW_WRITE_MEM,pInfo->virtAddr,pInfo->size);
                break;
            default:
                LOG_ERR("ERR cmd(%d)",ctrl);
                break;
       }
    }
    else
    {
        //a. get handle of ION_IOC_SHARE from IonBufFd and increase handle ref count
        ion_user_handle_t IonHandle;
        MINT32 IonBufFd;
        IonBufFd = pInfo->memID;
        if(ion_import(mIonDrv,IonBufFd,&IonHandle))
        {
            IMEM_ERR("ion_import fail");
            return -1;
        }

        //b. cache sync by range
        struct ion_sys_data sys_data;
        sys_data.sys_cmd=ION_SYS_CACHE_SYNC;
        sys_data.cache_sync_param.handle=IonHandle;
        sys_data.cache_sync_param.va        = 0;
        sys_data.cache_sync_param.size      = 0;
        
        switch(ctrl)
        {
            case IMEM_CACHECTRL_ENUM_FLUSH:
                sys_data.cache_sync_param.sync_type=ION_CACHE_FLUSH_BY_RANGE;
                break;
            case IMEM_CACHECTRL_ENUM_INVALID:
                sys_data.cache_sync_param.sync_type=ION_CACHE_INVALID_BY_RANGE;
                break;
            default:
                LOG_ERR("ERR cmd(%d)",ctrl);
                break;
        }
        if(ion_custom_ioctl(mIonDrv, ION_CMD_SYSTEM,&sys_data))
        {
            IMEM_ERR("CAN NOT DO SYNC, memID/(0x%x)",pInfo->memID);
            return -1;
        }

        //c. decrease handle ref count
        if(ion_free(mIonDrv,IonHandle))
        {
                IMEM_ERR("ion_free fail");
                return -1;
        }
    }
    IMEM_DBG("-");
    return err;
#endif
    return 0;
}


//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
#if defined (__ISP_USE_STD_M4U__) || defined (__ISP_USE_ION__)
//m4u
MINT32 IMemDrvImp::allocM4UMemory(
    MUINTPTR     virtAddr,
    MUINT32     size,
    MUINT32*    m4uVa,
    MINT32      memID)
{
    MINT32 ret = 0;
    //
    if(mpM4UDrv == NULL)
    {
        IMEM_DBG("Null M4U driver");
        return -1;
    }
    //

    IMEM_DRV_DELAY
    //
#if(ISP_HAL_PROFILE)
    MINT32 startTime = 0, endTime = 0;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    startTime = tv.tv_sec * 1000000 + tv.tv_usec;
#endif
    //
    ////need to check this one (for different ports?)///
    //ref to m4u.c(MTK_M4U_ioctl), the module id is just used in MMProfileLog
    M4U_MODULE_ID_ENUM eM4U_ID =M4U_PORT_CAM_IMGI;////seanlin for 6582M4U_CLNTMOD_IMG;

#if 0 //re-open by demand. k.zhang
    //ret = mpM4UDrv->m4u_power_on(eM4U_ID);
    ret = mpM4UDrv->m4u_enable_m4u_func(eM4U_ID);
    if(ret != M4U_STATUS_OK)
    {
        IMEM_ERR("m4u_enable_m4u_func fail");
        goto EXIT;
    }
#endif
    //
    ret = mpM4UDrv->m4u_alloc_mva(
            eM4U_ID,
            virtAddr,
            size,
            0,
            0, // 1: auto coherence
            m4uVa);
    if(ret != M4U_STATUS_OK)
    {
        IMEM_ERR("m4u_alloc_mva fail:[%d]",ret);
        goto EXIT;
    }
    //set lock to fals, due to camera only use memory once ---> K@121212 no need
    ret = mpM4UDrv->m4u_manual_insert_entry(
            eM4U_ID,
            *m4uVa,
            MFALSE);
    if(ret != M4U_STATUS_OK)
    {
        IMEM_ERR("m4u_manual_insert_entry fail:[%d]",ret);
        goto EXIT;
    }
    //
    ret = mpM4UDrv->m4u_insert_tlb_range(
            eM4U_ID,
            *m4uVa,
            *m4uVa + size -1,
            RT_RANGE_HIGH_PRIORITY,
            1);
    if(ret != M4U_STATUS_OK)
    {
        IMEM_ERR("m4u_insert_tlb_range fail:[%d]",ret);
        goto EXIT;
    }
    //
    IMEM_DBG("m4uVa(0x%x)", *m4uVa);
    //
    IMEM_DBG("M4U Flush(0x%lx)",(unsigned long)virtAddr);
    IMEM_DRV_DELAY
    mpM4UDrv->m4u_cache_sync(eM4U_ID,
                            M4U_CACHE_FLUSH_BEFORE_HW_READ_MEM,
                            virtAddr,
                            size);
    //
#if(ISP_HAL_PROFILE)
    gettimeofday(&tv, NULL);
    endTime = tv.tv_sec * 1000000 + tv.tv_usec;
    IMEM_DBG("profile time(%d) ms", (endTime - startTime) / 1000);
#endif
    //
    EXIT:
    IMEM_INF("[StdM4U]VA(0x%lx)/PA(0x%x)/size(0x%x)/mID(0x%x)", (unsigned long)virtAddr, *m4uVa, size, memID);
    return ret;

}
//-----------------------------------------------------------------------------
MINT32 IMemDrvImp::freeM4UMemory(
    MUINTPTR     virtAddr,
    MUINT32     m4uVa,
    MUINT32     size,
    MINT32      memID)
{
    MINT32 ret = 0;
    //
    if(mpM4UDrv == NULL)
    {
        IMEM_DBG("Null M4U driver");
        return -1;
    }
    //
    IMEM_DBG("va(0x%lx)/m4uVa(0x%lx)/size(%d)", (unsigned long)virtAddr, (unsigned long)m4uVa, size);
    IMEM_DRV_DELAY
    //
#if(ISP_HAL_PROFILE)
    MINT32 startTime = 0, endTime = 0;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    startTime = tv.tv_sec * 1000000 + tv.tv_usec;
#endif
    //
    ////need to check this one (for different ports?)///
    ret = mpM4UDrv->m4u_invalid_tlb_range(
           M4U_PORT_CAM_IMGI,////seanlin for 6582M4U_CLNTMOD_IMG,
            m4uVa,
            m4uVa + size-1);
    ret = mpM4UDrv->m4u_dealloc_mva(
           M4U_PORT_CAM_IMGI,//seanlin for 6582M4U_CLNTMOD_IMG,
            virtAddr,
            size,
            m4uVa);
    //
#if(ISP_HAL_PROFILE)
    gettimeofday(&tv, NULL);
    endTime = tv.tv_sec * 1000000 + tv.tv_usec;
    IMEM_DBG("profile time(%d) ms", (endTime - startTime) / 1000);
#endif

#if 0
    //
    //ret = mpM4UDrv->m4u_power_off(M4U_CLNTMOD_IMG);
    ret = mpM4UDrv->m4u_disable_m4u_func(M4U_CLNTMOD_IMG);

    if(ret != M4U_STATUS_OK)
    {
        IMEM_ERR("m4u_power_on fail");
    }
#endif

    //
    IMEM_INF("[StdM4U]VA(0x%lx)/PA(0x%lx)/size(0x%x)/mID(0x%x)", (unsigned long)virtAddr, (unsigned long)m4uVa, size, memID);
    return ret;

}

#endif //#if defined (__ISP_USE_STD_M4U__) || defined (__ISP_USE_ION__)

//-----------------------------------------------------------------------------


