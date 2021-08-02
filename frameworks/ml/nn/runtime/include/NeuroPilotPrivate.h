/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2018. All rights reserved.
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

#ifndef ANDROID_ML_NN_RUNTIME_NEURO_PILOT_PRIVATE_H
#define ANDROID_ML_NN_RUNTIME_NEURO_PILOT_PRIVATE_H

#if __ANDROID_API__ >= __ANDROID_API_O_MR1__

#include "NeuroPilotDef.h"

/*************************************************************************************************/
/*************************************************************************************************/
// Create Instance
typedef int (*ANeuroPilotModel_create_fn)(ANeuralNetworksModel** model);

// Profiler
typedef int (*ANeuroPilotExecution_setCurrentExecutionStep_fn)(
        ANeuralNetworksExecution *execution, uint32_t step);

typedef int (*ANeuroPilotExecution_startProfile_fn)(
        ANeuralNetworksStepExecutor *executor, const char* device);

typedef int (*ANeuroPilotExecution_stopProfile_fn)(
        ANeuralNetworksStepExecutor *executor, ANeuralNetworksRequest *request, int err);

typedef int (*ANeuroPilotExecution_clearProfilerInfo_fn)(
        ANeuralNetworksExecution *execution);

typedef bool (*ANeuroPilotUtils_isProfilerSupported_fn)();

// Utils
typedef bool (*ANeuroPilotUtils_forbidCpuExecution_fn)();

/*************************************************************************************************/
// Create Instance
inline int ANeuroPilotModelPrivate_makeModelBuilder(ANeuralNetworksModel **model) {
    LOAD_NP_FUNCTION(ANeuroPilotModel_create);
    EXECUTE_NP_FUNCTION_RETURN_INT(model);
}

// Profiler
inline int ANeuroPilotExecutionPrivate_setCurrentExecutionStep(
        ANeuralNetworksExecution *execution, uint32_t step) {
    LOAD_NP_FUNCTION(ANeuroPilotExecution_setCurrentExecutionStep);
    EXECUTE_NP_FUNCTION_RETURN_INT(execution, step);
}

inline int ANeuroPilotExecutionPrivate_startProfile(ANeuralNetworksStepExecutor *executor,
        const char* device) {
    LOAD_NP_FUNCTION(ANeuroPilotExecution_startProfile);
    EXECUTE_NP_FUNCTION_RETURN_INT(executor, device);
}

inline int ANeuroPilotExecutionPrivate_stopProfile(
        ANeuralNetworksStepExecutor *executor, ANeuralNetworksRequest *request, int err) {
    LOAD_NP_FUNCTION(ANeuroPilotExecution_stopProfile);
    EXECUTE_NP_FUNCTION_RETURN_INT(executor, request, err);
}

inline int ANeuroPilotExecutionPrivate_clearProfilerInfo(
        ANeuralNetworksExecution *execution) {
    LOAD_NP_FUNCTION(ANeuroPilotExecution_clearProfilerInfo);
    EXECUTE_NP_FUNCTION_RETURN_INT(execution);
}

inline bool ANeuroPilotUtilsPrivate_isProfilerSupported() {
    LOAD_NP_FUNCTION(ANeuroPilotUtils_isProfilerSupported);
    EXECUTE_NP_FUNCTION_RETURN_BOOL();
}

// Utils
inline bool ANeuroPilotUtilsPrivate_forbidCpuExecution() {
    LOAD_NP_FUNCTION(ANeuroPilotUtils_forbidCpuExecution);
    EXECUTE_NP_FUNCTION_RETURN_BOOL();
}

#endif  //  __ANDROID_API__ >= 27
#endif  // ANDROID_ML_NN_RUNTIME_NEURO_PILOT_PRIVATE_H

