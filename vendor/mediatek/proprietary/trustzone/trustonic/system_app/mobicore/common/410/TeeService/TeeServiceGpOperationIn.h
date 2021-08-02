/*
 * Copyright (c) 2018 TRUSTONIC LIMITED
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the TRUSTONIC LIMITED nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __TEE_SERVICE_GP_OPERATION_IN_H__
#define __TEE_SERVICE_GP_OPERATION_IN_H__

#include <memory>

#include <binder/Parcel.h>

#include "tee_client_api.h"
#include "TeeServiceGpOperation.h"

namespace vendor {

namespace trustonic {

namespace teeservice {

// How it works:
//  * push type
//  * none: just skip
//  * values:
//    * output: ignore
//    * input/inout: push
//  * buffer:
//    * push fd -> buffer
//    * push flags
//    * if partial:
//      * push offset
//      * push size

struct TeeServiceGpOperationIn {
    static void writeToParcel(
            const TeeServiceGpOperation& operation,
            ::android::Parcel* parcel) {
        parcel->writeUint64(operation.reference);
        if (!operation.reference) {
            return;
        }

        parcel->writeUint32(operation.started);
        for (size_t i = 0; i < 4; i++) {
            const TeeServiceGpOperation::Param& param = operation.params[i];
            parcel->writeUint32(param.type);
            switch (param.type) {
                case TEEC_NONE:
                case TEEC_VALUE_OUTPUT:
                    break;
                case TEEC_VALUE_INPUT:
                case TEEC_VALUE_INOUT:
                    parcel->writeUint32(param.value_a);
                    parcel->writeUint32(param.value_b);
                    break;
                case TEEC_MEMREF_PARTIAL_OUTPUT:
                case TEEC_MEMREF_PARTIAL_INPUT:
                case TEEC_MEMREF_PARTIAL_INOUT:
                    parcel->writeUint64(param.window_offset);
                    parcel->writeUint64(param.window_size);
                    // Fall through
		    [[fallthrough]];
                case TEEC_MEMREF_WHOLE:
                    parcel->writeUint64(param.reference);
                    // Fall through
		    [[fallthrough]];
                case TEEC_MEMREF_TEMP_OUTPUT:
                case TEEC_MEMREF_TEMP_INPUT:
                case TEEC_MEMREF_TEMP_INOUT:
                    // A Temporary Memory Reference may be null, which can be
                    // used to request the required output size.
                    if (param.buffer < 0) {
                        parcel->writeBool(false);
                    } else {
                        parcel->writeBool(true);
                        parcel->writeFileDescriptor(param.buffer);
                    }

                    parcel->writeUint64(param.size);
                    // Not part of GP protocol for _TEMP_, but used to know when
                    // to copy
                    parcel->writeUint32(param.flags);
                    break;
            }
        }
    }

    static void readFromParcel(
            const ::android::Parcel* parcel,
            TeeServiceGpOperation* operation) {
        parcel->readUint64(&operation->reference);
        if (!operation->reference) {
            return;
        }

        parcel->readUint32(&operation->started);
        for (size_t i = 0; i < 4; i++) {
            TeeServiceGpOperation::Param& param = operation->params[i];
            parcel->readUint32(&param.type);
            switch (param.type) {
                case TEEC_NONE:
                case TEEC_VALUE_OUTPUT:
                    break;
                case TEEC_VALUE_INPUT:
                case TEEC_VALUE_INOUT:
                    parcel->readUint32(&param.value_a);
                    parcel->readUint32(&param.value_b);
                    break;
                case TEEC_MEMREF_PARTIAL_OUTPUT:
                case TEEC_MEMREF_PARTIAL_INPUT:
                case TEEC_MEMREF_PARTIAL_INOUT:
                    parcel->readUint64(&param.window_offset);
                    parcel->readUint64(&param.window_size);
                    // Fall through
		    [[fallthrough]];
                case TEEC_MEMREF_WHOLE:
                    parcel->readUint64(&param.reference);
                    // Fall through
		    [[fallthrough]];
                case TEEC_MEMREF_TEMP_OUTPUT:
                case TEEC_MEMREF_TEMP_INPUT:
                case TEEC_MEMREF_TEMP_INOUT:
                    if (parcel->readBool()) {
                        param.buffer = ::dup(parcel->readFileDescriptor());
                    }

                    parcel->readUint64(&param.size);
                    // Not part of GP protocol for _TEMP_, but used to know when
                    // to copy
                    parcel->readUint32(&param.flags);
                    break;
            }
        }
    }
};

}

}

}

#endif // __TEE_SERVICE_GP_OPERATION_IN_H__
