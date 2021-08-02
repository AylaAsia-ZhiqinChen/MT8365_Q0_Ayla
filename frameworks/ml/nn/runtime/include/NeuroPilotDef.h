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

#ifndef MEDIATEK_ML_NN_RUNTIME_NEURO_PILOT_DEFITION_H
#define MEDIATEK_ML_NN_RUNTIME_NEURO_PILOT_DEFITION_H

#include <dlfcn.h>
#include <android/log.h>

/*************************************************************************************************/
// Definition
/// M: Profiler @{
typedef struct {
    std::string devName;                    // Execution Device
    std::string opName;                     // Operation List on this step
    double delta;                           // Execution Time (us)
} ExecTime;

typedef struct {
    int32_t operandType;                    // Operand Type
    uint32_t size;                          // Output buffer size (byte)
    uint8_t *buffer;                        // Output buffer
} ExecResult;

typedef struct {
    uint32_t step;                          // Current Step (Sub-Model)
    bool success;                           // Execution Result
    ExecTime deviceTime;                    // Struct to Record Info of Rxecution Time
    std::vector<ExecResult> opResults;      // Struct to Record Info of Rxecution Result
} ProfilerResult;
/// M: Profiler @}

typedef struct ANeuralNetworksStepExecutor ANeuralNetworksStepExecutor;
typedef struct ANeuralNetworksRequest ANeuralNetworksRequest;

/*************************************************************************************************/
// For add-on

#define NP_LOG(format, ...) \
    __android_log_print(ANDROID_LOG_DEBUG, "NeuroPilot", format "\n", ##__VA_ARGS__);

#define LOAD_NP_FUNCTION(name) \
  static name##_fn fn = reinterpret_cast<name##_fn>(loadNpFunction(#name));

#define EXECUTE_NP_FUNCTION(...) \
  if (fn != nullptr) {        \
    fn(__VA_ARGS__);          \
  }

#define EXECUTE_NP_FUNCTION_RETURN_INT(...) \
    return fn != nullptr ? fn(__VA_ARGS__) : ANEURALNETWORKS_BAD_STATE;

#define EXECUTE_NP_FUNCTION_RETURN_BOOL(...) \
    return fn != nullptr ? fn(__VA_ARGS__) : false;

static void* sHandle;
inline void* loadNpLibrary(const char* name) {
    sHandle = dlopen(name, RTLD_LAZY | RTLD_LOCAL);
    if (sHandle == nullptr) {
        NP_LOG("npapi error: unable to open library %s", name);
    }
    return sHandle;
}

inline void* getNpLibraryHandle() {
    if (sHandle == nullptr) {
        sHandle = loadNpLibrary("libneuropilot.so");
    }
    return sHandle;
}

inline void* loadNpFunction(const char* name) {
    void* fn = nullptr;
    if (getNpLibraryHandle() != nullptr) {
        fn = dlsym(getNpLibraryHandle(), name);
    }

    if (fn == nullptr) {
        NP_LOG("npapi error: unable to open function %s", name);
    }

    return fn;
}

#endif  // MEDIATEK_ML_NN_RUNTIME_NEURO_PILOT_DEFITION_H

