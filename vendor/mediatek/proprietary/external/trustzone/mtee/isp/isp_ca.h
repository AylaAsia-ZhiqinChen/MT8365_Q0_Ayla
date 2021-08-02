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

#ifndef __ISP_CA_H__
#define __ISP_CA_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <tz_cross/ta_mem.h>
#include <tz_cross/ta_dbg.h>
#include <tz_cross/ta_isp_sec_top.h>
#include <tz_cross/ta_isp_sec_common.h>
#include <tz_cross/ta_isp_sec_drvplatform.h>

struct SecMgr_RegInfo {
	uint32_t dapc_cq[DAPC_NUM_CQ];
};
typedef struct{
    SEC_MEM_TYPE type;
    uint32_t buff_size;
    uint64_t buff_va;
    uint32_t buff_sec_mva;
    int32_t  memID;
}SecMgr_SubSecInfo;

typedef struct{
    SEC_MEM_TYPE type;
    uint32_t module;
    uint32_t cq;
    uint32_t dummy;
    uint32_t dupq;
    uint32_t buff_size;
    uint64_t buff_va;
    uint32_t port;
    uint32_t buff_sec_mva;
    int32_t  memID;
    SecMgr_SubSecInfo sub;
}SecMgr_SecInfo;

typedef enum {
	MEM_PROTECT,
	MEM_SECURE,
	MEM_2DFR,
} MEM_TYPE;

uint64_t MtkSecISP_tlcHandleCreate();

int MtkSecISP_tlcOpen(uint64_t ispHandle);

int MtkSecISP_tlcClose(uint64_t ispHandle);

int MtkSecISP_tlcInit(uint64_t ispHandle);

int MtkSecISP_tlcUnInit(uint64_t ispHandle);

int MtkSecISP_tlcSecConfig(uint64_t ispHandle, int port, int tg_idx);

int MtkSecISP_tlcQueryMVAFromHandle(uint64_t ispHandle, uint64_t sec_handle, uint64_t pa,
	MEM_TYPE mem_type);

int MtkSecISP_tlcRegisterShareMem(uint64_t ispHandle, uint64_t secInfo);

int MtkSecISP_tlcMigrateTable(uint64_t ispHandle, SecMgr_SecInfo* secInfo);

int MtkSecISP_tlcDumpSecmem(uint64_t ispHandle, uint64_t sec_handle, uint64_t ptr, unsigned int size);

int MtkSecISP_tlcQueryHWInfo(uint64_t ispHandle, uint64_t SecInfo);

int MtkSecISP_tlcSetSecCam(uint64_t ispHandle, struct SecMgr_CamInfo camInfo);

#ifdef __cplusplus
} //end extern "C"
#endif

#endif

