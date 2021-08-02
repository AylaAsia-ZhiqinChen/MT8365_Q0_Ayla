
/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2019. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#ifndef __TRANSLATOR_MANAGER_H__
#define __TRANSLATOR_MANAGER_H__

#include "OemKpiDef.h"
#include "OemKpiMap.h"

// Defined with libtranslator_utils.so
#include "TranslatorManagerBase.h"

using namespace std;

#define OEM_TRANSLATOR_VERSION "v1.0"

// Include macro header file to define translator class
#include "BeginTranslatorClassDefine.h"
#include "TranslatorList_v1.0.inc"
#include "EndTranslatorClassDefine.h"

class TranslatorManager : public TranslatorManagerBase {
public:
    TranslatorManager();
    ~TranslatorManager();
    KPI_GID getKpiGroupId(KPI_OBJ_TYPE kpiType) override;
    KPI_OBJ_RESULT_CODE translateKpi(
            KPI_OBJ_TYPE kpiType,
            KPI_MSGID msgId,
            KPI_SIMID simId,
            uint64_t &timestampUs,
            KPI_SOURCE_TYPE source,
            KpiObj &outObj,
            uint64_t datalen,
            const void *data,
            const void *param) override;

private:
    void setupTranslatorConfig() override;
    ObjId *getObjId(KPI_OBJ_TYPE kpiType) override;
    KPI_OBJ_RESULT_CODE makeOemTrapMessage(
            KPI_OBJ_TYPE kpiType, KPI_SIMID simId, uint64_t timestampUs, ObjId *pOid, KpiObj &outObj);
};

#endif
