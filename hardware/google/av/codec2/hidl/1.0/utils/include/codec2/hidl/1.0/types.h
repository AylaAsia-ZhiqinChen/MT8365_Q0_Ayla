/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HARDWARE_GOOGLE_MEDIA_C2_V1_0_UTILS_TYPES_H
#define HARDWARE_GOOGLE_MEDIA_C2_V1_0_UTILS_TYPES_H

#include <chrono>

#include <bufferpool/ClientManager.h>
#include <android/hardware/media/bufferpool/1.0/IClientManager.h>
#include <android/hardware/media/bufferpool/1.0/types.h>
#include <hardware/google/media/c2/1.0/IComponentStore.h>
#include <hardware/google/media/c2/1.0/types.h>
#include <gui/IGraphicBufferProducer.h>

#include <C2Component.h>
#include <C2Param.h>
#include <C2ParamDef.h>
#include <C2Work.h>

using namespace std::chrono_literals;

namespace hardware {
namespace google {
namespace media {
namespace c2 {
namespace V1_0 {
namespace utils {

using ::android::hardware::hidl_bitfield;
using ::android::hardware::hidl_handle;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::status_t;
using ::android::sp;
using ::android::hardware::media::bufferpool::V1_0::implementation::
        ConnectionId;
using ::android::IGraphicBufferProducer;

// Types of metadata for Blocks.
struct C2Hidl_Range {
    uint32_t offset;
    uint32_t length; // Do not use "size" because the name collides with C2Info::size().
};
typedef C2GlobalParam<C2Info, C2Hidl_Range, 0> C2Hidl_RangeInfo;

struct C2Hidl_Rect {
    uint32_t left;
    uint32_t top;
    uint32_t width;
    uint32_t height;
};
typedef C2GlobalParam<C2Info, C2Hidl_Rect, 1> C2Hidl_RectInfo;

// C2SettingResult -> SettingResult
Status objcpy(
        SettingResult* d,
        const C2SettingResult& s);

// SettingResult -> std::unique_ptr<C2SettingResult>
c2_status_t objcpy(
        std::unique_ptr<C2SettingResult>* d,
        const SettingResult& s);

// C2ParamDescriptor -> ParamDescriptor
Status objcpy(
        ParamDescriptor* d,
        const C2ParamDescriptor& s);

// ParamDescriptor -> std::shared_ptr<C2ParamDescriptor>
c2_status_t objcpy(
        std::shared_ptr<C2ParamDescriptor>* d,
        const ParamDescriptor& s);

// C2FieldSupportedValuesQuery -> FieldSupportedValuesQuery
Status objcpy(
        FieldSupportedValuesQuery* d,
        const C2FieldSupportedValuesQuery& s);

// FieldSupportedValuesQuery -> C2FieldSupportedValuesQuery
c2_status_t objcpy(
        C2FieldSupportedValuesQuery* d,
        const FieldSupportedValuesQuery& s);

// C2FieldSupportedValuesQuery -> FieldSupportedValuesQueryResult
Status objcpy(
        FieldSupportedValuesQueryResult* d,
        const C2FieldSupportedValuesQuery& s);

// FieldSupportedValuesQuery, FieldSupportedValuesQueryResult -> C2FieldSupportedValuesQuery
c2_status_t objcpy(
        C2FieldSupportedValuesQuery* d,
        const FieldSupportedValuesQuery& sq,
        const FieldSupportedValuesQueryResult& sr);

// C2Component::Traits -> ComponentTraits
Status objcpy(
        IComponentStore::ComponentTraits* d,
        const C2Component::Traits& s);

// ComponentTraits -> C2Component::Traits, std::unique_ptr<std::vector<std::string>>
// Note: The output d is only valid as long as aliasesBuffer remains alive.
c2_status_t objcpy(
        C2Component::Traits* d,
        std::unique_ptr<std::vector<std::string>>* aliasesBuffer,
        const IComponentStore::ComponentTraits& s);

// C2StructDescriptor -> StructDescriptor
Status objcpy(
        StructDescriptor* d,
        const C2StructDescriptor& s);

// StructDescriptor -> C2StructDescriptor
c2_status_t objcpy(
        std::unique_ptr<C2StructDescriptor>* d,
        const StructDescriptor& s);

// Abstract class to be used in
// objcpy(std::list<std::unique_ptr<C2Work>> -> WorkBundle).
struct BufferPoolSender {
    typedef ::android::hardware::media::bufferpool::V1_0::
            ResultStatus ResultStatus;
    typedef ::android::hardware::media::bufferpool::V1_0::
            BufferStatusMessage BufferStatusMessage;
    typedef ::android::hardware::media::bufferpool::
            BufferPoolData BufferPoolData;

    /**
     * Send bpData and return BufferStatusMessage that can be supplied to
     * IClientManager::receive() in the receiving process.
     *
     * This function will be called from within the function
     * objcpy(std::list<std::unique_ptr<C2Work>> -> WorkBundle).
     *
     * \param[in] bpData BufferPoolData identifying the buffer to send.
     * \param[out] bpMessage BufferStatusMessage of the transaction. Information
     *    inside \p bpMessage should be passed to the receiving process by some
     *    other means so it can call receive() properly.
     * \return ResultStatus value that determines the success of the operation.
     *    (See the possible values of ResultStatus in
     *    hardware/interfaces/media/bufferpool/1.0/types.hal.)
     */
    virtual ResultStatus send(
            const std::shared_ptr<BufferPoolData>& bpData,
            BufferStatusMessage* bpMessage) = 0;

    virtual ~BufferPoolSender() = default;
};

// Default implementation of BufferPoolSender.
//
// To use DefaultBufferPoolSender, the IClientManager instance of the receiving
// process must be set before send() can operate. DefaultBufferPoolSender will
// hold a strong reference to the IClientManager instance and use it to call
// IClientManager::registerSender() to establish the bufferpool connection when
// send() is called.
struct DefaultBufferPoolSender : BufferPoolSender {
    typedef ::android::hardware::media::bufferpool::V1_0::implementation::
            ClientManager ClientManager;
    typedef ::android::hardware::media::bufferpool::V1_0::
            IClientManager IClientManager;

    // Set the IClientManager instance of the receiving process and the refresh
    // interval for the connection. The default interval is 4.5 seconds, which
    // is slightly shorter than the amount of time the bufferpool will keep an
    // inactive connection for.
    DefaultBufferPoolSender(
            const sp<IClientManager>& receiverManager = nullptr,
            std::chrono::steady_clock::duration refreshInterval = 4500ms);

    // Set the IClientManager instance of the receiving process and the refresh
    // interval for the connection. The default interval is 4.5 seconds, which
    // is slightly shorter than the amount of time the bufferpool will keep an
    // inactive connection for.
    void setReceiver(
            const sp<IClientManager>& receiverManager,
            std::chrono::steady_clock::duration refreshInterval = 4500ms);

    // Implementation of BufferPoolSender::send(). send() will establish a
    // bufferpool connection if needed, then send the bufferpool data over to
    // the receiving process.
    virtual ResultStatus send(
            const std::shared_ptr<BufferPoolData>& bpData,
            BufferStatusMessage* bpMessage) override;

private:
    std::mutex mMutex;
    sp<ClientManager> mSenderManager;
    sp<IClientManager> mReceiverManager;
    int64_t mReceiverConnectionId;
    int64_t mSourceConnectionId;
    std::chrono::steady_clock::time_point mLastSent;
    std::chrono::steady_clock::duration mRefreshInterval;
};

// std::list<std::unique_ptr<C2Work>> -> WorkBundle
// Note: If bufferpool will be used, bpSender must not be null.
Status objcpy(
        WorkBundle* d,
        const std::list<std::unique_ptr<C2Work>>& s,
        BufferPoolSender* bpSender = nullptr);

// WorkBundle -> std::list<std::unique_ptr<C2Work>>
c2_status_t objcpy(
        std::list<std::unique_ptr<C2Work>>* d,
        const WorkBundle& s);

/**
 * Parses a params blob and returns C2Param pointers to its params.
 * \param[out] params target vector of C2Param pointers
 * \param[in] blob parameter blob to parse
 * \retval C2_OK if the full blob was parsed
 * \retval C2_BAD_VALUE otherwise
 */
c2_status_t parseParamsBlob(
        std::vector<C2Param*> *params,
        const hidl_vec<uint8_t> &blob);

/**
 * Concatenates a list of C2Params into a params blob.
 * \param[out] blob target blob
 * \param[in] params parameters to concatenate
 * \retval C2_OK if the blob was successfully created
 * \retval C2_BAD_VALUE if the blob was not successful (this only happens if the parameters were
 *         not const)
 */
Status createParamsBlob(
        hidl_vec<uint8_t> *blob,
        const std::vector<C2Param*> &params);
Status createParamsBlob(
        hidl_vec<uint8_t> *blob,
        const std::vector<std::unique_ptr<C2Param>> &params);
Status createParamsBlob(
        hidl_vec<uint8_t> *blob,
        const std::vector<std::shared_ptr<const C2Info>> &params);
Status createParamsBlob(
        hidl_vec<uint8_t> *blob,
        const std::vector<std::unique_ptr<C2Tuning>> &params);

/**
 * Parses a params blob and create a vector of C2Params whose members are copies
 * of the params in the blob.
 * \param[out] params the resulting vector
 * \param[in] blob parameter blob to parse
 * \retval C2_OK if the full blob was parsed and params was constructed
 * \retval C2_BAD_VALUE otherwise
 */
c2_status_t copyParamsFromBlob(
        std::vector<std::unique_ptr<C2Param>>* params,
        Params blob);

/**
 * Parses a params blob and applies updates to params
 * \param[in,out] params params to be updated
 * \param[in] blob parameter blob containing updates
 * \retval C2_OK if the full blob was parsed and params was updated
 * \retval C2_BAD_VALUE otherwise
 */
c2_status_t updateParamsFromBlob(
        const std::vector<C2Param*>& params,
        const Params& blob);

/**
 * Converts a BufferPool status value to c2_status_t.
 * \param BufferPool status
 * \return Corresponding c2_status_t
 */
c2_status_t toC2Status(::android::hardware::media::bufferpool::V1_0::
        ResultStatus rs);

// BufferQueue-Based Block Operations
// ==================================

// Create a GraphicBuffer object from a graphic block and attach it to an
// IGraphicBufferProducer.
status_t attachToBufferQueue(const C2ConstGraphicBlock& block,
                             const sp<IGraphicBufferProducer>& igbp,
                             uint32_t generation,
                             int32_t* bqSlot);

// Return false if block does not come from a bufferqueue-based blockpool.
// Otherwise, extract generation, bqId and bqSlot and return true.
bool getBufferQueueAssignment(const C2ConstGraphicBlock& block,
                              uint32_t* generation,
                              uint64_t* bqId,
                              int32_t* bqSlot);

// Disassociate the given block with its designated bufferqueue so that
// cancelBuffer() will not be called when the block is destroyed. If the block
// does not have a designated bufferqueue, the function returns false.
// Otherwise, it returns true.
//
// Note: This function should be called after attachBuffer() or queueBuffer() is
// called manually.
bool yieldBufferQueueBlock(const C2ConstGraphicBlock& block);

// Call yieldBufferQueueBlock() on blocks in the given workList. processInput
// determines whether input blocks are yielded. processOutput works similarly on
// output blocks. (The default value of processInput is false while the default
// value of processOutput is true. This implies that in most cases, only output
// buffers contain bufferqueue-based blocks.)
//
// Note: This function should be called after WorkBundle has been successfully
// sent over the Treble boundary to another process.
void yieldBufferQueueBlocks(const std::list<std::unique_ptr<C2Work>>& workList,
                            bool processInput = false,
                            bool processOutput = true);

// Assign the given block to a bufferqueue so that when the block is destroyed,
// cancelBuffer() will be called.
//
// If the block does not come from a bufferqueue-based blockpool, this function
// returns false.
//
// If the block already has a bufferqueue assignment that matches the given one,
// the function returns true.
//
// If the block already has a bufferqueue assignment that does not match the
// given one, the block will be reassigned to the given bufferqueue. This
// will call attachBuffer() on the given igbp. The function then returns true on
// success or false on any failure during the operation.
//
// Note: This function should be called after detachBuffer() or dequeueBuffer()
// is called manually.
bool holdBufferQueueBlock(const C2ConstGraphicBlock& block,
                          const sp<IGraphicBufferProducer>& igbp,
                          uint64_t bqId,
                          uint32_t generation);

// Call holdBufferQueueBlock() on input or output blocks in the given workList.
// Since the bufferqueue assignment for input and output buffers can be
// different, this function takes forInput to determine whether the given
// bufferqueue is for input buffers or output buffers. (The default value of
// forInput is false.)
//
// In the (rare) case that both input and output buffers are bufferqueue-based,
// this function must be called twice, once for the input buffers and once for
// the output buffers.
//
// Note: This function should be called after WorkBundle has been received from
// another process.
void holdBufferQueueBlocks(const std::list<std::unique_ptr<C2Work>>& workList,
                           const sp<IGraphicBufferProducer>& igbp,
                           uint64_t bqId,
                           uint32_t generation,
                           bool forInput = false);

}  // namespace utils
}  // namespace V1_0
}  // namespace c2
}  // namespace media
}  // namespace google
}  // namespace hardware

#endif  // HARDWARE_GOOGLE_MEDIA_C2_V1_0_UTILS_TYPES_H
