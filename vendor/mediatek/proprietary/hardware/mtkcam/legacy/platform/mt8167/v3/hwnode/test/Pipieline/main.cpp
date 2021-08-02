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
 * MediaTek Inc. (C) 2010. All rights reserved.
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
#define LOG_TAG "PipielineNodeTest"
//
#include <mtkcam/Log.h>
//
#include <stdlib.h>
#include <utils/Errors.h>
#include <utils/List.h>
#include <utils/RefBase.h>
//
#include <mtkcam/metadata/client/mtk_metadata_tag.h>
//
#include <mtkcam/v3/pipeline/IPipelineDAG.h>
#include <mtkcam/v3/pipeline/IPipelineNode.h>
#include <mtkcam/v3/pipeline/IPipelineNodeMapControl.h>
#include <mtkcam/v3/pipeline/IPipelineFrameControl.h>
//
#include <mtkcam/v3/utils/streambuf/StreamBufferPool.h>
#include <mtkcam/v3/utils/streambuf/StreamBuffers.h>
#include <mtkcam/v3/utils/streaminfo/MetaStreamInfo.h>
#include <mtkcam/v3/utils/streaminfo/ImageStreamInfo.h>
//
#include <mtkcam/utils/imagebuf/IIonImageBufferHeap.h>
//
#include <mtkcam/hal/IHalSensor.h>
//
#include <../../BaseNode.h>
#include <mtkcam/v3/hwnode/P1Node.h>
#include <mtkcam/v3/hwnode/P2Node.h>
//
//[workaround]
#include <mtkcam/hal/sensor_hal.h>
//
using namespace NSCam;
using namespace v3;
using namespace NSCam::v3::Utils;
using namespace android;

/******************************************************************************
 *
 ******************************************************************************/
#define MY_LOGV(fmt, arg...)        CAM_LOGV("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_LOGD("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_LOGI("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_LOGW("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_LOGE("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
//
#define MY_LOGV_IF(cond, ...)       do { if ( (cond) ) { MY_LOGV(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) ) { MY_LOGI(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) ) { MY_LOGW(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) ) { MY_LOGE(__VA_ARGS__); } }while(0)
//
#define TEST(cond, result)          do { if ( (cond) == (result) ) { printf("Pass\n"); } else { printf("Failed\n"); } }while(0)
#define FUNCTION_IN     MY_LOGD_IF(1, "+");

#define P2_SUPPORT 0
/******************************************************************************
 *
 ******************************************************************************/
void help()
{
    printf("Pipeline <test>\n");
}

/******************************************************************************
 *
 ******************************************************************************/

namespace {

    enum STREAM_ID{
        STREAM_ID_RAW1 = 1,
        STREAM_ID_RAW2,
        STREAM_ID_YUV1,
        STREAM_ID_YUV2,
        STREAM_ID_INMETADATA,
        STREAM_ID_APPMETADATA1,
        STREAM_ID_APPMETADATA2,
        STREAM_ID_HALMETADATA1
    };

    enum NODE_ID{
        NODE_ID_NODE1 = 1,
        NODE_ID_NODE2,
        NODE_ID_FAKE
    };

    class FakeNode : public virtual IPipelineNode
                   , public virtual BaseNode
    {

        protected:  ////
            Vector<MUINT32>                 mStreamId;

        public:     ////                    Attributes.

                                            FakeNode()
                                                : IPipelineNode()
                                                , BaseNode()
                                                {}

            struct  ConfigParams
            {
                Vector<MUINT32> inputStreamId;
            };

        public:     ////                    Operations.

            /**
             *
             */
            virtual MERROR                  init() { return OK; }

            /**
             *
             */
            virtual MERROR                  uninit() { return OK; }

            /**
             *
             */
            using BaseNode::flush;
            virtual MERROR                  flush() { return OK; }

            /**
             *
             */
            virtual MERROR                  queue(
                                                android::sp<IPipelineFrame> pFrame
                                            )
                                            {
                                                MY_LOGD("[FakeNode] FrameNo: %d", pFrame->getFrameNo());
                                                return onHandleflush(pFrame);
                                            }

            /**
             *
             */
            virtual MVOID                   config(ConfigParams &config){
                                                mOpenId = 0;
                                                mNodeId = NODE_ID_FAKE;
                                                mNodeName = "fake";
                                                mStreamId = config.inputStreamId;
                                            }

        protected:

            virtual MERROR                  onHandleflush(android::sp<IPipelineFrame>& /*pFrame*/) {
                                                //IStreamBufferSet& rStreamBufferSet  = pFrame->getStreamBufferSet();
                                                //  Meta Stream Buffers
                                                //  Mark this buffer as RELEASE by this user.
                                                //  Image Stream Buffers
                                                //  Mark this buffer as RELEASE by this user.
                                                //for (size_t i = 0; i < mStreamId.size(); i++) {
                                                //    rStreamBufferSet.markUserStatus(mStreamId[i], NODE_ID_FAKE, IUsersManager::UserStatus::RELEASE);
                                                //}
                                                //
                                                //rStreamBufferSet.applyRelease(NODE_ID_FAKE);

                                                return OK;
                                            }
    };




    class AppSimulator
        : public virtual RefBase
    {
    };

    //
    android::sp<AppSimulator>           mpAppSimulator = NULL;
    //
    sp<HalImageStreamBuffer::Allocator::StreamBufferPoolT> mpPool_HalImage = NULL;
    sp<HalImageStreamBuffer::Allocator::StreamBufferPoolT> mpPool_HalResizeImage = NULL;
    sp<HalMetaStreamBuffer::Allocator::StreamBufferPoolT> mpPool_HalMetadata = NULL;
    //
    typedef NSCam::v3::Utils::IStreamInfoSetControl       IStreamInfoSetControlT;
    android::sp<IStreamInfoSetControlT> mpStreamInfoSet = NULL;
    android::sp<IPipelineNodeMapControl>mpPipelineNodeMap = NULL;
    android::sp<IPipelineDAG>           mpPipelineDAG = NULL;
    android::sp<P1Node>                 mpNode1 = NULL;
    android::sp<P2Node>                 mpNode2 = NULL;
    android::sp<FakeNode>               mpFakeNode = NULL;
    //
    IHalSensor* mpSensorHalObj;
    P1Node::SensorParams mSensorParams;
    android::Vector<android::sp<IImageStreamInfo> > mpvHalImageRaw;
    android::sp<IImageStreamInfo> mpHalImageResizeRaw = NULL;
    android::Vector<android::sp<IImageStreamInfo> > mpvImageYUV;
    sp<IMetaStreamInfo>  mpHalMetaPlatform = NULL;
    sp<IMetaStreamInfo>  mpAppMetaResult = NULL;
    sp<IMetaStreamInfo>  mpApp2MetaResult = NULL;
    sp<IMetaStreamInfo>  mpAppMetaRequest = NULL;
    //
    static MUINT32 gSensorId = 0;
    static bool test_full = true;
    static bool test_resize = true;
}; // namespace


/******************************************************************************
 *
 ******************************************************************************/
android::sp<IPipelineNodeMapControl>
getPipelineNodeMapControl()
{
    return mpPipelineNodeMap;
}


/******************************************************************************
 *
 ******************************************************************************/
android::sp<IStreamInfoSet>
getStreamInfoSet()
{
    return mpStreamInfoSet;
}


/******************************************************************************
 *
 ******************************************************************************/
android::sp<IPipelineNodeMap>
getPipelineNodeMap()
{
    return mpPipelineNodeMap;
}


/******************************************************************************
 *
 ******************************************************************************/
android::sp<IPipelineDAG>
getPipelineDAG()
{
    return mpPipelineDAG;
}


/******************************************************************************
 *
 ******************************************************************************/
void clear_global_var()
{
    mpPipelineNodeMap = NULL;
    mpStreamInfoSet = NULL;
    mpPipelineDAG = NULL;
    if (test_full) {
    mpPool_HalImage->uninitPool("Tester");
    }
    if (test_resize) {
        mpPool_HalResizeImage->uninitPool("Tester");
    }


    mpPool_HalImage = NULL;
    mpPool_HalResizeImage = NULL;

    mpPool_HalMetadata->uninitPool("Tester");
    mpPool_HalMetadata = NULL;

    mpAppSimulator = NULL;
}

/******************************************************************************
 *
 ******************************************************************************/
void prepareSensor()
{
    IHalSensorList* const pHalSensorList = IHalSensorList::get();
    pHalSensorList->searchSensors();
    mpSensorHalObj = pHalSensorList->createSensor("tester", gSensorId);
    MUINT32    sensorArray[1] = {gSensorId};
    mpSensorHalObj->powerOn("tester", 1, &sensorArray[0]);
}

/******************************************************************************
 *
 ******************************************************************************/
void closeSensor()
{
    MUINT32    sensorArray[1] = {gSensorId};
    mpSensorHalObj->powerOff("tester", 1, &sensorArray[0]);
}


/******************************************************************************
 *
 ******************************************************************************/
void setConfig(P1Node::ConfigParams &params)
{
    params.pInAppMeta = mpAppMetaRequest;
    params.pOutAppMeta = mpAppMetaResult;
    params.pOutHalMeta= mpHalMetaPlatform;
    params.pvOutImage_full = mpvHalImageRaw;
    params.pOutImage_resizer = mpHalImageResizeRaw;
    params.sensorParams = mSensorParams;
    params.pStreamPool_full = mpPool_HalImage;
    params.pStreamPool_resizer = mpPool_HalResizeImage;
}

/******************************************************************************
 *
 ******************************************************************************/
void setConfig(P2Node::ConfigParams &params)
{
    params.pInAppMeta = mpAppMetaRequest;
    if (test_full) {
        params.pvInFullRaw.push_back(mpvHalImageRaw[0]);
    }
    if (test_resize) {
        params.pInResizedRaw = mpHalImageResizeRaw;
    }
    params.vOutImage = mpvImageYUV;
    params.pOutAppMeta = mpApp2MetaResult;
    params.pInHalMeta = mpHalMetaPlatform;
}


/******************************************************************************
 *
 ******************************************************************************/
void createSensorParam(MUINT sensor_mode)
{
    IHalSensorList* const pHalSensorList = IHalSensorList::get();
    NSCam::SensorStaticInfo sensorStaticInfo[1];
    pHalSensorList->querySensorStaticInfo(NSCam::SENSOR_DEV_MAIN,&sensorStaticInfo[0]);
    mSensorParams.mode = sensor_mode;
    if (sensor_mode == SENSOR_SCENARIO_ID_NORMAL_PREVIEW) {
        mSensorParams.size = MSize(sensorStaticInfo[0].previewWidth, sensorStaticInfo[0].previewHeight);
    } else if (sensor_mode == SENSOR_SCENARIO_ID_NORMAL_CAPTURE) {
        mSensorParams.size = MSize(sensorStaticInfo[0].captureWidth, sensorStaticInfo[0].captureHeight);
    }
}


/******************************************************************************
 *
 ******************************************************************************/
void createFullImageStream(MSize size)
{
    MINT const format = eImgFmt_BAYER10;
    MUINT const usage = eBUFFER_USAGE_SW_READ_OFTEN |
                        eBUFFER_USAGE_HW_CAMERA_READWRITE
                        ;
    IImageStreamInfo::BufPlanes_t bufPlanes;
    IImageStreamInfo::BufPlane bufPlane;
    //
    bufPlane.sizeInBytes = size.w * size.h * 10 / 8;
    bufPlane.rowStrideInBytes = size.w * 10 / 8;
    bufPlanes.push_back(bufPlane);
    //
    sp<IImageStreamInfo>
    pStreamInfo = new ImageStreamInfo(
        "Hal:Image:Full RAW",
        STREAM_ID_RAW1,
        eSTREAMTYPE_IMAGE_INOUT,//???
        12, 12,
        usage, format, size, bufPlanes
    );
    mpvHalImageRaw.push_back(pStreamInfo);
    //
    //
    size_t bufStridesInBytes[3] = {0};
    size_t bufBoundaryInBytes[3]= {0};
    for (size_t i = 0; i < bufPlanes.size(); i++) {
        bufStridesInBytes[i] = bufPlanes[i].rowStrideInBytes;
    }
    IIonImageBufferHeap::AllocImgParam_t const allocImgParam(
        format, size,
        bufStridesInBytes, bufBoundaryInBytes,
        bufPlanes.size()
    );

    mpPool_HalImage = new HalImageStreamBuffer::Allocator::StreamBufferPoolT(
        pStreamInfo->getStreamName(),
        HalImageStreamBuffer::Allocator(pStreamInfo.get(), allocImgParam)
    );
    MERROR err = mpPool_HalImage->initPool("Tester", pStreamInfo->getMaxBufNum(), pStreamInfo->getMinInitBufNum());
    if  ( err ) {
        MY_LOGE("mpPool_HalImage init fail");
    }
}


/******************************************************************************
 *
 ******************************************************************************/
void createYUVImageStream(MSize imgSize)
{
    MINT const format = eImgFmt_YV12;
    MUINT const usage = eBUFFER_USAGE_SW_READ_OFTEN |
                        eBUFFER_USAGE_HW_CAMERA_READWRITE
                        ;
    IImageStreamInfo::BufPlanes_t bufPlanes;
    IImageStreamInfo::BufPlane bufPlane;
    //
    size_t const y_stride = imgSize.w;
    size_t const c_stride = y_stride / 2;
    //
    bufPlane.rowStrideInBytes = y_stride;
    bufPlane.sizeInBytes = y_stride * imgSize.h;
    bufPlanes.push_back(bufPlane);
    //
    bufPlane.rowStrideInBytes = c_stride;
    bufPlane.sizeInBytes = c_stride * imgSize.h / 2;
    bufPlanes.push_back(bufPlane);  //V
    bufPlanes.push_back(bufPlane);  //U
    //
    sp<ImageStreamInfo>
    pStreamInfo = new ImageStreamInfo(
        "App:Image:YUV1",
        STREAM_ID_YUV1,
        eSTREAMTYPE_IMAGE_OUT,
        5, 1,
        usage, format, imgSize, bufPlanes
    );
    mpvImageYUV.push_back(pStreamInfo);
}

/******************************************************************************
 *
 ******************************************************************************/
void createResizerImageStream(MSize size)
{
    MINT const format = eImgFmt_BAYER10;
    MUINT const usage = eBUFFER_USAGE_SW_READ_OFTEN |
                        eBUFFER_USAGE_HW_CAMERA_READWRITE
                        ;
    IImageStreamInfo::BufPlanes_t bufPlanes;
    IImageStreamInfo::BufPlane bufPlane;
    //
    bufPlane.sizeInBytes = size.w * size.h * 10 / 8;
    bufPlane.rowStrideInBytes = size.w * 10 / 8;
    bufPlanes.push_back(bufPlane);
    //
    sp<IImageStreamInfo>
    pStreamInfo = new ImageStreamInfo(
        "Hal:Image:Resizer RAW",
        STREAM_ID_RAW2,
        eSTREAMTYPE_IMAGE_INOUT,//???
        12, 12,
        usage, format, size, bufPlanes
    );
    mpHalImageResizeRaw = pStreamInfo;
    //
    //
    size_t bufStridesInBytes[3] = {0};
    size_t bufBoundaryInBytes[3]= {0};
    for (size_t i = 0; i < bufPlanes.size(); i++) {
        bufStridesInBytes[i] = bufPlanes[i].rowStrideInBytes;
    }
    IIonImageBufferHeap::AllocImgParam_t const allocImgParam(
        format, size,
        bufStridesInBytes, bufBoundaryInBytes,
        bufPlanes.size()
    );

    mpPool_HalResizeImage = new HalImageStreamBuffer::Allocator::StreamBufferPoolT(
        pStreamInfo->getStreamName(),
        HalImageStreamBuffer::Allocator(pStreamInfo.get(), allocImgParam)
    );
    MERROR err = mpPool_HalResizeImage->initPool("Tester", pStreamInfo->getMaxBufNum(), pStreamInfo->getMinInitBufNum());
    if  ( err ) {
        MY_LOGE("mpPool_HalImage init fail");
    }
}


/******************************************************************************
 *
 ******************************************************************************/
void createHalOutMetaStream()
{
    sp<IMetaStreamInfo>
    pStreamInfo = new MetaStreamInfo(
        "Hal out Metadata",
        STREAM_ID_HALMETADATA1,
        eSTREAMTYPE_META_INOUT,
        5, 1
    );
    mpHalMetaPlatform = pStreamInfo;

    mpPool_HalMetadata = new HalMetaStreamBuffer::Allocator::StreamBufferPoolT(
        pStreamInfo->getStreamName(),
        HalMetaStreamBuffer::Allocator(pStreamInfo.get())
    );
    MERROR err = mpPool_HalMetadata->initPool("Tester", pStreamInfo->getMaxBufNum(), pStreamInfo->getMinInitBufNum());
    if  ( err ) {
        MY_LOGE("mpPool_HalMetadata");
    }
}


/******************************************************************************
 *
 ******************************************************************************/
void creatAppInMetaStream()
{
    sp<MetaStreamInfo>
    pStreamInfo = new MetaStreamInfo(
        "App in Request",
        STREAM_ID_INMETADATA,
        eSTREAMTYPE_META_IN,
        5, 1
    );
    mpAppMetaRequest = pStreamInfo;
}


/******************************************************************************
 *
 ******************************************************************************/
void createAppOutMetaStream()
{
    sp<IMetaStreamInfo>
    pStreamInfo = new MetaStreamInfo(
        "App out Metadata",
        STREAM_ID_APPMETADATA1,
        eSTREAMTYPE_META_OUT,
        5, 1
    );
    mpAppMetaResult = pStreamInfo;
}

/******************************************************************************
 *
 ******************************************************************************/
void createApp2OutMetaStream()
{
    sp<IMetaStreamInfo>
    pStreamInfo = new MetaStreamInfo(
        "App out 2 Metadata",
        STREAM_ID_APPMETADATA2,
        eSTREAMTYPE_META_OUT,
        5, 1
    );
    mpApp2MetaResult = pStreamInfo;
}

/******************************************************************************
 *
 ******************************************************************************/
void createDAG()
{
    mpPipelineNodeMap = IPipelineNodeMapControl::create();
    mpPipelineDAG = IPipelineDAG::create();

    mpPipelineNodeMap->setCapacity(3);
    //
    {
        mpPipelineDAG->addNode(NODE_ID_NODE1);
        ssize_t const tmpNodeIndex = mpPipelineNodeMap->add(NODE_ID_NODE1, mpNode1);
    }
    //
    {
        mpPipelineDAG->addNode(NODE_ID_NODE2);
        ssize_t const tmpNodeIndex = mpPipelineNodeMap->add(NODE_ID_NODE2, mpNode2);


        sp<IStreamInfoSetControl> const&
        rpInpStreams = mpPipelineNodeMap->getNodeAt(tmpNodeIndex)->editInStreams();
        sp<IStreamInfoSetControl> const&
        rpOutStreams = mpPipelineNodeMap->getNodeAt(tmpNodeIndex)->editOutStreams();
        //
        //  [input]
        //      App:Meta:Request
        //      Hal:Image:P2
        //  [output]
        //      App:Meta:Result
        //
        rpInpStreams->editAppMeta().addStream(mpAppMetaRequest);
        rpInpStreams->editHalMeta().addStream(mpHalMetaPlatform);
        if (test_full) {
            rpInpStreams->editHalImage().addStream(mpvHalImageRaw[0]);
        }
        if (test_resize) {
            rpInpStreams->editHalImage().addStream(mpHalImageResizeRaw);
        }
        //
        rpOutStreams->editAppMeta().addStream(mpApp2MetaResult);
        rpOutStreams->editAppImage().addStream(mpvImageYUV[0]);
    }
    //
    {
        mpPipelineDAG->addNode(NODE_ID_FAKE);
        ssize_t const tmpNodeIndex = mpPipelineNodeMap->add(NODE_ID_FAKE, mpFakeNode);
    }


    mpPipelineDAG->addEdge(mpNode1->getNodeId(), mpNode2->getNodeId());
    mpPipelineDAG->addEdge(mpNode2->getNodeId(), mpFakeNode->getNodeId());
    mpPipelineDAG->setRootNode(mpNode1->getNodeId());
    for (size_t i = 0; i < mpPipelineNodeMap->size(); i++)
    {
        mpPipelineDAG->setNodeValue(mpPipelineNodeMap->nodeAt(i)->getNodeId(), i);
    }
}


/******************************************************************************
 *
 ******************************************************************************/
void prepareConfig(MUINT sensor_mode)
{
    printf("prepareConfig + \n");

    //
    createSensorParam(sensor_mode);

    //create streamPool
    //create out imagebuffer
    printf("create image buffer\n");
    if (test_full) {
        createFullImageStream(mSensorParams.size);
    }

    if (test_resize) {
        createResizerImageStream(MSize(mSensorParams.size.w*7/10, mSensorParams.size.h));
    }

    createYUVImageStream(MSize(1280, 720));


    //create out hal metadata
    printf("create out hal metadata\n");
    createHalOutMetaStream();

    //create out app metadata
    printf("create out app metadata\n");
    createAppOutMetaStream();
    createApp2OutMetaStream();

    //create in app metadata
    printf("create in app metadata\n");
    creatAppInMetaStream();

    ////////////////////////////////////////////////////////////////////////////
    //
    /////////////////////////////////////////////////////////////////////////////
    printf("add stream to streamInfoset\n");
    mpStreamInfoSet = IStreamInfoSetControl::create();

    if (test_full) {
        for(size_t i = 0; i < mpvHalImageRaw.size(); i++) {
            mpStreamInfoSet->editHalImage().addStream(mpvHalImageRaw[i]);
        }
    }
    if (test_resize) {
        mpStreamInfoSet->editHalImage().addStream(mpHalImageResizeRaw);
    }

    mpStreamInfoSet->editAppImage().addStream(mpvImageYUV[0]);
    mpStreamInfoSet->editHalMeta().addStream(mpHalMetaPlatform);
    mpStreamInfoSet->editAppMeta().addStream(mpAppMetaResult);
    mpStreamInfoSet->editAppMeta().addStream(mpApp2MetaResult);
    mpStreamInfoSet->editAppMeta().addStream(mpAppMetaRequest);

    ////////////////////////////////////////////////////////////////////////////
    //
    /////////////////////////////////////////////////////////////////////////////
    printf("add stream to pipelineNodeMap\n");
    createDAG();

    mpAppSimulator = new AppSimulator;

    printf("prepareConfig - \n");
}


/******************************************************************************
 *
 ******************************************************************************/
void setupInfoIOMap(android::sp<IPipelineFrameControl> &pFrame,
                    IPipelineNode::NodeId_T const nodeId)
{
    //
    IPipelineFrame::InfoIOMapSet aInfoIOMapSet;
    IPipelineFrame::ImageInfoIOMapSet& rImageInfoIOMapSet = aInfoIOMapSet.mImageInfoIOMapSet;
    IPipelineFrame::MetaInfoIOMapSet&  rMetaInfoIOMapSet  = aInfoIOMapSet.mMetaInfoIOMapSet;
    //
    //
    sp<IPipelineNodeMapControl::INode> pNodeExt = getPipelineNodeMapControl()->getNodeFor(nodeId);
    sp<IStreamInfoSet const> pInStream = pNodeExt->getInStreams();
    sp<IStreamInfoSet const> pOutStream= pNodeExt->getOutStreams();
    //
    //  Image
    {
        ssize_t index = rImageInfoIOMapSet.add();
        if (index < 0) {
            MY_LOGE("rImageInfoIOMapSet.add fail");
            return;
        }
        IPipelineFrame::ImageInfoIOMap& rMap =
        rImageInfoIOMapSet.editItemAt(index);
        //
        //Input
        for (size_t i = 0; i < pInStream->getImageInfoNum(); i++)
        {

            sp<IImageStreamInfo> p = pInStream->getImageInfoAt(i);
            rMap.vIn.add(p->getStreamId(), p);

        }


        //Output
        for (size_t i = 0; i < pOutStream->getImageInfoNum(); i++)
        {
            sp<IImageStreamInfo> p = pOutStream->getImageInfoAt(i);
            rMap.vOut.add(p->getStreamId(), p);
        }
    }
    //
    //  Meta
    {
        ssize_t index = rMetaInfoIOMapSet.add();
        if (index < 0) {
            MY_LOGE("rMetaInfoIOMapSet.add fail");
            return;
        }
        IPipelineFrame::MetaInfoIOMap& rMap =
        rMetaInfoIOMapSet.editItemAt(index);
        //
        //Input
        //[TODO] sensor stream

        //Input
        for (size_t i = 0; i < pInStream->getMetaInfoNum(); i++)
        {
            sp<IMetaStreamInfo> p = pInStream->getMetaInfoAt(i);
            rMap.vIn.add(p->getStreamId(), p);
            //printf("streamId: %d\n", p->getStreamId());
        }


        //
        //Output
        for (size_t i = 0; i < pOutStream->getMetaInfoNum(); i++)
        {
            sp<IMetaStreamInfo> p = pOutStream->getMetaInfoAt(i);
            rMap.vOut.add(p->getStreamId(), p);
        }
    }
    //
    //
    pFrame->addInfoIOMapSet(nodeId, aInfoIOMapSet);
}


/******************************************************************************
 *
 ******************************************************************************/
void setupBufferSetControl_fullraw(sp<IStreamBufferSetControl> &pBufferSetControl)
{
    sp<HalImageStreamBuffer> pStreamBuffer;

    StreamId_T const streamId = STREAM_ID_RAW1;
    //
    sp<IImageStreamInfo> pStreamInfo = getStreamInfoSet()->getImageInfoFor(streamId);

    //
    //acquireFromPool
    MY_LOGD("[acquireFromPool] + %s ", pStreamInfo->getStreamName());
    MERROR err = mpPool_HalImage->acquireFromPool(
        "Tester", pStreamBuffer, ::s2ns(30)
    );
    MY_LOGD("[acquireFromPool] - %s %p err:%d", pStreamInfo->getStreamName(), pStreamBuffer.get(), err);
    MY_LOGE_IF(OK!=err || pStreamBuffer==0, "pStreamBuffer==0");
    //
    ssize_t userGroupIndex = 0;
    //User Group1
    {
        sp<IUsersManager::IUserGraph> pUserGraph = pStreamBuffer->createGraph();
        IUsersManager::User user;
        //
        user.mUserId = NODE_ID_NODE1;
        user.mCategory = IUsersManager::Category::PRODUCER;
        user.mUsage = pStreamInfo->getUsageForAllocator();
        pUserGraph->addUser(user);
        //
        IUsersManager::User user2;
        //
        user2.mUserId = NODE_ID_NODE2;
        user2.mCategory = IUsersManager::Category::CONSUMER;
        user2.mUsage = pStreamInfo->getUsageForAllocator();
        pUserGraph->addUser(user2);
        //
        userGroupIndex = pStreamBuffer->enqueUserGraph(pUserGraph.get());
    }
    //
    //
    pBufferSetControl->editMap_HalImage()->add(pStreamBuffer);

}


/******************************************************************************
 *
 ******************************************************************************/
void setupBufferSetControl_YUV(sp<IStreamBufferSetControl> &pBufferSetControl)
{
    //APP/Image/YUV
    StreamId_T const streamId = STREAM_ID_YUV1;
    //
    sp<IImageStreamInfo> pStreamInfo = getStreamInfoSet()->getImageInfoFor(streamId);
    sp<HalImageStreamBuffer> pStreamBuffer;
    //
    IImageStreamInfo::BufPlanes_t bufPlanes = pStreamInfo->getBufPlanes();
    //
    size_t bufStridesInBytes[3] = {0};
    size_t bufBoundaryInBytes[3]= {0};
    for (size_t i = 0; i < bufPlanes.size(); i++) {
        bufStridesInBytes[i] = bufPlanes[i].rowStrideInBytes;
    }
    IIonImageBufferHeap::AllocImgParam_t const allocImgParam(
        pStreamInfo->getImgFormat(), pStreamInfo->getImgSize(),
        bufStridesInBytes, bufBoundaryInBytes,
        bufPlanes.size()
    );
    //
    typedef NSCam::v3::Utils::HalImageStreamBuffer::Allocator StreamBufferAllocatorT;
    pStreamBuffer = StreamBufferAllocatorT(pStreamInfo.get(), allocImgParam)();
    MY_LOGE_IF(pStreamBuffer==0, "pStreamBuffer==0");
    //
    ssize_t userGroupIndex = 0;
    //User Group1
    {
        sp<IUsersManager::IUserGraph> pUserGraph = pStreamBuffer->createGraph();
        IUsersManager::User user;
        //
        user.mUserId = NODE_ID_NODE2;
        user.mCategory = IUsersManager::Category::PRODUCER;
        user.mUsage = pStreamInfo->getUsageForAllocator();
        pUserGraph->addUser(user);
        //
        userGroupIndex = pStreamBuffer->enqueUserGraph(pUserGraph.get());
    }
    //
    pBufferSetControl->editMap_AppImage()->add(pStreamBuffer);
}


/******************************************************************************
 *
 ******************************************************************************/
void setupBufferSetControl_resizeraw(sp<IStreamBufferSetControl> &pBufferSetControl)
{
    sp<HalImageStreamBuffer> pStreamBuffer;

    StreamId_T const streamId = STREAM_ID_RAW2;
    //
    sp<IImageStreamInfo> pStreamInfo = getStreamInfoSet()->getImageInfoFor(streamId);

    //
    //acquireFromPool
    MY_LOGD("[acquireFromPool] + %s ", pStreamInfo->getStreamName());
    MERROR err = mpPool_HalResizeImage->acquireFromPool(
        "Tester", pStreamBuffer, ::s2ns(300)
    );
    MY_LOGD("[acquireFromPool] - %s %p err:%d", pStreamInfo->getStreamName(), pStreamBuffer.get(), err);
    MY_LOGE_IF(OK!=err || pStreamBuffer==0, "pStreamBuffer==0");
    //
    ssize_t userGroupIndex = 0;
    //User Group1
    {
        sp<IUsersManager::IUserGraph> pUserGraph = pStreamBuffer->createGraph();
        IUsersManager::User user;
        //
        user.mUserId = NODE_ID_NODE1;
        user.mCategory = IUsersManager::Category::PRODUCER;
        user.mUsage = pStreamInfo->getUsageForAllocator();
        pUserGraph->addUser(user);
        //
        user.mUserId = NODE_ID_NODE2;
        user.mCategory = IUsersManager::Category::CONSUMER;
        user.mUsage = pStreamInfo->getUsageForAllocator();
        pUserGraph->addUser(user);
        //
        userGroupIndex = pStreamBuffer->enqueUserGraph(pUserGraph.get());
    }
    //
    //
    pBufferSetControl->editMap_HalImage()->add(pStreamBuffer);
}


/******************************************************************************
 *
 ******************************************************************************/
void setupBufferSetControl_halMeta(sp<IStreamBufferSetControl> &pBufferSetControl)
{
    StreamId_T const streamId = STREAM_ID_HALMETADATA1;
    //
    sp<IMetaStreamInfo> pStreamInfo = getStreamInfoSet()->getMetaInfoFor(streamId);
    sp<HalMetaStreamBuffer> pStreamBuffer;
    //
    //acquireFromPool
    MY_LOGD("[acquireFromPool] + %s ", pStreamInfo->getStreamName());
    MERROR err = mpPool_HalMetadata->acquireFromPool(
        "Tester", pStreamBuffer, ::s2ns(300)
    );
    MY_LOGD("[acquireFromPool] - %s %p err:%d", pStreamInfo->getStreamName(), pStreamBuffer.get(), err);
    MY_LOGE_IF(OK!=err || pStreamBuffer==0, "pStreamBuffer==0");
    //
    ssize_t userGroupIndex = 0;
    //User Group1 ///WTF ???
    {
        sp<IUsersManager::IUserGraph> pUserGraph = pStreamBuffer->createGraph();
        IUsersManager::User user;
        //
        user.mUserId = NODE_ID_NODE1;
        user.mCategory = IUsersManager::Category::PRODUCER;
        user.mUsage = 0;
        pUserGraph->addUser(user);

        user.mUserId = NODE_ID_NODE2;
        user.mCategory = IUsersManager::Category::CONSUMER;
        user.mUsage = 0;
        pUserGraph->addUser(user);

        userGroupIndex = pStreamBuffer->enqueUserGraph(pUserGraph.get());
    }
    //
    pBufferSetControl->editMap_HalMeta()->add(pStreamBuffer);
}


/******************************************************************************
 *
 ******************************************************************************/
void setupBufferSetControl_resultMeta(sp<IStreamBufferSetControl> &pBufferSetControl,
                                          StreamId_T streamId, MUINT32 node)
{
    //
    sp<IMetaStreamInfo> pStreamInfo = getStreamInfoSet()->getMetaInfoFor(streamId);
    sp<IMetaStreamBuffer> pStreamBuffer;
    //
    //alloc without default value
    typedef NSCam::v3::Utils::HalMetaStreamBuffer::Allocator StreamBufferAllocatorT;
    pStreamBuffer = StreamBufferAllocatorT(pStreamInfo.get())();
    MY_LOGE_IF(pStreamBuffer==0, "pStreamBuffer==0");
    //
    ssize_t userGroupIndex = 0;
    //User Group1 //WFK????
    {
        sp<IUsersManager::IUserGraph> pUserGraph = pStreamBuffer->createGraph();
        IUsersManager::User user;
        //
        user.mUserId = node;
        user.mCategory = IUsersManager::Category::PRODUCER;
        pUserGraph->addUser(user);
        //
        userGroupIndex = pStreamBuffer->enqueUserGraph(pUserGraph.get());
    }
    //
    pBufferSetControl->editMap_AppMeta()->add(pStreamBuffer);
}


/******************************************************************************
 *
 ******************************************************************************/
void setupBufferSetControl_requestMeta(sp<IStreamBufferSetControl> &pBufferSetControl)
{
    StreamId_T const streamId = STREAM_ID_INMETADATA;
    //
    sp<IMetaStreamInfo> pStreamInfo = getStreamInfoSet()->getMetaInfoFor(streamId);
    sp<IMetaStreamBuffer> pStreamBuffer;
    //
    IMetadata appRequest;
    {
        IMetadata::IEntry entry1(MTK_SCALER_AVAILABLE_MAX_DIGITAL_ZOOM);
        entry1.push_back(16, Type2Type< MFLOAT >());
        appRequest.update(MTK_SCALER_AVAILABLE_MAX_DIGITAL_ZOOM, entry1);
    }

    typedef NSCam::v3::Utils::HalMetaStreamBuffer::Allocator StreamBufferAllocatorT;
    pStreamBuffer = StreamBufferAllocatorT(pStreamInfo.get())(appRequest);
    MY_LOGE_IF(pStreamBuffer==0, "pStreamBuffer==0");
    //
    ssize_t userGroupIndex = 0;
    //User Group1
    {
        sp<IUsersManager::IUserGraph> pUserGraph = pStreamBuffer->createGraph();
        IUsersManager::User user;
        //
        user.mUserId = NODE_ID_NODE1;
        user.mCategory = IUsersManager::Category::CONSUMER;
        pUserGraph->addUser(user);
        //
        user.mUserId = NODE_ID_NODE2;
        user.mCategory = IUsersManager::Category::CONSUMER;
        pUserGraph->addUser(user);
        //
        userGroupIndex = pStreamBuffer->enqueUserGraph(pUserGraph.get());
    }
    //
    pBufferSetControl->editMap_AppMeta()->add(pStreamBuffer);

}

/******************************************************************************
 *
 ******************************************************************************/
void
prepareRequest(android::sp<IPipelineFrameControl> &pFrame, MINT32 frameNo)
{
    printf("prepare request\n");

    pFrame = IPipelineFrameControl::create(frameNo);
    pFrame->setPipelineNodeMap(getPipelineNodeMapControl());
    pFrame->setPipelineDAG(getPipelineDAG());
    pFrame->setStreamInfoSet(getStreamInfoSet());

    //P1 Node
    setupInfoIOMap(pFrame, NODE_ID_NODE1);

    //P2 Node
    setupInfoIOMap(pFrame, NODE_ID_NODE2);


    //
    //FAKE Node
    {
    // do nothing
    }

    sp<IStreamBufferSetControl> pBufferSetControl = IStreamBufferSetControl::create(
        frameNo, NULL
    );
    //
    //

    //RAW
    if (test_full) {
        setupBufferSetControl_fullraw(pBufferSetControl);
    }

    if (test_resize) {
        setupBufferSetControl_resizeraw(pBufferSetControl);
    }

    //YUV
    setupBufferSetControl_YUV(pBufferSetControl);

    //HAL/Meta/PLATFORM
    setupBufferSetControl_halMeta(pBufferSetControl);

    //App:Meta:Result
    setupBufferSetControl_requestMeta(pBufferSetControl);

    //APP/Meta/Request
    setupBufferSetControl_resultMeta(pBufferSetControl, STREAM_ID_APPMETADATA1, NODE_ID_NODE1);
    setupBufferSetControl_resultMeta(pBufferSetControl, STREAM_ID_APPMETADATA2, NODE_ID_NODE2);

    //
    pFrame->setStreamBufferSet(pBufferSetControl);
}


/******************************************************************************
 *
 ******************************************************************************/
int main(int argc, char** argv)
{
    MY_LOGD("start test");

    if (argc < 3) return 0;

    test_full = atoi(argv[1]) > 0 ? true : false;
    test_resize = atoi(argv[2]) > 0 ? true : false;

    //sensor
    prepareSensor();

    //
    mpNode1 = P1Node::createInstance();
    mpNode2 = P2Node::createInstance(P2Node::PASS2_STREAM);
    mpFakeNode = new FakeNode();
    //init
    {
         struct  P1Node::InitParams params;
         params.openId = gSensorId;
         params.nodeName = "Tester1";
         params.nodeId = NODE_ID_NODE1;
         mpNode1->init(params);
    };


    {
         struct  P2Node::InitParams params;
         params.openId = gSensorId;
         params.nodeName = "Tester2";
         params.nodeId = NODE_ID_NODE2;
         mpNode2->init(params);
    };
    //----------test 1 ------------//

    //config
    {
       prepareConfig(SENSOR_SCENARIO_ID_NORMAL_PREVIEW);
       P1Node::ConfigParams params;
       setConfig(params);
       mpNode1->config(params);
    }

    {
       P2Node::ConfigParams params;
       setConfig(params);
       mpNode2->config(params);
    }


    //request
    for (MINT32 frameNo = 0; frameNo < 30; frameNo++)
    {
        MY_LOGD("[frameNo:%d] + submitRequest", frameNo);
        android::sp<IPipelineFrameControl> pFrame;
        prepareRequest(pFrame, frameNo);
        mpNode1->queue(pFrame);
        MY_LOGD("[frameNo:%d] - submitRequest", frameNo);
    }

    //flush
    printf("flush\n");
    mpNode1->flush();
    mpNode2->flush();


    //----------test 2------------//

    clear_global_var();

    //config
    {
       prepareConfig(SENSOR_SCENARIO_ID_NORMAL_CAPTURE);
       P1Node::ConfigParams params;
       setConfig(params);
       mpNode1->config(params);
    }

    {
       P2Node::ConfigParams params;
       setConfig(params);
       mpNode2->config(params);
    }


    //request
    for (MINT32 frameNo = 0; frameNo < 30; frameNo++)
    {
        MY_LOGD("[frameNo:%d] + submitRequest", frameNo);
        android::sp<IPipelineFrameControl> pFrame;
        prepareRequest(pFrame, frameNo);
        mpNode1->queue(pFrame);
        MY_LOGD("[frameNo:%d] - submitRequest", frameNo);
    }

    //flush
    printf("flush\n");
    mpNode1->flush();
    mpNode2->flush();


    //uninit
    printf("uninit\n");
    mpNode1->uninit();
    mpNode2->uninit();


    closeSensor();
    clear_global_var();

    MY_LOGD("end of test");
    return 0;
}

