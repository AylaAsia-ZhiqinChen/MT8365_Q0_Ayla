/*
 * Copyright (C) 2018 MediaTek Inc.
 * All rights reserved
 *
 * The present software is the confidential and proprietary information of
 * Mediatek Inc. You shall not disclose the present software and shall
 * use it only in accordance with the terms of the license agreement you
 * entered into with MediaTek Inc. This software may be subject to
 * export or import laws in certain countries.
 */
#define _GNU_SOURCE
#include <fcntl.h>
#include <linux/mtk_ion.h>
#include <linux/ion.h>
#include <log/log.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <uree/system.h>
#include <uree/mem.h>
#include <uree/dbg.h>
#include <unistd.h>

#include "isp_ca.h"

#define ISP_CA_PORTING 1

#define ISPCA_LOGD(fmt, arg...)        do { if (1) { ALOGW(fmt, ##arg); } } while(0)
#define ISPCA_LOGI(fmt, arg...)        do { if (1) { ALOGW(fmt, ##arg); } } while(0)
#define ISPCA_LOGW(fmt, arg...)        do { if (1) { ALOGW(fmt, ##arg); } } while(0)
#define ISPCA_LOGE(fmt, arg...)        do { if (1) { ALOGE(fmt, ##arg); } } while(0)

#define ATRACE_TAG ATRACE_TAG_CAMERA

#define LOG_TAG "ISP_CA"
#define ISPCA_UUID { 0x85f630e0 , 0xf096, 0x4c5f, { 0xa2, 0xcc, 0x26, 0x8c, 0xe0, 0x4e, 0x3d, 0xa3 }}
#define ISP_ATRACE_CALL() {}

#define ISP_FALSE 0
#define ISP_TRUE 1

typedef struct
{
    UREE_SHAREDMEM_HANDLE sharedHandle;
    UREE_SHAREDMEM_PARAM sharedParam;
    uint64_t buff_va;
    uint32_t size;
} IspShareMemEntry_t;

typedef struct
{
    IspShareMemEntry_t cq_despt_tab[CQ_DESPT_L0_NUM][CQ_DESPT_L1_NUM];
    IspShareMemEntry_t vir_reg_tab[VIR_REG_L0_NUM][VIR_REG_L1_NUM];
    IspShareMemEntry_t frm_header_tab[FRM_HEADER_L0_NUM][FRM_HEADER_L1_NUM];
    IspShareMemEntry_t lsc_tab[LSC_L0_NUM][LSC_L1_NUM];
    IspShareMemEntry_t bpc_tab[BPC_L0_NUM][BPC_L1_NUM];
} IspShareMemTab_t;

typedef struct
{
    UREE_SESSION_HANDLE session;
    IspShareMemTab_t IspShareMemTab[CAM_NUM];
} IspHandle;

void* gISPHandle = 0;
bool gSecISPInit = 0;
UREE_SESSION_HANDLE gIspMemSession = 0;

static void allocate_secure_mem(void)
{

}

static void free_secure_mem(void)
{

}

static int _getEntrySize(int sec_type)
{
    switch (sec_type) {
    case SECMEM_CQ_DESCRIPTOR_TABLE:
    case SECMEM_VIRTUAL_REG_TABLE:
    case SECMEM_LSC:
    case SECMEM_BPC:
        return -1;
    default:
        ISPCA_LOGI("%s, Invalid table type", __FUNCTION__);
        return -1;
    }
}

static bool _useSameMemID(int sec_type)
{
    switch (sec_type) {
    case SECMEM_CQ_DESCRIPTOR_TABLE:
    case SECMEM_VIRTUAL_REG_TABLE:
    case SECMEM_LSC:
    case SECMEM_BPC:
        return ISP_FALSE;
    default:
        ISPCA_LOGI("%s, Invalid table type", __FUNCTION__);
        return ISP_FALSE;
    }
}


unsigned long MtkSecISP_RegisterSharedMemory(void* buffer, int size) {
    TZ_RESULT ret;
    UREE_SHAREDMEM_HANDLE sharedHandle = 0;
    UREE_SHAREDMEM_PARAM  sharedParam;

    if(gIspMemSession == 0){
        ISPCA_LOGE("%s gIspMemSession is NULL", __FUNCTION__);
        return 0;
    }

    sharedParam.buffer = buffer;
    sharedParam.size = size;
    ret = UREE_RegisterSharedmemWithTag(gIspMemSession, &sharedHandle, &sharedParam, "ISP regShareMem");

    if (ret != TZ_RESULT_SUCCESS)
    {
        ISPCA_LOGE("UREE_RegisterSharedmemWithTag Error: %d, line:%d, handle:%d", ret, __LINE__, sharedHandle);
    }
    else
    {
        ISPCA_LOGD("UREE_RegisterSharedmemWithTag: %d, line:%d, handle:%d", ret, __LINE__, sharedHandle);
    }
    return sharedHandle;
}


int MtkSecISP_ReleaseSharedMemory(unsigned long sharedHandle)
{
    TZ_RESULT ret;

    if(gIspMemSession == 0){
        ISPCA_LOGE("%s gIspMemSession is NULL", __FUNCTION__);
        return ISP_FALSE;
    }
    ret = UREE_UnregisterSharedmem(gIspMemSession, sharedHandle);

    if (ret != TZ_RESULT_SUCCESS)
    {
        ISPCA_LOGE("UREE_UnregisterSharedmem Error: %d, line:%d", ret, __LINE__);
        return ISP_FALSE;
    }else {
        ISPCA_LOGD("UREE_UnregisterSharedmem: %d, line:%d,sharedHandle(%d)", ret, __LINE__,sharedHandle);
    }
    return ISP_TRUE;
}


static int MtkSecISP_isInit(void* handle)
{
    if (handle && gISPHandle)
        return ISP_TRUE;
    return ISP_FALSE;
}

uint64_t MtkSecISP_tlcHandleCreate(void)
{
    ISPCA_LOGI("%s +", __FUNCTION__);

    if(gISPHandle == 0)
    {
        gISPHandle = malloc(sizeof(IspHandle));
        if(gISPHandle){
            memset(gISPHandle, 0, sizeof(IspHandle));
            allocate_secure_mem();
            ISPCA_LOGI("First init. gISPHandle(0x%p)", gISPHandle);
        }
    }

    return (uint64_t)gISPHandle;
}

int MtkSecISP_tlcOpen(uint64_t handle)     //MTEE_ISPSessionCreate
{
    
    ISP_ATRACE_CALL();
    ISPCA_LOGI("%s +", __FUNCTION__);
    TZ_RESULT result;
    uint32_t err_origin = 0;
    IspHandle *pHandle = (IspHandle*)gISPHandle;

    if (!gISPHandle)
    {
        ISPCA_LOGE("%s handle is NULL", __FUNCTION__);
        return ISP_FALSE;
    }

    result = UREE_CreateSession(TZ_TA_ISP_UUID, &(pHandle->session));
    if (result != TZ_RESULT_SUCCESS) {
        ISPCA_LOGE("UREE_CreateSession Error: 0x%x, %d\n", (uint32_t) pHandle->session, result);
        return ISP_FALSE;
    }
    result = UREE_CreateSession(TZ_TA_MEM_UUID, &gIspMemSession);
    if (result != TZ_RESULT_SUCCESS)
    {
        ISPCA_LOGE ("Mem CreateSession gIspMemSession Error: %d, line:%d", result, __LINE__);
        if(0 != pHandle->session)
        {
            UREE_CloseSession(pHandle->session);
            pHandle->session = 0;
        }
        return ISP_FALSE;
    }

    ISPCA_LOGD("UREE_CreateSession finished!");
    ISPCA_LOGI("%s -", __FUNCTION__);
    return ISP_TRUE;

}

int MtkSecISP_tlcClose(uint64_t ispHandle){    //MTEE_ISPSessionClose
    ISP_ATRACE_CALL();
    ISPCA_LOGI("%s +", __FUNCTION__);
    TZ_RESULT ret;

    if (!ispHandle)
    {
        ISPCA_LOGE("%s handle is NULL", __FUNCTION__);
        return ISP_FALSE;
    }

    IspHandle *pHandle = (IspHandle*)ispHandle;
    if(0 != pHandle->session)
    {
        ret = UREE_CloseSession(pHandle->session);
        pHandle->session = 0;
    }
    if (ret != TZ_RESULT_SUCCESS)
    {
        ISPCA_LOGE("UREE_CloseSession Error: %d, line:%d", ret, __LINE__);
    }
    if(gIspMemSession != 0)
    {
        ret = UREE_CloseSession(gIspMemSession);
    }
    if (ret != TZ_RESULT_SUCCESS)
    {
        ISPCA_LOGE("UREE_CloseSession Error: %d, line:%d", ret, __LINE__);
    }

    free(gISPHandle);
    gISPHandle = NULL;
    ISPCA_LOGI("%s -", __FUNCTION__);
    return ISP_TRUE; 

}

#define MTK_M4U_REINIT 6
#define MTK_M4U_MAGICNO 'g'
#define MTK_M4U_T_SEC_INIT _IOW(MTK_M4U_MAGICNO, 50, int)
static int MtkSecISP_m4uSecInit()
{
#if 0   // ISP_CA_PORTING
    int m4u_fd, ret;

    m4u_fd = open("/proc/m4u", O_RDONLY);
    if (m4u_fd == -1)
    {
        LOGE("open /proc/m4u failed! errno=%d, %s\n", errno, strerror(errno));
        goto FAIL;
    }

    ISPCA_LOGI("%s +", __FUNCTION__);
    ret = ioctl(m4u_fd, MTK_M4U_T_SEC_INIT, NULL);
    if (ret)
    {
        if (ret == -MTK_M4U_REINIT) {
	        ISPCA_LOGI("m4u has been intialized\n");
        } else {
//            LOGE("m4u ioctl sec_init failed! errno=%d, %s\n", errno, strerror(errno));
            goto FAIL;
        }
    }
    else
    {
        ISPCA_LOGI("m4u sec_init sucess\n");
    }
    ISPCA_LOGI("%s -", __FUNCTION__);

    close(m4u_fd);
    return 0;

FAIL:
    close(m4u_fd);
#endif
    return -1;
}

int MtkSecISP_tlcInit(uint64_t ispHandle){
    MTEEC_PARAM param[4];
    TZ_RESULT ret;
    IspHandle *pHandle = (IspHandle*)ispHandle;
    if (!ispHandle)
    {
        ISPCA_LOGE("%s handle is NULL", __func__);
        return ISP_FALSE;
    }
    if (gSecISPInit)
    {
        ISPCA_LOGE("%s has been initialized", __FUNCTION__);
        return ISP_FALSE;
    }    

#if 0 //ISP_CA_PORTING
    ISPCA_LOGI("%s +", __FUNCTION__);
    result = MtkSecISP_m4uSecInit();
    if (result < 0)
        return result;
#endif
    ISPCA_LOGI("%s run TZCMD_ISP_INIT", __FUNCTION__);

    ret = UREE_TeeServiceCall(pHandle->session, TZCMD_ISP_INIT, 0, param);
    if (ret != TZ_RESULT_SUCCESS) {
        ISPCA_LOGE("MtkSecISP_tlcInit Error: %s\n", TZ_GetErrorString(ret));
        return ISP_FALSE;
    }
    gSecISPInit = 1;
    ISPCA_LOGI("%s -", __FUNCTION__);    
    return ISP_TRUE;

}

static int _relaseTabShareMem(void* handle, int sec_type, int cam_id)
{
    IspHandle *pHandle = (IspHandle*)handle;
    IspShareMemTab_t *shareMemTabs;
    IspShareMemEntry_t *ispTabEntry;
    int maxL0, maxL1, i, j, size;

    shareMemTabs = &pHandle->IspShareMemTab[cam_id];
    switch (sec_type) {
    case SECMEM_CQ_DESCRIPTOR_TABLE:
        size = CQ_DESPT_L0_NUM * CQ_DESPT_L1_NUM;
        ispTabEntry = &shareMemTabs->cq_despt_tab[0][0];
        break;
    case SECMEM_VIRTUAL_REG_TABLE:
        size = VIR_REG_L0_NUM * VIR_REG_L1_NUM;
        ispTabEntry = &shareMemTabs->vir_reg_tab[0][0];
        break;
    case SECMEM_LSC:
        size = LSC_L0_NUM * LSC_L1_NUM;
        ispTabEntry = &shareMemTabs->lsc_tab[0][0];
        break;
    case SECMEM_BPC:
        size = BPC_L0_NUM * BPC_L1_NUM;
        ispTabEntry = &shareMemTabs->bpc_tab[0][0];
        break;
    default:
        ISPCA_LOGI("%s, Invalid table type", __FUNCTION__);
        return ISP_FALSE;
    }

    for (i=0 ; i<size ; i++)
        if(ispTabEntry[i].sharedHandle != 0)
            if (!_useSameMemID(sec_type) || i == 0)
                MtkSecISP_ReleaseSharedMemory(ispTabEntry[i].sharedHandle);

    return ISP_TRUE;
}

static int relaseAllTabShareMem(void* handle)
{
    int sec_type, cam_id;

    ISPCA_LOGI("%s +", __FUNCTION__);
    for (sec_type=0 ; sec_type<SECMEM_MAX ; sec_type++)
        for (cam_id=0 ; cam_id<CAM_NUM ; cam_id++)
            _relaseTabShareMem(handle, sec_type, cam_id);
    ISPCA_LOGI("%s -", __FUNCTION__);

    return ISP_TRUE;
}

int MtkSecISP_tlcUnInit(uint64_t handle)
{
    TZ_RESULT ret;
    IspHandle *pHandle = (IspHandle*)handle;
    uint32_t paramTypes;
    MTEEC_PARAM params[4];

    ISP_ATRACE_CALL();
    if (!handle)
    {
        ISPCA_LOGE("%s handle is NULL", __FUNCTION__);
        return ISP_FALSE;
    }
    if (!gSecISPInit)
    {
        ISPCA_LOGE("%s has been uninitialized", __FUNCTION__);
        return ISP_FALSE;
    }

    paramTypes = TZ_ParamTypes(
        TZPT_VALUE_INPUT,
        TZPT_NONE,
        TZPT_NONE,
        TZPT_NONE);
        
    ISPCA_LOGI("%s +", __FUNCTION__);
    ret = UREE_TeeServiceCall(pHandle->session, TZCMD_ISP_UNINIT, 0, params);

    if(ret != TZ_RESULT_SUCCESS)
    {
        ISPCA_LOGE("Invoke Uninit failed (0x%08x)", ret);
        return ISP_FALSE;
    }

    relaseAllTabShareMem((void*)handle);
    gSecISPInit = 0;
    ISPCA_LOGI("%s -", __FUNCTION__);
    return ISP_TRUE;
}

int MtkSecISP_tlcSecConfig(uint64_t ispHandle, int port, int tg_idx)
{
    ISPCA_LOGI("%s +", __FUNCTION__);
    TZ_RESULT ret;
    TZ_RESULT result;
    IspHandle *pHandle = (IspHandle*)ispHandle;
    uint32_t paramTypes;
    MTEEC_PARAM params[4];


    if (!pHandle)
    {
        ISPCA_LOGE("%s handle is NULL", __FUNCTION__);
        return ISP_FALSE;
    }

    paramTypes = TZ_ParamTypes(
        TZPT_VALUE_INPUT,
        TZPT_NONE,
        TZPT_NONE,
        TZPT_NONE);

    params[0].value.a = port;
    ret = UREE_TeeServiceCall(pHandle->session, TZCMD_ISP_SECCONFIG, paramTypes, params);
    if(ret != TZ_RESULT_SUCCESS)
    {
        ISPCA_LOGE("Invoke SecConfig failed (0x%08x)", ret);
        return ret;
    }

    ISPCA_LOGI("%s -", __FUNCTION__);
    return ISP_TRUE;
}


int MtkSecISP_tlcQueryMVAFromHandle(uint64_t handle, uint64_t sec_handle, uint64_t _pa,
	MEM_TYPE mem_type)
{
    TZ_RESULT ret;
    IspHandle *pHandle = (IspHandle*)handle;
    uint32_t paramTypes;
    MTEEC_PARAM params[4];
    uint32_t* pa = (uint32_t*)_pa;

    ISP_ATRACE_CALL();
    if (!handle)
    {
        ISPCA_LOGE("%s handle is NULL", __FUNCTION__);
        return ISP_FALSE;
    }

    paramTypes = TZ_ParamTypes(
        TZPT_VALUE_INPUT,
        TZPT_VALUE_INPUT,
        TZPT_VALUE_OUTPUT,
        TZPT_NONE);

    params[0].value.a = sec_handle;
    params[0].value.b = sec_handle>>32;
    params[1].value.a = mem_type;

    ISPCA_LOGD("%s handle:0x%p, sec_handle:0x%lx, mem_type:%d, pa:0x%x",
        __FUNCTION__, pHandle, sec_handle, mem_type, params[2].value.a);

    ret = UREE_TeeServiceCall(pHandle->session, TZCMD_ISP_QUERYMVA, paramTypes, params);
    if(ret != TZ_RESULT_SUCCESS)
    {
        ISPCA_LOGE("Invoke QueryMVA failed (0x%08x)", ret);
        return ret;
    }
    
    *pa = params[2].value.a;

    return ISP_TRUE;
}

static bool MtkSecISP_isTabIndexValid(SecMgr_SecInfo *secInfo)
{
    int tabMaxIndex = 0, level0 = 0, level1 = 0;

    switch (secInfo->type) {
    case SECMEM_CQ_DESCRIPTOR_TABLE:
        tabMaxIndex = TAB_INDEX_CQ_DESCRIPTOR;
        if (secInfo->dummy)
            level0 = CQ_DESPT_L0_NUM - 1;
        else
            level0 = secInfo->cq;
        level1 = secInfo->dupq;
        break;
    case SECMEM_VIRTUAL_REG_TABLE:
        tabMaxIndex = TAB_INDEX_VIRTUAL_REG;
        if (secInfo->dummy)
            level0 = VIR_REG_L0_NUM - 1;
        else
            level0 = secInfo->cq;
        level1 = secInfo->dupq;
        break;
    case SECMEM_LSC:
    case SECMEM_BPC:
        return ISP_TRUE;
    default:
        ISPCA_LOGI("%s, Invalid table type", __FUNCTION__);
        return ISP_FALSE;
    }

    if (secInfo->module < CAM_NUM &&
        secInfo->type < SECMEM_MAX &&
        level0 < GET_TAB_INDEX(tabMaxIndex, 0) &&
        level1 < GET_TAB_INDEX(tabMaxIndex, 1))
        return ISP_TRUE;

    ISPCA_LOGD("%s, tabIndex:0x:%x", __FUNCTION__, tabMaxIndex);
    ISPCA_LOGD("%s, Invalid input table index. cam_id;%d, tb_type:%d, L0:%d, L1:%d, tabMaxIndex:0x%x",
        __FUNCTION__, secInfo->module, secInfo->type,
        level0, level1, tabMaxIndex);

    return ISP_FALSE;
}

static bool _convertSecInfoToTabIndex(IspHandle *pHandle, SecMgr_SecInfo *secInfo,
	struct ISPSecTabIndex_t *tabIndex)
{
    int i;
    uint64_t buff_va;
    IspShareMemTab_t *shareMemTabs;
    IspShareMemEntry_t *ispTabEntry;

    tabIndex->cam_id = secInfo->module;
    tabIndex->tb_type = secInfo->type;
    switch (tabIndex->tb_type) {
    case SECMEM_CQ_DESCRIPTOR_TABLE:
        if (secInfo->dummy)
            tabIndex->level0 = CQ_DESPT_L0_NUM - 1;
        else
            tabIndex->level0 = secInfo->cq;
        tabIndex->level1 = secInfo->dupq;
        break;
    case SECMEM_VIRTUAL_REG_TABLE:
        if (secInfo->dummy)
            tabIndex->level0 = VIR_REG_L0_NUM - 1;
        else
            tabIndex->level0 = secInfo->cq;
        tabIndex->level1 = secInfo->dupq;
        break;
    case SECMEM_LSC:
        tabIndex->level0 = 0;
        shareMemTabs = &pHandle->IspShareMemTab[secInfo->module];
        for (i = 0 ; i < LSC_L1_NUM ; i++) {
            ispTabEntry = &shareMemTabs->lsc_tab[tabIndex->level0][i];
            buff_va = (uint64_t)ispTabEntry->sharedParam.buffer;
            if (buff_va == secInfo->buff_va)
            {
                tabIndex->level1 = i;
                return ISP_TRUE;
            }
        }
//        LOGE("Cannot find LSC Entry. buf_va:0x%llx, port:%d",
  //          secInfo->buff_va, secInfo->port);
        return ISP_FALSE;
    case SECMEM_BPC:
        tabIndex->level0 = 0;
        tabIndex->level1 = 0;
        break;
    default:
        ISPCA_LOGI("%s, Invalid table type", __FUNCTION__);
        return ISP_FALSE;
    }
    return ISP_TRUE;
}


static IspShareMemEntry_t *MtkSecISP_getTabEntryByIdx(IspHandle *pHandle,
	struct ISPSecTabIndex_t *tb_idx)
{
    IspShareMemTab_t *shareMemTabs;

    shareMemTabs = &pHandle->IspShareMemTab[tb_idx->cam_id];
    switch (tb_idx->tb_type)
    {
    case SECMEM_CQ_DESCRIPTOR_TABLE:
        return &shareMemTabs->cq_despt_tab[tb_idx->level0][tb_idx->level1];
    case SECMEM_VIRTUAL_REG_TABLE:
        return &shareMemTabs->vir_reg_tab[tb_idx->level0][tb_idx->level1];
    case SECMEM_LSC:
        return &shareMemTabs->lsc_tab[tb_idx->level0][tb_idx->level1];
    default:
        return NULL;
    }
}

static bool _getEmptyTabEntry(IspHandle *pHandle, SecMgr_SecInfo *secInfo,
	struct ISPSecTabIndex_t *tabIndex)
{
    int level0, level1, i;
    uint64_t buff_va;
    IspShareMemTab_t *shareMemTabs;
    IspShareMemEntry_t *ispTabEntry;

    if (secInfo->type == SECMEM_FRAME_HEADER)
    {
        ISPCA_LOGE("not supported");
        return ISP_FALSE;
    } else if (secInfo->type == SECMEM_LSC) {
        tabIndex->cam_id = secInfo->module;
        tabIndex->tb_type = secInfo->type;
        tabIndex->level0 = 0;
        shareMemTabs = &pHandle->IspShareMemTab[secInfo->module];
        for (i = 0 ; i < LSC_L1_NUM ; i++) {
            ispTabEntry = &shareMemTabs->lsc_tab[tabIndex->level0][i];
            buff_va = (uint64_t)ispTabEntry->sharedParam.buffer;
            if (buff_va == secInfo->buff_va)
                return false;
            if (buff_va == 0)
            {
                tabIndex->level1 = i;
                return ISP_TRUE;
            }
        }
        return ISP_FALSE;
    }
    _convertSecInfoToTabIndex(pHandle, secInfo, tabIndex);
    ispTabEntry = MtkSecISP_getTabEntryByIdx(pHandle, tabIndex);
    if (ispTabEntry == NULL)
    {
        ISPCA_LOGE("get Tab Entry fail");
        return ISP_FALSE;
    }

    if (ispTabEntry->sharedHandle != 0)
    {
        ISPCA_LOGI("The share memory has been registered already.");
        return ISP_FALSE;
    }

    return ISP_TRUE;
}

static int _registerEntryShareMem(IspHandle *pHandle, IspShareMemEntry_t *ispEntry,
	SecMgr_SecInfo *secInfo, int buff_size, int isSub)
{
    TZ_RESULT ret;   
    if (isSub) {
        ispEntry->sharedParam.buffer = (void *)secInfo->sub.buff_va;
    } else{
        ispEntry->sharedParam.buffer = (void *)secInfo->buff_va;        
    }
    ispEntry->sharedParam.size = buff_size;
    ret = UREE_RegisterSharedmemWithTag(gIspMemSession, &(ispEntry->sharedHandle), &(ispEntry->sharedParam), "ISP regShareMem");

    if (ret != TZ_RESULT_SUCCESS || ispEntry->sharedHandle == 0)
    {
        ISPCA_LOGE("UREE_RegisterSharedmemWithTag Error: %d, va:%llx va_org:%llx size:%d Handle(%d)",
            ret, ispEntry->sharedParam.buffer, secInfo->buff_va, buff_size, ispEntry->sharedHandle);
    }else {
        ISPCA_LOGD("UREE_RegisterSharedmemWithTag: %d, va:%llx va_org:%llx size:%d Handle(%d)",
            ret, ispEntry->sharedParam.buffer, secInfo->buff_va, buff_size, ispEntry->sharedHandle);
    }
    
    return ret;
}

static int _tlcHasSubInfo(struct ISPSecTabIndex_t *tabIndex)
{
    if (tabIndex->tb_type == SECMEM_CQ_DESCRIPTOR_TABLE && tabIndex->level0 == 0)
        return ISP_TRUE;
    return ISP_FALSE;
}

int MtkSecISP_tlcRegisterShareMem(uint64_t ispHandle, uint64_t _secInfo)
{
    IspHandle *pHandle = (IspHandle*)ispHandle;
    uint32_t paramTypes;
    MTEEC_PARAM params[4];
    TZ_RESULT ret = 0;
    IspShareMemEntry_t *ispEntry;
    IspShareMemEntry_t *ispSubEntry;
    struct ISPSecTabIndex_t tabIndex;
    ISPCA_LOGD("%s+, secInfo:%llx\n",
        __FUNCTION__,_secInfo);
    SecMgr_SecInfo *secInfo = (SecMgr_SecInfo*)_secInfo;

    ISP_ATRACE_CALL();
    ISPCA_LOGD("%s+, type:%d buff_size:%d, buff_va:0x%llx sizeof(buff_va):%d, port:%d, cq:%d, dupq:%d, module:%d, memID:%d\n",
        __FUNCTION__, secInfo->type, secInfo->buff_size, secInfo->buff_va, sizeof(secInfo->buff_va),
        secInfo->port, secInfo->cq, secInfo->dupq, secInfo->module, secInfo->memID);
    if (MtkSecISP_isInit((void*)pHandle) == ISP_FALSE)
    {
        ISPCA_LOGE("%s, SecISP is not initialized. ispHandle:0x%p, gSecIspInit:%d",
            __FUNCTION__, pHandle, gSecISPInit);
        return ISP_FALSE;
    }
    if (MtkSecISP_isTabIndexValid(secInfo) == ISP_FALSE)
        return ISP_FALSE;
    if (!_getEmptyTabEntry(pHandle, secInfo, &tabIndex))
    {
        ISPCA_LOGI("The share memory has been registered already.");
        return ISP_FALSE;
    }

    ispEntry = MtkSecISP_getTabEntryByIdx(pHandle, &tabIndex);
	if (ispSubEntry == NULL)
	{
		ISPCA_LOGE("get Tab Entry fail");
		return ISP_FALSE;
	}
    if (ispEntry->buff_va != 0)
    {
        ISPCA_LOGI("Target buff_va is not empty.");
        return ISP_FALSE;
    }

    ispEntry->buff_va = (uint64_t)secInfo->buff_va;
    if (secInfo->type == SECMEM_FRAME_HEADER)
    {
        ISPCA_LOGE("not supported\n");
        return ISP_FALSE;
    } else {
        _registerEntryShareMem(pHandle, ispEntry, secInfo, secInfo->buff_size, 0);
        if (_tlcHasSubInfo(&tabIndex)) {
            struct ISPSecTabIndex_t tabSubIndex;

            tabSubIndex = tabIndex;
            tabSubIndex.tb_type = secInfo->sub.type;
            ispSubEntry = MtkSecISP_getTabEntryByIdx(pHandle, &tabSubIndex);
            _registerEntryShareMem(pHandle, ispSubEntry, secInfo, secInfo->sub.buff_size, 1);
        }
    }

    if (secInfo->type == SECMEM_FRAME_HEADER) {
        ISPCA_LOGE("not supported\n");
        return ISP_FALSE;
    } else {
        paramTypes = TZ_ParamTypes(
            TZPT_VALUE_INPUT,
            TZPT_VALUE_INPUT,
            TZPT_VALUE_INPUT,
            TZPT_VALUE_OUTPUT);
        params[0].value.a = secInfo->buff_size;
		params[0].value.b = secInfo->sub.buff_size;
        params[1].value.a = tabIndex.tb_type;
        params[1].value.b = tabIndex.cam_id;
        params[2].value.a = tabIndex.level0;
        params[2].value.b = tabIndex.level1;
        ISPCA_LOGI("%s, tb_type:%d, cam_id:%d, L0:%d, L1:%d\n",
            __FUNCTION__, tabIndex.tb_type, tabIndex.cam_id, tabIndex.level0,
            tabIndex.level1);
        ret = UREE_TeeServiceCall(pHandle->session, TZCMD_ISP_CQTABREGISTER, paramTypes, params);
        secInfo->buff_sec_mva = params[3].value.a;
        secInfo->sub.buff_sec_mva = params[3].value.b;
        if(ret != TZ_RESULT_SUCCESS)
        {
            ISPCA_LOGE("Invoke TZCMD_ISP_CQTABREGISTER failed (0x%08x)", ret);
            if(ispEntry && ispEntry->sharedHandle)
                MtkSecISP_ReleaseSharedMemory(ispEntry->sharedHandle);
            if(ispSubEntry && ispSubEntry->sharedHandle)
                MtkSecISP_ReleaseSharedMemory(ispSubEntry->sharedHandle);
            return ISP_FALSE;
        }
    }
    ISPCA_LOGI("%s, buffer_sec_info:0x%x, sub_sec_info:0x%x\n",
        __FUNCTION__, secInfo->buff_sec_mva, secInfo->sub.buff_sec_mva);

    ISPCA_LOGI("%s-\n", __FUNCTION__);

    return ISP_TRUE;
}

static void _printoutSecInfo(SecMgr_SecInfo *secInfo)
{
    switch (secInfo->type) {
    case SECMEM_CQ_DESCRIPTOR_TABLE:
    case SECMEM_VIRTUAL_REG_TABLE:
        ISPCA_LOGI("%s+ type:%d, cq:%d, dummy:%d, dupq:%d\n",
            __func__, secInfo->type, secInfo->cq,
            secInfo->dummy, secInfo->dupq);
        break;

    case SECMEM_LSC:
        ISPCA_LOGI("%s+ type:%d, buff_va:0x%llx, module:%d\n",
            __func__, secInfo->type, secInfo->buff_va,
            secInfo->module);
        break;
    case SECMEM_BPC:
        break;
    default:
        ISPCA_LOGI("%s+, unknown secure table type\n",
            __func__);
    }

}

int MtkSecISP_tlcMigrateTable(uint64_t ispHandle, SecMgr_SecInfo* secInfo)
{
    TZ_RESULT result;
    IspHandle *pHandle = (IspHandle*)ispHandle;
    uint32_t paramTypes;
    MTEEC_PARAM params[4];
    IspShareMemEntry_t *ispTabEntry, *ispTabEntrySub;
    struct ISPSecTabMirInfo_t tabMirInfo;
    UREE_SHAREDMEM_HANDLE sharedHandle;
    ISP_ATRACE_CALL();
    ISPCA_LOGD("%s+\n", __func__);
    _printoutSecInfo(secInfo);

    if (!MtkSecISP_isInit(pHandle))
    {
        ISPCA_LOGE("%s, SecISP is not initialized. ispHandle:0x%p, gSecIspInit:%d",
            __FUNCTION__, pHandle, gSecISPInit);
        return ISP_FALSE;
    }
    if (MtkSecISP_isTabIndexValid(secInfo) == ISP_FALSE)
        return ISP_FALSE;
    if (_convertSecInfoToTabIndex(pHandle, secInfo, &tabMirInfo.tabIndex) == ISP_FALSE)
        return ISP_FALSE;
    ISPCA_LOGD("%s tb_type:%d cam_id:%d L0:%d L1:%d ",
        __FUNCTION__, tabMirInfo.tabIndex.tb_type, tabMirInfo.tabIndex.cam_id,
        tabMirInfo.tabIndex.level0, tabMirInfo.tabIndex.level1);
    ISPCA_LOGD("type %x module %x cq %x dummy %x dupq %x buff_size %x buff_va %llu port %x buff_sec_mva %x",
        secInfo->type, secInfo->module,
        secInfo->cq, secInfo->dummy,
        secInfo->dupq, secInfo->buff_size,
        secInfo->buff_va, secInfo->port,
        secInfo->buff_sec_mva);
    ispTabEntry = MtkSecISP_getTabEntryByIdx(pHandle, &tabMirInfo.tabIndex);
    if (_useSameMemID(tabMirInfo.tabIndex.tb_type))
    {
        ISPCA_LOGE("not supported");
        return ISP_FALSE;
    } else {
        params[0].memref.handle = ispTabEntry->sharedHandle;
        params[0].memref.offset = 0;
        params[0].memref.size = ispTabEntry->sharedParam.size;
    }

    sharedHandle = MtkSecISP_RegisterSharedMemory(&tabMirInfo, sizeof(struct ISPSecTabMirInfo_t));
    if (sharedHandle == 0)
    {
        ISPCA_LOGE("Register share memory failed");
        return ISP_FALSE;
    }

    params[1].memref.handle = sharedHandle;
    params[1].memref.offset = 0;
    params[1].memref.size = sizeof(struct ISPSecTabMirInfo_t);

    if (_tlcHasSubInfo(&tabMirInfo.tabIndex))
    {
        tabMirInfo.subIndex = tabMirInfo.tabIndex;
        tabMirInfo.subIndex.tb_type = secInfo->sub.type;
        ispTabEntrySub = MtkSecISP_getTabEntryByIdx(pHandle, &tabMirInfo.subIndex);

        if(ispTabEntrySub == NULL) {
            ISPCA_LOGE("fail to get sub table idx=%d type=%d l0=%d l1=%d",
                tabMirInfo.subIndex.cam_id, tabMirInfo.subIndex.tb_type, tabMirInfo.subIndex.level0, tabMirInfo.subIndex.level1);
            ISPCA_LOGD("fail to get sub table, buff_va %llx buff_size %x buff_sec_mva %x type %x",
                secInfo->sub.buff_va,
                secInfo->sub.buff_size,
                secInfo->sub.buff_sec_mva,
                secInfo->sub.type);
            MtkSecISP_ReleaseSharedMemory(sharedHandle);
            return ISP_FALSE;
        }
        params[2].memref.handle = ispTabEntrySub->sharedHandle;
        params[2].memref.offset = 0;
        params[2].memref.size = secInfo->sub.buff_size;

        paramTypes = TZ_ParamTypes(
            TZPT_MEMREF_INOUT,
            TZPT_MEMREF_INOUT,
            TZPT_MEMREF_INOUT,
            TZPT_VALUE_OUTPUT);
    } else {
        paramTypes = TZ_ParamTypes(
            TZPT_MEMREF_INOUT,
            TZPT_MEMREF_INOUT,
            TZPT_NONE,
            TZPT_VALUE_OUTPUT);
    }
    result = UREE_TeeServiceCall(pHandle->session, TZCMD_ISP_CQTABMIGRATE, paramTypes, params);
    if (result < 0) {
        ISPCA_LOGD("%s, TZCMD_ISP_CQTABMIGRATE (0x%08x) para[0]:%x %x para[1]:%x %x para[2]:%x %x",
            __FUNCTION__,
            params[0].memref.handle, params[0].memref.size, 
            params[1].memref.handle, params[1].memref.size, 
            params[2].memref.handle, params[2].memref.size
        );
    }
    MtkSecISP_ReleaseSharedMemory(sharedHandle);
    ISPCA_LOGD("%s-\n", __func__);
    return ISP_TRUE;
}

#if 0
int MtkSecISP_tlcUnregisterShareMem(void *ispHandle, SecMgr_SecInfo secInfo)
{
    IspHandle *pHandle = (IspHandle*)ispHandle;
    uint32_t paramTypes;
    MTEEC_PARAM params[4];    
    UREE_SHAREDMEM_HANDLE sharedHandle;

    int ret = 0;
    IspShareMemEntry_t *ispTabEntry;
    struct ISPSecTabIndex_t tabIndex;

    if (!MtkSecISP_isInit(ispHandle))
    {
        LOGE("%s, SecISP is not initialized. ispHandle:0x%p, gSecIspInit:%d",
            __FUNCTION__, ispHandle, gSecISPInit);
        return ISP_FALSE;
    }
    if (MtkSecISP_isTabIndexValid(&secInfo) == ISP_FALSE)
        return ISP_FALSE;
    if (_convertSecInfoToTabIndex(pHandle, &secInfo, &tabIndex) == ISP_FALSE)
        return ISP_FALSE;

    ISPCA_LOGD("%s tb_type:%d cam_id:%d",
        __FUNCTION__, tabIndex.tb_type, tabIndex.cam_id);

    ispTabEntry = MtkSecISP_getTabEntryByIdx(pHandle, &tabIndex);
    MtkSecISP_ReleaseSharedMemory(ispTabEntry->sharedHandle);


    sharedHandle = MtkSecISP_RegisterSharedMemory(&tabIndex, sizeof(struct ISPSecTabIndex_t));
    if (sharedHandle == 0)
    {
        LOGE("%s, RegisterSharedMemory failed (0x%08x)",
            __FUNCTION__, ret);
        return ret;
    }

    paramTypes = TZ_ParamTypes(
        TZPT_MEMREF_INPUT,
        TZPT_NONE,
        TZPT_NONE,
        TZPT_NONE);

    params[0].memref.handle = sharedHandle;
    params[0].memref.offset = 0;
    params[0].memref.size = sizeof(struct ISPSecTabIndex_t);
    ret = UREE_TeeServiceCall(pHandle->session, TZCMD_ISP_CQTABUNREGISTER, paramTypes, params);
    if(ret != TZ_RESULT_SUCCESS)
    {
        LOGE("Invoke TZCMD_ISP_CQTABUNREGISTER failed (0x%08x)", ret);
        return ret;
    }
    if (!_useSameMemID(secInfo.type) || tabIndex.level1 == 0)
        MtkSecISP_ReleaseSharedMemory(sharedHandle);

    ispTabEntry->sharedHandle = 0;
    ispTabEntry->buff_va = 0;
    return ISP_TRUE;
}
#endif

int MtkSecISP_tlcDumpSecmem(uint64_t handle, uint64_t sec_handle, uint64_t dst_ptr, uint32_t size)
{
    IspHandle *pHandle = (IspHandle*)handle;
    uint32_t paramTypes;
    MTEEC_PARAM params[4];    
    TZ_RESULT ret = 0;
    void *temp_buf = NULL;
    UREE_SHAREDMEM_HANDLE sharedHandle;

    ISPCA_LOGD("%s+\n", __FUNCTION__);
    if (!handle)
    {
        ISPCA_LOGE("%s handle is NULL", __FUNCTION__);
        return ISP_FALSE;
    }

    temp_buf = malloc(size);
    if (!temp_buf)
    {
        ISPCA_LOGI("allocate temp buffer fail\n");
        return ISP_FALSE;
    }

    sharedHandle = MtkSecISP_RegisterSharedMemory(temp_buf, size);

    if (sharedHandle == 0)
    {
        ISPCA_LOGE("%s, Register share memory fail, ret:%d", __FUNCTION__, ret);
        free(temp_buf);
        return ISP_FALSE;
    }

    paramTypes = TZ_ParamTypes(
        TZPT_MEMREF_INOUT,
        TZPT_VALUE_INPUT,
        TZPT_NONE,
        TZPT_NONE);

    params[0].memref.handle = sharedHandle;
    params[0].memref.offset = 0;
    params[0].memref.size = size;
    params[1].value.a = sec_handle;
    params[1].value.b = sec_handle>>32;

    ret = UREE_TeeServiceCall(pHandle->session, TZCMD_ISP_DUMPSECMEM, paramTypes, params);
    if(ret != TZ_RESULT_SUCCESS)
    {
        ISPCA_LOGE("Invoke TZCMD_ISP_DUMPSECMEM failed (0x%08x)", ret);
        free(temp_buf);
        MtkSecISP_ReleaseSharedMemory(sharedHandle);
        return ISP_FALSE;
    }

    MtkSecISP_ReleaseSharedMemory(sharedHandle);

    memcpy((void*)dst_ptr, temp_buf, size);
    free(temp_buf);
    return ISP_TRUE;
}

int MtkSecISP_tlcQueryHWInfo(uint64_t handle, uint64_t SecInfo)
{
    IspHandle *pHandle = (IspHandle*)handle;
    uint32_t paramTypes;
    MTEEC_PARAM params[4];
    TZ_RESULT ret = 0;
    UREE_SHAREDMEM_HANDLE sharedHandle;

    if (!handle)
    {
        ISPCA_LOGE("%s handle is NULL", __FUNCTION__);
        return ISP_FALSE;
    }

    sharedHandle = MtkSecISP_RegisterSharedMemory((void*)SecInfo, sizeof(struct SecMgr_QueryInfo));

    if (sharedHandle == 0)
    {
        ISPCA_LOGE("Register share memory failed (0x%08x)");
        return ISP_FALSE;
    }

    ISPCA_LOGI("%s handle:0x%p",
        __FUNCTION__, pHandle);

    paramTypes = TZ_ParamTypes(
        TZPT_MEMREF_INOUT,
        TZPT_NONE,
        TZPT_NONE,
        TZPT_NONE);

    params[0].memref.handle = sharedHandle;
    params[0].memref.offset = 0;
    params[0].memref.size = sizeof(struct SecMgr_QueryInfo);

    ret = UREE_TeeServiceCall(pHandle->session, TZCMD_ISP_QUERYHWINFO, paramTypes, params);
    if(ret != TZ_RESULT_SUCCESS)
    {
        ISPCA_LOGE("Invoke TZCMD_ISP_QUERYHWINFO failed (0x%08x)", ret);
        MtkSecISP_ReleaseSharedMemory(sharedHandle);
        return ISP_FALSE;
    }

    MtkSecISP_ReleaseSharedMemory(sharedHandle);

    return ISP_TRUE;
}

int MtkSecISP_tlcSetSecCam(uint64_t ispHandle, struct SecMgr_CamInfo camInfo)
{
    IspHandle *pHandle = (IspHandle*)ispHandle;
    uint32_t paramTypes;
    MTEEC_PARAM params[4];
    TZ_RESULT ret = 0;

    UREE_SHAREDMEM_HANDLE sharedHandle;

    sharedHandle = MtkSecISP_RegisterSharedMemory(&camInfo, sizeof(struct SecMgr_CamInfo));

    if (!MtkSecISP_isInit(pHandle))
    {
        ISPCA_LOGE("%s, SecISP is not initialized. ispHandle:0x%p, gSecIspInit:%d",
            __FUNCTION__, pHandle, gSecISPInit);
        MtkSecISP_ReleaseSharedMemory(sharedHandle);
        return ISP_FALSE;
    }

    ISPCA_LOGI("%s+, cam:%d",
        __FUNCTION__, camInfo.CamModule);
    paramTypes = TZ_ParamTypes(
        TZPT_MEMREF_INOUT,
        TZPT_NONE,
        TZPT_NONE,
        TZPT_NONE);
    params[0].memref.handle = sharedHandle;
    params[0].memref.offset = 0;
    params[0].memref.size = sizeof(struct SecMgr_CamInfo);

    ret = UREE_TeeServiceCall(pHandle->session, TZCMD_ISP_SETSECCAM, paramTypes, params);
    if(ret != TZ_RESULT_SUCCESS)
    {
        ISPCA_LOGE("Invoke TZCMD_ISP_SETSECCAM failed (0x%08x)", ret);
        MtkSecISP_ReleaseSharedMemory(sharedHandle);
        return ISP_FALSE;
    }

    MtkSecISP_ReleaseSharedMemory(sharedHandle);
    ISPCA_LOGI("%s-", __FUNCTION__);
    return ISP_TRUE;

}
#if 0
int MtkSecISP_tlcGetSecCam(void *ispHandle, struct Sec_CamState *camState)
{
    IspHandle *pHandle = (IspHandle*)ispHandle;
    uint32_t paramTypes;
    MTEEC_PARAM params[4];
    TZ_RESULT ret = 0;
    UREE_SHAREDMEM_HANDLE sharedHandle;

    sharedHandle = MtkSecISP_RegisterSharedMemory(camState, sizeof(struct Sec_CamState));

    if (!MtkSecISP_isInit(ispHandle))
    {
        LOGE("%s, SecISP is not initialized. ispHandle:0x%p, gSecIspInit:%d",
            __FUNCTION__, ispHandle, gSecISPInit);
        return ISP_FALSE;
    }

    ISPCA_LOGI("%s+", __FUNCTION__);
    paramTypes = TZ_ParamTypes(
        TZPT_MEMREF_INOUT,
        TZPT_NONE,
        TZPT_NONE,
        TZPT_NONE);
    params[0].memref.handle = sharedHandle;
    params[0].memref.offset = 0;
    params[0].memref.size = sizeof(struct Sec_CamState);

    ret = UREE_TeeServiceCall(pHandle->session, TZCMD_ISP_GETSECCAM, paramTypes, params);
    if(ret != TZ_RESULT_SUCCESS)
    {
        LOGE("Invoke TZCMD_ISP_GETSECCAM failed (0x%08x)", ret);
        return ret;
    }

    MtkSecISP_ReleaseSharedMemory(sharedHandle);
    ISPCA_LOGI("%s-", __FUNCTION__);
    return ISP_TRUE;

}
#endif

