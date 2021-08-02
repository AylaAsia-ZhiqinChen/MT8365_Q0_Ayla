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

#include <sys/stat.h>
#include <plugin/PipelinePluginType.h>
#include "MTKFaceBeauty.h"
#include "MTKFaceBeautyErrCode.h"
#include "MTKFaceBeautyType.h"
#include "BufferUtils.h"
#include "LogUtils.h"
#include "bmp.h"
#include "crz.h"
#include "mtk/mtk_platform_metadata_tag.h"
#include "mtk/mtk_feature_type.h"
#include "mtk/mtk_feature_type.h"
using namespace NSCam;
using namespace android;
using namespace std;
using namespace NSCam::NSPipelinePlugin;

#define LOG_TAG "PostAlgo/FBProviderImpl"
#define INPUT_YUV420
//#define DEBUG_FB
//#define DEBUG_FB_FACE_RECT
/******************************************************************************
 *
 ******************************************************************************/
class FBProviderImpl : public YuvPlugin::IProvider
{
    typedef YuvPlugin::Property Property;
    typedef YuvPlugin::Selection Selection;
    typedef YuvPlugin::Request::Ptr RequestPtr;
    typedef YuvPlugin::RequestCallback::Ptr RequestCallbackPtr;

    public:

    virtual void set(MINT32 iOpenId, MINT32 iOpenId2)
    {
        FUNCTION_IN;
        MY_LOGD("set openId:%d openId2:%d", iOpenId, iOpenId2);
        mOpenid = iOpenId;
        FUNCTION_OUT;
    }

    virtual const Property& property()
    {
        FUNCTION_IN;
        static Property prop;
        static bool inited;

        if (!inited) {
            prop.mName = "MTK FB";
            prop.mFeatures = MTK_FEATURE_FB;
            prop.mInPlace = MFALSE;
            prop.mFaceData = eFD_Current;
            prop.mPosition = 0;
            inited = true;
        }
        FUNCTION_OUT;
        return prop;
    };

    virtual MERROR negotiate(Selection& sel)
    {
        FUNCTION_IN;
        sel.mIBufferFull.setRequired(MTRUE)
            .addAcceptedFormat(eImgFmt_YV12)
            .addAcceptedSize(eImgSize_Full);
        sel.mOBufferFull.setRequired(MTRUE)
            .addAcceptedFormat(eImgFmt_YV12)
            .addAcceptedSize(eImgSize_Full);

        sel.mIMetadataDynamic.setRequired(MFALSE);
        sel.mIMetadataApp.setRequired(MTRUE);
        sel.mIMetadataHal.setRequired(MFALSE);
        sel.mOMetadataApp.setRequired(MTRUE);
        sel.mOMetadataHal.setRequired(MFALSE);

        FUNCTION_OUT;
        return OK;
    };

    virtual void init()
    {
        FUNCTION_IN;
        FUNCTION_OUT;
    };

    virtual MERROR process(RequestPtr pRequest,
            RequestCallbackPtr pCallback = nullptr)
    {
        MY_LOGD("process FBProviderImpl >>>>>>>>>>>>>>>>>>>");
        FUNCTION_IN;
        IImageBuffer *in = NULL, *out = NULL;
        IMetadata *pIMetataHAL = NULL;

        if (pRequest->mIBufferFull != nullptr) {
            in = pRequest->mIBufferFull->acquire();
        }

        if (pRequest->mOBufferFull != nullptr) {
            out = pRequest->mOBufferFull->acquire();
            //MY_LOGD("[OUT] Full image VA: 0x%p", pImgBuffer->getBufVA(0));
        }

        if (pRequest->mIMetadataDynamic != nullptr) {
            IMetadata *meta = pRequest->mIMetadataDynamic->acquire();
            if (meta != NULL)
                MY_LOGD("[IN] Dynamic metadata count: %d", meta->count());
            else
                MY_LOGD("[IN] Dynamic metadata empty");
        }

        if (pRequest->mIMetadataHal != nullptr) {
            pIMetataHAL = pRequest->mIMetadataHal->acquire();
            if (pIMetataHAL != NULL)
                MY_LOGD("[IN] HAL metadata count: %d", pIMetataHAL->count());
            else
                MY_LOGD("[IN] HAL metadata empty");
        }

        if (in != NULL && out != NULL)
        {
            IMetadata* pImetadata = pRequest->mIMetadataApp->acquire();

            pFaceBeautyInterface = MTKFaceBeauty::createInstance(DRV_FACEBEAUTY_OBJ_SW);
            MTKFaceBeautyEnvInfo FaceBeautyEnvInfo;
            MTKFaceBeautyProcInfo FaceBeautyProcInfo;
            MTKFaceBeautyResultInfo FaceBeautyResultInfo;
            MTKFaceBeautyTuningPara FaceBeautyTuningInfo;
            MTKFaceBeautyGetProcInfo FaceBeautyGetProcInfo;
            MRESULT Retcode;
            MSize inSize = in->getImgSize();
            gImageWidth = inSize.w;
            gImageHeight = inSize.h;
            MY_LOGD("process gImageWidth = %d, gImageHeight = %d",gImageWidth, gImageHeight);
            gImageNrDsWidth = gImageWidth/2;
            gImageNrDsHeight = gImageHeight/2;
            gImageDsWidth = 320;
            gImageDsHeight = (inSize.h*gImageDsWidth)/inSize.w;

            FaceBeautyProcInfo.Step1SrcImgWidth = gImageNrDsWidth;
            FaceBeautyProcInfo.Step1SrcImgHeight = gImageNrDsHeight;
            FaceBeautyProcInfo.Step2SrcImgWidth = gImageDsWidth;
            FaceBeautyProcInfo.Step2SrcImgHeight = gImageDsHeight;
            FaceBeautyProcInfo.SrcImgWidth = gImageWidth;
            FaceBeautyProcInfo.SrcImgHeight = gImageHeight;
            //FaceBeautyProcInfo.ExtremeMode = 0;
            FaceBeautyProcInfo.ExtremeMode = 1;
            FaceBeautyProcInfo.PMode = 0;
            Retcode = pFaceBeautyInterface->FaceBeautyFeatureCtrl(MTKFACEBEAUTY_FEATURE_GET_PROC_INFO, &FaceBeautyProcInfo, &FaceBeautyGetProcInfo);

            WorkingBuffer = new MUINT8[FaceBeautyGetProcInfo.WorkingBufferSize];
            WorkingBufferSize =  FaceBeautyGetProcInfo.WorkingBufferSize;
            gFDHeight = (inSize.h*gFDWidth)/inSize.w;
            // Set Environment Info into FB Driver

            FaceBeautyEnvInfo.Step1SrcImgWidth = gImageNrDsWidth;
            FaceBeautyEnvInfo.Step1SrcImgHeight = gImageNrDsHeight;
            FaceBeautyEnvInfo.Step2SrcImgWidth = gImageDsWidth;
            FaceBeautyEnvInfo.Step2SrcImgHeight = gImageDsHeight;
            FaceBeautyEnvInfo.SrcImgWidth = gImageWidth;
            FaceBeautyEnvInfo.SrcImgHeight = gImageHeight;
            FaceBeautyEnvInfo.FDWidth = gFDWidth;
            FaceBeautyEnvInfo.FDHeight = gFDHeight;
            FaceBeautyEnvInfo.SrcImgFormat = MTKFACEBEAUTY_IMAGE_YV12;
            FaceBeautyEnvInfo.STEP1_ENABLE = true;
            FaceBeautyEnvInfo.pWorkingBufAddr = (void*)WorkingBuffer;
            FaceBeautyEnvInfo.WorkingBufSize = WorkingBufferSize;

            FaceBeautyEnvInfo.pTuningPara = &FaceBeautyTuningInfo;
            MINT32 value = 0;
            tryGetMetadata<MINT32>(pImetadata, MTK_POSTALGO_FACE_BEAUTY_SMOOTH, value);
            MY_LOGD("process MTK_POSTALGO_FACE_BEAUTY_SMOOTH = %d", value);
            FaceBeautyEnvInfo.pTuningPara->SmoothLevel = value;//gSmoothLevel;

            tryGetMetadata<MINT32>(pImetadata, MTK_POSTALGO_FACE_BEAUTY_BRIGHT, value);
            MY_LOGD("process MTK_POSTALGO_FACE_BEAUTY_BRIGHT = %d", value);
            FaceBeautyEnvInfo.pTuningPara->BrightLevel = value;//gBrightLevel;

            tryGetMetadata<MINT32>(pImetadata, MTK_POSTALGO_FACE_BEAUTY_ENLARGE_EYE, value);
            MY_LOGD("process MTK_POSTALGO_FACE_BEAUTY_ENLARGE_EYE = %d", value);
            FaceBeautyEnvInfo.pTuningPara->EnlargeEyeLevel = value;//gWarpLevel;

            tryGetMetadata<MINT32>(pImetadata, MTK_POSTALGO_FACE_BEAUTY_SLIM_FACE, value);
            MY_LOGD("process MTK_POSTALGO_FACE_BEAUTY_SLIM_FACE = %d", value);
            FaceBeautyEnvInfo.pTuningPara->SlimFaceLevel = value;//gWarpLevel;

            tryGetMetadata<MINT32>(pImetadata, MTK_POSTALGO_FACE_BEAUTY_RUDDY, value);
            MY_LOGD("process MTK_POSTALGO_FACE_BEAUTY_RUDDY = %d", value);
            FaceBeautyEnvInfo.pTuningPara->RuddyLevel = value;//gWarpLevel;

            FaceBeautyEnvInfo.pTuningPara->WarpFaceNum = 1;//gWarpFaceNum;
            FaceBeautyEnvInfo.pTuningPara->MinFaceRatio = 12;
            FaceBeautyEnvInfo.pTuningPara->AlignTH1 = -10000;
            FaceBeautyEnvInfo.pTuningPara->AlignTH2 = -20;
            FaceBeautyEnvInfo.pTuningPara->ContrastLevel = 12;//gContrastLevel;
            Retcode = pFaceBeautyInterface->FaceBeautyInit(&FaceBeautyEnvInfo, 0);
            MY_LOGD("FaceBeautyInit<<<<<<<<<<<<<<");

            ////////////////////////////////////////////////////////////
            ///////////////////////// Step1 ////////////////////////////
            MY_LOGD("process Step1 >>>>>>>>>>>>>");
            if (ImageNrDs420WorkingBuffer == nullptr) {
                ImageNrDs420WorkingBuffer = BufferUtils::acquireWorkingBuffer(MSize(gImageNrDsWidth, gImageNrDsHeight),
                        eImgFmt_YV12);
            }

            BufferUtils::mdpResizeAndConvert(in, ImageNrDs420WorkingBuffer.get());
            // Set Proc Info into FB Driver for Generate NR Image

            unsigned char* gImageNrDs420Buffer = getBuffer(ImageNrDs420WorkingBuffer.get(), 1.5);
#ifdef DEBUG_FB
            YUVBufferSave(getBuffer(in, 1.5),
                    "yuvInbuffer", "YV12", gImageWidth, gImageHeight, gImageWidth*gImageHeight*3/2);
            YUVBufferSave(gImageNrDs420Buffer, "gImageNrDs420Buffer", "YV12",
                    gImageNrDsWidth, gImageNrDsHeight, gImageNrDsWidth*gImageNrDsHeight*3/2);
#endif
            // Set Proc Info into FB Driver for Generate NR Image
            FaceBeautyProcInfo.FaceBeautyCtrlEnum = MTKFACEBEAUTY_CTRL_STEP1;
            FaceBeautyProcInfo.Step1DstImgAddr = NULL;
            FaceBeautyProcInfo.Step1SrcImgAddr = gImageNrDs420Buffer;
            Retcode = pFaceBeautyInterface->FaceBeautyFeatureCtrl(MTKFACEBEAUTY_FEATURE_SET_PROC_INFO, &FaceBeautyProcInfo, 0);
            Retcode = pFaceBeautyInterface->FaceBeautyMain();
            Retcode = pFaceBeautyInterface->FaceBeautyFeatureCtrl(MTKFACEBEAUTY_FEATURE_GET_RESULT, 0, &FaceBeautyResultInfo);
#ifdef DEBUG_FB
            YUVBufferSave(FaceBeautyResultInfo.Step1ResultAddr,
                    "ImageSWNrDs420Buffer", "YV12",gImageDsWidth, gImageDsHeight, gImageDsWidth*gImageDsHeight*2);
#endif
            UpSampleFromDs420ToFull422(FaceBeautyResultInfo.Step1ResultAddr); //////////
#ifdef DEBUG_FB
            YUVBufferSave(gImageSWNRBlurBuffer, "gImageSWNRBlur422Buffer",
                    "422", gImageWidth, gImageHeight, gImageWidth*gImageHeight*2);
#endif
            MY_LOGD("process Step1 <<<<<<<<<<<<<<<");
            ///////////////////////// Step1 ////////////////////////////

            ///////////////////////// Step2 ////////////////////////////
            // Set Proc Info into FB Driver for Face Alignment
            MY_LOGD("process Step2 >>>>>>>>>>>>>");
            getFaceInfo(in);
            MY_LOGD("process gImageDsWidth = %d, gImageDsHeight = %d",
                    gImageDsWidth, gImageDsHeight);
#ifdef DEBUG_FB
            YUVBufferSave(gImageDsBuffer, "gImageDsBuffer", "422",
                    gImageDsWidth,gImageDsHeight, gImageDsWidth*gImageDsHeight*2);
#endif
            FaceBeautyProcInfo.FaceBeautyCtrlEnum = MTKFACEBEAUTY_CTRL_STEP2;
            FaceBeautyProcInfo.Step2SrcImgAddr = gImageDsBuffer; //
            int face_clock_position = getFaceOrientation(pImetadata);
            MY_LOGD("process Step2 face_clock_position = %d", face_clock_position);
            int face_rect_array_length = 4;
            tryGetMetadata<MINT32>(pImetadata, MTK_POSTALGO_FACE_BEAUTY_FACE_DETECTION_SIZE, face_rect_array_length);
            gFaceCount = face_rect_array_length/4;
            MY_LOGD("process Step2 gFaceCount = %d", gFaceCount);
            for(int i = 0; i < gFaceCount; i++)
            {   MRect faceRect;
                tryGetMetadata<MRect>(pImetadata, MTK_POSTALGO_FACE_BEAUTY_FACE_DETECTION, faceRect);
                MPoint leftRightcorner = faceRect.leftTop();
                MY_LOGD("process MTK_POSTALGO_FACE_BEAUTY_FACE_DETECTION = %d point x= %d, y = %d",
                        MTK_POSTALGO_FACE_BEAUTY_FACE_DETECTION, leftRightcorner.x, leftRightcorner.y);
                MSize  faceSize = faceRect.size();
                MY_LOGD("process MTK_POSTALGO_FACE_BEAUTY_FACE_DETECTION_SIZE width = %d, height = %d",
                        faceSize.w, faceSize.h);
                FaceBeautyProcInfo.FDLeftTopPointX1[i] = leftRightcorner.x;
                FaceBeautyProcInfo.FDLeftTopPointY1[i] = leftRightcorner.y;

                FaceBeautyProcInfo.FDBoxSize[i] = faceSize.w;
                FaceBeautyProcInfo.FDPose[i] = face_clock_position; //face orientation
                if(FaceBeautyEnvInfo.pTuningPara -> WarpFaceNum ==1 && i== 0)
                {
                    FaceBeautyProcInfo.fb_pos[i][0] = leftRightcorner.x + faceSize.w * 0.5; // face center position x
                    FaceBeautyProcInfo.fb_pos[i][1] = leftRightcorner.y + faceSize.h * 0.5; // face centor position y
                }
                else if(FaceBeautyEnvInfo.pTuningPara -> WarpFaceNum == 0)
                {
                    FaceBeautyProcInfo.fb_pos[i][0] = leftRightcorner.x + faceSize.w * 0.5;
                    FaceBeautyProcInfo.fb_pos[i][1] = leftRightcorner.y + faceSize.h * 0.5;
                }
            }
            FaceBeautyProcInfo.FaceCount = gFaceCount;
            Retcode = pFaceBeautyInterface -> FaceBeautyFeatureCtrl(MTKFACEBEAUTY_FEATURE_SET_PROC_INFO,
                    &FaceBeautyProcInfo, 0);
            Retcode = pFaceBeautyInterface -> FaceBeautyMain();
            Retcode = pFaceBeautyInterface -> FaceBeautyFeatureCtrl(MTKFACEBEAUTY_FEATURE_GET_RESULT, 0,
                    &FaceBeautyResultInfo);
            ///////////////////////// Step2 ////////////////////////////

            ///////////////////////// Step3 ////////////////////////////
            MY_LOGD("process Step3 >>>>>>>>>>>>>");
            // Set Proc Info into FB Driver for Alpha map
            FaceBeautyProcInfo.FaceBeautyCtrlEnum = MTKFACEBEAUTY_CTRL_STEP3;
            Retcode = pFaceBeautyInterface->FaceBeautyFeatureCtrl(MTKFACEBEAUTY_FEATURE_SET_PROC_INFO,
                    &FaceBeautyProcInfo, 0);

            // Process and Get the FB Down Sampled Alpha Map
            // FaceBeautyResultInfo.AlphaMapDsAddr,
            // FaceBeautyResultInfo.AlphaMapColorDsAddr
            // will be set
            Retcode = pFaceBeautyInterface->FaceBeautyMain();
            Retcode = pFaceBeautyInterface->FaceBeautyFeatureCtrl(MTKFACEBEAUTY_FEATURE_GET_RESULT, 0,
                    &FaceBeautyResultInfo);
            // downsample the ds alpha map and upsample it to full size by resizer
            // gAplhaMapBuffer will be new unsigned char[gImageWidth*gImage Height] in this function
            UpSampleDsAlphaTextureMap((unsigned char*)FaceBeautyResultInfo.Step3ResultAddr_1);
            //SaveLuma(gAplhaMapBuffer, "NR_AlphaMap.y", gImageWidth , gImageHeight);

            ///////////////////////// Step4 ////////////////////////////
            MY_LOGD("process Step4 >>>>>>>>>>>>>");
            // Set Proc Info into FB Driver for texture alpha blending
            FaceBeautyProcInfo.FaceBeautyCtrlEnum = MTKFACEBEAUTY_CTRL_STEP4;
            FaceBeautyProcInfo.SrcImgAddr = (unsigned char*)getBuffer(in, 1.5);
            //FaceBeautyProcInfo.SrcImgBlurAddr = gImageBlurBuffer;
            FaceBeautyProcInfo.Step4SrcImgAddr_1 = gImageSWNRBlurBuffer;
            FaceBeautyProcInfo.Step4SrcImgAddr_2 = gAplhaMapBuffer;
#ifdef DEBUG_FB
            YUVBufferSave(gAplhaMapBuffer, "gAplhaMapBuffer", "Y", gImageWidth, gImageHeight, gImageWidth*gImageHeight);
#endif
            Retcode = pFaceBeautyInterface->FaceBeautyFeatureCtrl(MTKFACEBEAUTY_FEATURE_SET_PROC_INFO,
                    &FaceBeautyProcInfo, 0);
            // SaveResult(gImageSWNRBlurBuffer, "blur.bmp");
            // Process and Get the Texture Smoothed Image
            // FaceBeautyResultInfo.BlendTextureImgAddr
            // will be set
            Retcode = pFaceBeautyInterface->FaceBeautyMain();
            Retcode = pFaceBeautyInterface->FaceBeautyFeatureCtrl(MTKFACEBEAUTY_FEATURE_GET_RESULT, 0, &FaceBeautyResultInfo);
            // SaveResult(FaceBeautyResultInfo.Step4ResultAddr, "NR.bmp");//
            // SaveResult(FaceBeautyResultInfo.Step4ResultAddr, "/storage/emulated/0/FB/NR", gImageWidth*gImageHeight);
            // downsample the ds alpha color map and upsample it to full size by resizer
            // gAplhaMapColorBuffer will be new unsigned char[gImageWidth*gImageHeight] in this function
            UpSampleDsAlphaColorMap((unsigned char*)FaceBeautyResultInfo.Step3ResultAddr_2);
#ifdef DEBUG_FB
            YUVBufferSave(gAplhaMapColorBuffer, "gAplhaMapColorBuffer", "Y", gImageWidth, gImageHeight,
                    gImageWidth*gImageHeight);
#endif
            // SaveLuma(gAplhaMapColorBuffer, "CL_AlphaMap.y", gImageWidth , gImageHeight);

            ///////////////////////// Step5 ////////////////////////////
            MY_LOGD("process Step5 >>>>>>>>>>>>>");
            // Set Proc Info into FB Driver for color alpha blending
            FaceBeautyProcInfo.FaceBeautyCtrlEnum = MTKFACEBEAUTY_CTRL_STEP5;
            FaceBeautyProcInfo.Step5SrcImgAddr = gAplhaMapColorBuffer;
            Retcode = pFaceBeautyInterface->FaceBeautyFeatureCtrl(MTKFACEBEAUTY_FEATURE_SET_PROC_INFO, &FaceBeautyProcInfo, 0);


            // Process and Get the Color Adjustment Image
            // FaceBeautyResultInfo.AdjustColorImgAddr
            // will be set
            Retcode = pFaceBeautyInterface->FaceBeautyMain();
            Retcode = pFaceBeautyInterface->FaceBeautyFeatureCtrl(MTKFACEBEAUTY_FEATURE_GET_RESULT, 0, &FaceBeautyResultInfo);
            //SaveResult(FaceBeautyResultInfo.Step5ResultAddr, "NRCL.bmp");

            ///////////////////////// Step6 ////////////////////////////
            MY_LOGD("process Step6 >>>>>>>>>>>>>");

            // Set Proc Info into FB Driver for warping
            //Use MDP to upsample YV12 buffer.
            gImageBlurBuffer = new unsigned char[gImageWidth*gImageHeight*2];
            FaceBeautyProcInfo.FaceBeautyCtrlEnum = MTKFACEBEAUTY_CTRL_STEP6;
            FaceBeautyProcInfo.Step6TempAddr = gImageBlurBuffer;
            Retcode = pFaceBeautyInterface->FaceBeautyFeatureCtrl(MTKFACEBEAUTY_FEATURE_SET_PROC_INFO, &FaceBeautyProcInfo, 0);


            // Process and Get the Final Warped Image
            // FaceBeautyResultInfo.WarpedImgAddr
            // will be set and this is the final result
            Retcode = pFaceBeautyInterface->FaceBeautyMain();
            Retcode = pFaceBeautyInterface->FaceBeautyFeatureCtrl(MTKFACEBEAUTY_FEATURE_GET_RESULT, 0, &FaceBeautyResultInfo);
#ifdef DEBUG_FB
            ///// SaveResult(FaceBeautyResultInfo.Step6ResultAddr, "/storage/emulated/0/FB/NRCLWP.bmp");
#endif
            MSize outSize = out -> getImgSize();
            if (out->getImgFormat() != eImgFmt_YV12 || !(gImageWidth == outSize.w && gImageHeight == outSize.h)) {
                if (mYV12WorkingBuffer == nullptr) {
                    MSize inSize = in->getImgSize();
                    mYV12WorkingBuffer =  BufferUtils::acquireWorkingBuffer(inSize,
                            eImgFmt_YV12);
                }
                memcpy((void *)mYV12WorkingBuffer->getBufVA(0), FaceBeautyResultInfo.Step6ResultAddr, gImageWidth*gImageHeight);
                memcpy((void *)mYV12WorkingBuffer->getBufVA(1),
                        FaceBeautyResultInfo.Step6ResultAddr+gImageWidth*gImageHeight, (gImageWidth*gImageHeight)/4);
                memcpy((void *)mYV12WorkingBuffer->getBufVA(2),
                        FaceBeautyResultInfo.Step6ResultAddr+(gImageWidth*gImageHeight*5)/4,
                        (gImageWidth*gImageHeight)/4);
#ifdef DEBUG_FB_FACE_RECT
                debugFaceInfo(pImetadata, mYV12WorkingBuffer);
#endif
                int rotationDegree = getJpegRotation(pRequest);
                MY_LOGD("process jpeg rotationDegree = %d", rotationDegree);
                BufferUtils::mdpResizeAndConvert(mYV12WorkingBuffer.get(), out, rotationDegree);

            } else {
                memcpy((void *)out->getBufVA(0), FaceBeautyResultInfo.Step6ResultAddr, gImageWidth*gImageHeight);
                memcpy((void *)out->getBufVA(1), FaceBeautyResultInfo.Step6ResultAddr + gImageWidth*gImageHeight,
                        (gImageWidth*gImageHeight)/4);
                memcpy((void *)out->getBufVA(2), FaceBeautyResultInfo.Step6ResultAddr+(gImageWidth*gImageHeight*5)/4,
                        (gImageWidth*gImageHeight)/4);
            }

            MY_LOGD("process Step6 <<<<<<<<<<<<<<<<<<<");

            delete[]gAplhaMapBuffer;
            delete[]gAplhaMapColorBuffer;

            Retcode = pFaceBeautyInterface->FaceBeautyExit();

            pFaceBeautyInterface->destroyInstance();



            delete[]gImageDsBuffer;
            delete[]gImageBlurBuffer;
            delete[]gImageNrDs420Buffer;
            delete[]gImageSWNRBlurBuffer;
        }

        if (pCallback != nullptr) {
            MY_LOGD("callback request");
            pCallback->onCompleted(pRequest, 0);
        }
        FUNCTION_OUT;
        return 0;
    };

    void releaseWorkingBuffer(IImageBuffer *buf) {
        if (buf != nullptr) {
            IImageBufferAllocator::getInstance()->free(buf);
        }
    }

    int getFaceOrientation(IMetadata const *pImetadata) {
            int physical_id = 0;
            tryGetMetadata<MINT32>(pImetadata, MTK_POSTALGO_FACE_BEAUTY_PHYSICAL_ID, physical_id);
            MY_LOGD("physical_id = %d", physical_id);
            int face_clock;
            if (physical_id == 0) {
                face_clock = 9;
            } else {
                face_clock = 3;
            }
        return face_clock;
    }

    int getJpegRotation(RequestPtr pRequest) {
        MINT32 jpegRotation = 0;
        IMetadata* pImetadata = pRequest->mIMetadataApp->acquire();
        if (pImetadata != nullptr && pImetadata->count() > 0) {
            tryGetMetadata<MINT32>(pImetadata, MTK_POSTALGO_JPEG_ORIENTATION, jpegRotation);
        }
        return jpegRotation;
    }

    void UpSampleDsAlphaColorMap(unsigned char* alpha_map_color_ds)
    {
        int i,j;
        int place;
        BITMAP bmp_in, bmp_out;

        bmp_in.width   = gImageDsWidth;
        bmp_in.height  = gImageDsHeight;
        bmp_in.offbits = 54;
        bmp_in.is_444  = 1;

        if ((bmp_in.r=(unsigned char *)calloc( gImageWidth*gImageHeight,sizeof(unsigned char) )) == NULL ||
                (bmp_in.g=(unsigned char *)calloc( gImageWidth*gImageHeight,sizeof(unsigned char) )) == NULL ||
                (bmp_in.b=(unsigned char *)calloc( gImageWidth*gImageHeight,sizeof(unsigned char) )) == NULL ) {
            printf("Error allocating memory \n");
            exit(1);
        }
        if ((bmp_out.r=(unsigned char *)calloc( gImageWidth*gImageHeight,sizeof(unsigned char) )) == NULL ||
                (bmp_out.g=(unsigned char *)calloc( gImageWidth*gImageHeight,sizeof(unsigned char) )) == NULL ||
                (bmp_out.b=(unsigned char *)calloc( gImageWidth*gImageHeight,sizeof(unsigned char) )) == NULL ) {
            printf("Error allocating memory \n");
            exit(1);
        }

        int DSEL = 7;
        int USEL = 1;
        int ECV  = 0;

        //CRZ: Up sample to alpha color map to original size

        bmp_in.width   = gImageDsWidth;
        bmp_in.height  = gImageDsHeight;
        bmp_in.offbits = 54;
        bmp_in.is_444  = 1;
        DSEL = 1;

        for(i=0;i<gImageDsHeight*gImageDsWidth;i++)
            bmp_in.r[i] = alpha_map_color_ds[i];


        bmp_out.width   = gImageWidth;
        bmp_out.height  = gImageHeight;
        bmp_out.offbits = 54;
        bmp_out.is_444  = 1;

        CRZ::crz (&bmp_in, &bmp_out, USEL, DSEL, ECV);

        gAplhaMapColorBuffer = new unsigned char[gImageWidth*gImageHeight];

        for(i=0;i<gImageHeight;i++)
        {
            for(j=0;j<gImageWidth;j++)
            {
                place = i*gImageWidth+j;
                gAplhaMapColorBuffer[place] = bmp_out.r[place];
            }
        }
        //delete [] alpha_map_color_ds;

        free(bmp_in.r);
        free(bmp_in.g);
        free(bmp_in.b);
        free(bmp_out.r);
        free(bmp_out.g);
        free(bmp_out.b);
    }

    virtual void abort(vector<RequestPtr>& pRequests)
    {
        FUNCTION_IN;

        FUNCTION_OUT;
    };

    virtual void uninit()
    {
        FUNCTION_IN;
        FUNCTION_OUT;
    };

    void UpSampleDsAlphaTextureMap(unsigned char* alpha_map_ds)
    {
        int i,j;
        int place;
        BITMAP bmp_in, bmp_out;

        bmp_in.width   = gImageDsWidth;
        bmp_in.height  = gImageDsHeight;
        bmp_in.offbits = 54;
        bmp_in.is_444  = 1;

        if ((bmp_in.r=(unsigned char *)calloc( gImageWidth*gImageHeight,sizeof(unsigned char) )) == NULL ||
                (bmp_in.g=(unsigned char *)calloc( gImageWidth*gImageHeight,sizeof(unsigned char) )) == NULL ||
                (bmp_in.b=(unsigned char *)calloc( gImageWidth*gImageHeight,sizeof(unsigned char) )) == NULL ) {
            printf("Error allocating memory \n");
            exit(1);
        }

        for(i=0;i<gImageDsWidth*gImageDsHeight;i++)
            bmp_in.r[i] = alpha_map_ds[i];


        bmp_out.width   = gImageWidth;
        bmp_out.height  = gImageHeight;
        bmp_out.offbits = 54;
        bmp_out.is_444  = 1;

        if ((bmp_out.r=(unsigned char *)calloc( gImageWidth*gImageHeight,sizeof(unsigned char) )) == NULL ||
                (bmp_out.g=(unsigned char *)calloc( gImageWidth*gImageHeight,sizeof(unsigned char) )) == NULL ||
                (bmp_out.b=(unsigned char *)calloc( gImageWidth*gImageHeight,sizeof(unsigned char) )) == NULL ) {
            printf("Error allocating memory \n");
            exit(1);
        }

        int DSEL = 7;
        int USEL = 1;
        int ECV  = 0;

        CRZ::crz (&bmp_in, &bmp_out, USEL, DSEL, ECV);

        gAplhaMapBuffer = new unsigned char[gImageWidth*gImageHeight];
        //unsigned char* skin_mask = new unsigned char[gImageWidth*gImageHeight];

        for(i=0;i<gImageHeight;i++)
        {
            for(j=0;j<gImageWidth;j++)
            {
                place = i*gImageWidth+j;
                gAplhaMapBuffer[place] = bmp_out.r[place];
            }
        }
        //delete [] alpha_map_ds;
        //delete [] skin_mask_ds;

        free(bmp_in.r);
        free(bmp_in.g);
        free(bmp_in.b);
        free(bmp_out.r);
        free(bmp_out.g);
        free(bmp_out.b);
    }

    void getFaceInfo(IImageBuffer *in) {
        MY_LOGD("gImageDsWidth = %d, gImageDsHeight = %d, format = %d", gImageDsWidth, gImageDsHeight, eImgFmt_RGBA8888);
        if (mImageDsRGBAWorkingBuffer == nullptr) {
            mImageDsRGBAWorkingBuffer = BufferUtils::acquireWorkingBuffer(MSize(gImageDsWidth, gImageDsHeight),
                    eImgFmt_RGBA8888);
        }
        BufferUtils::mdpResizeAndConvert(in, mImageDsRGBAWorkingBuffer.get());
        unsigned char* rgbabuffer = (unsigned char*)getBuffer(mImageDsRGBAWorkingBuffer.get(), 4);
        unsigned char* rgbbuffer = new unsigned char[gImageDsWidth*gImageDsHeight*3];
        //translate RGBA -> RGB
        RGBAtoRGB(rgbabuffer,rgbbuffer,gImageDsWidth*gImageDsHeight);
        getDsImageBuffer(rgbbuffer, gImageDsWidth, gImageDsHeight);
        delete[]rgbabuffer;
        delete[]rgbbuffer;
    };

    void getDsImageBuffer(unsigned char* RGBBuffer, int imageWidth, int height)
    {

        unsigned char* B_data_ds = new unsigned char[imageWidth*height];
        unsigned char* R_data_ds = new unsigned char[imageWidth*height];
        unsigned char* G_data_ds = new unsigned char[imageWidth*height];
        gImageDsBuffer = new unsigned char[imageWidth*height*2];

        ReadRGB(RGBBuffer, R_data_ds, G_data_ds, B_data_ds, imageWidth, height); //read the RGB value
#ifdef YUV_input
        RGB2YUV(R_data_ds, G_data_ds, B_data_ds, gImageDsWidth, gImageDsHeight); //R->Y, G->Cb, B->Cr
        YUV444to422(G_data_ds, B_data_ds, gImageDsWidth, gImageDsHeight, 0);
#endif

        memcpy(gImageDsBuffer, R_data_ds, imageWidth*height);
        memcpy(gImageDsBuffer+imageWidth*height, G_data_ds, imageWidth*height/2);
        memcpy(gImageDsBuffer+imageWidth*height*3/2, B_data_ds, imageWidth*height/2);


        delete[]B_data_ds;
        delete[]R_data_ds;
        delete[]G_data_ds;

    }

    void RGBAtoRGB(unsigned char *rgba, unsigned char *rgb, int numPixels)
    {
        int col;

        for (col = 0; col < numPixels; col++, rgba += 4, rgb += 3) {
            rgb[0] = rgba[1];
            rgb[1] = rgba[2];
            rgb[2] = rgba[3];
        }
    }

    void ReadRGB(unsigned char* RGBBuffer,unsigned char* R_data_ds,unsigned char* G_data_ds,unsigned char* B_data_ds,int input_img_width_ds, int input_img_height_ds)
    {
        int pad_width_ds;
        int i,j;
        unsigned char* BMP_data_ds;

        pad_width_ds = (input_img_width_ds*3 + 3)/4*4;        //padding handling

        BMP_data_ds = new unsigned char [pad_width_ds*input_img_height_ds];
        memcpy(BMP_data_ds, RGBBuffer, input_img_height_ds*input_img_width_ds);
        for(i=0;i<input_img_height_ds;i++)
        {
            for(j=0;j<input_img_width_ds*3;j++)
            {
                int place = i*pad_width_ds + j;
                if(j%3==0)
                    B_data_ds[(input_img_height_ds-i-1)*input_img_width_ds+j/3] = BMP_data_ds[place];
                else if(j%3==1)
                    G_data_ds[(input_img_height_ds-i-1)*input_img_width_ds+j/3] = BMP_data_ds[place];
                else
                    R_data_ds[(input_img_height_ds-i-1)*input_img_width_ds+j/3] = BMP_data_ds[place];
            }
        }

        delete []BMP_data_ds;
    }

    unsigned char * getBuffer(IImageBuffer * in, float multiple) {
        IImageBufferHeap* pHeap = in->getImageBufferHeap();
        MY_LOGD("getBuffer Format:%d", pHeap->getImgFormat());
        MY_LOGD("getBuffer getImgSize:width = %d, height = %d", pHeap->getImgSize().w, pHeap->getImgSize().h);
        MY_LOGD("getBuffer 2PlaneCount  = %d", (int)pHeap->getPlaneCount());
        int usage = eBUFFER_USAGE_SW_WRITE_OFTEN;
        int size = (int)(pHeap->getImgSize().w * pHeap->getImgSize().h * multiple);
        MY_LOGD("getBuffer size  = %d", size);
        unsigned char* imageBuffer = new unsigned char[size];
        pHeap->lockBuf("FB getBuffer", usage);
        int offset = 0;
        for (unsigned int i = 0; i < pHeap->getPlaneCount(); i++)
        {   char * buffer = (char *)pHeap->getBufVA(i);
            int lenght = pHeap->getBufSizeInBytes(i);
            MY_LOGD("getBufSizeInBytes:%d", (int)pHeap->getBufSizeInBytes(i));
            if (buffer)
            {
                //::memset((MUINT8*)pHeap->getBufVA(i), 0xAA, pHeap->getBufSizeInBytes(i));
                memcpy(imageBuffer+offset, buffer, lenght);
                offset = offset + lenght;
            }
        }
        pHeap->unlockBuf("FB getBuffer");
        return imageBuffer;
    };

    FBProviderImpl()
        :mOpenid(-1)
    {
        FUNCTION_IN;

        mEnable = 1;

        FUNCTION_OUT;
    };

    virtual ~FBProviderImpl()
    {
        FUNCTION_IN;

        FUNCTION_OUT;
    };

    template <class T>
        inline bool
        tryGetMetadata( IMetadata const *pMetadata, mtk_platform_metadata_tag_t tag, T& rVal)
        {
            if(pMetadata == nullptr) return MFALSE;

            IMetadata::IEntry entry = pMetadata->entryFor(tag);
            if(!entry.isEmpty())
            {
                rVal = entry.itemAt(0,Type2Type<T>());
                return true;
            }
            else
            {
                MY_LOGW(" no metadata %d ", tag);
            }
            return false;
        };

    void UpSampleFromDs420ToFull422(unsigned char* ds_yuv_420)
    {
        int i,j;
        int place;
        BITMAP bmp_in, bmp_out;

        //////Y channel////////////////
        bmp_in.width   = gImageNrDsWidth;
        bmp_in.height  = gImageNrDsHeight;
        bmp_in.offbits = 54;
        bmp_in.is_444  = 1;

        if ((bmp_in.r=(unsigned char *)calloc( gImageWidth*gImageHeight,sizeof(unsigned char) )) == NULL ||
                (bmp_in.g=(unsigned char *)calloc( gImageWidth*gImageHeight,sizeof(unsigned char) )) == NULL ||
                (bmp_in.b=(unsigned char *)calloc( gImageWidth*gImageHeight,sizeof(unsigned char) )) == NULL ) {
            printf("Error allocating memory \n");
            exit(1);
        }

        for(i=0;i<gImageNrDsWidth*gImageNrDsHeight;i++)
            bmp_in.r[i] = ds_yuv_420[i];


        bmp_out.width   = gImageWidth;
        bmp_out.height  = gImageHeight;
        bmp_out.offbits = 54;
        bmp_out.is_444  = 1;

        if ((bmp_out.r=(unsigned char *)calloc( gImageWidth*gImageHeight,sizeof(unsigned char) )) == NULL ||
                (bmp_out.g=(unsigned char *)calloc( gImageWidth*gImageHeight,sizeof(unsigned char) )) == NULL ||
                (bmp_out.b=(unsigned char *)calloc( gImageWidth*gImageHeight,sizeof(unsigned char) )) == NULL ) {
            printf("Error allocating memory \n");
            exit(1);
        }

        int DSEL = 7;
        int USEL = 1;
        int ECV  = 0;

        CRZ::crz (&bmp_in, &bmp_out, USEL, DSEL, ECV);

        gImageSWNRBlurBuffer = new unsigned char[gImageWidth*gImageHeight*2];
        unsigned char* temp_buffer = gImageSWNRBlurBuffer;
        //unsigned char* skin_mask = new unsigned char[gImageWidth*gImageHeight];

        for(i=0;i<gImageHeight;i++)
        {
            for(j=0;j<gImageWidth;j++)
            {
                place = i*gImageWidth+j;
                temp_buffer[place] = bmp_out.r[place];
            }
        }

        ///////U channel/////////////////////////////////
        bmp_in.width   = gImageNrDsWidth/2;
        bmp_in.height  = gImageNrDsHeight/2;
        bmp_in.offbits = 54;
        bmp_in.is_444  = 1;

        temp_buffer  += gImageWidth*gImageHeight;



        bmp_out.width   = gImageWidth/2;
        ds_yuv_420 += gImageNrDsWidth*gImageNrDsHeight;
#ifdef INPUT_YUV420
        bmp_out.height  = gImageHeight/2;
        //ds_yuv_420 += gImageNrDsWidth*gImageNrDsHeight*5/4;

#else
        bmp_out.height  = gImageHeight;

#endif
        bmp_out.offbits = 54;
        bmp_out.is_444  = 1;
        for(i=0;i<gImageNrDsWidth/2*gImageNrDsHeight/2;i++)
            bmp_in.r[i] = ds_yuv_420[i];


        CRZ::crz (&bmp_in, &bmp_out, USEL, DSEL, ECV);
#ifdef INPUT_YUV420
        for(i=0;i<gImageHeight/2;i++)
#else
            for(i=0;i<gImageHeight;i++)
#endif
            {
                for(j=0;j<gImageWidth/2;j++)
                {
                    place = i*gImageWidth/2+j;
                    temp_buffer[place] = bmp_out.r[place];
                }
            }


        //temp_buffer  += gImageWidth/2*gImageHeight;
#ifdef INPUT_YUV420
        ds_yuv_420 += (gImageNrDsWidth/2)*(gImageNrDsHeight/2);
#else
        ds_yuv_420 += gImageNrDsWidth/2*gImageNrDsHeight/2;
#endif
        for(i=0;i<gImageNrDsWidth/2*gImageNrDsHeight/2;i++)
            bmp_in.r[i] = ds_yuv_420[i];

        CRZ::crz (&bmp_in, &bmp_out, USEL, DSEL, ECV);

#ifdef INPUT_YUV420
        temp_buffer  += (gImageWidth/2)*(gImageHeight/2);
        for(i=0;i<gImageHeight/2;i++)
#else
            temp_buffer  += (gImageWidth/2)*gImageHeight;
        for(i=0;i<gImageHeight;i++)
#endif
        {
            for(j=0;j<gImageWidth/2;j++)
            {
                place = i*gImageWidth/2+j;
                temp_buffer[place] = bmp_out.r[place];
            }
        }

        free(bmp_in.r);
        free(bmp_in.g);
        free(bmp_in.b);
        free(bmp_out.r);
        free(bmp_out.g);
        free(bmp_out.b);
    }

    void YUVBufferSave(unsigned char* image, const char* filename1, const char* format, int w, int h, int size)
    {

        char path[128] = "/data/camera_post_algo_fb_dump";
        int result = mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
        if ((result == -1) && (errno != EEXIST)) {
            MY_LOGE("mkdir fail, error %d, return", errno);
            return;
        }

        char fileName[128];
        sprintf(fileName, "%s/%s_w%d_h%d_%s", path, filename1, w, h, format);
        MY_LOGD("fileName=%s", fileName);
        FILE *fp = fopen(fileName, "wb");
        if (NULL == fp) {
            MY_LOGE("fail to open file %s", fileName);
        } else {
            fwrite(image,1,size,fp);
            fclose(fp);
        }

    }

    void SaveResult(unsigned char* image, char filename1[])
    {
        int i,j;
        int place, place1;
        int pad_width = (gImageWidth*3 + 3)/4*4;        //padding handling
        unsigned char* BMP_data_o = new unsigned char[pad_width*gImageHeight];
        unsigned char* buffer = new unsigned char[gImageWidth*gImageHeight*3];
        unsigned char* R_data = buffer;
        unsigned char* G_data = R_data+gImageWidth*gImageHeight;
        unsigned char* B_data = G_data+gImageWidth*gImageHeight;
        memcpy(R_data, image, gImageWidth*gImageHeight);
#ifdef INPUT_YUV420 //YV12
        memcpy(B_data, image+gImageWidth*gImageHeight,   gImageWidth*gImageHeight/4);
        memcpy(G_data, image+gImageWidth*gImageHeight*5/4, gImageWidth*gImageHeight/4);
#else
        memcpy(G_data, image+gImageWidth*gImageHeight,   gImageWidth*gImageHeight/2);
        memcpy(B_data, image+gImageWidth*gImageHeight*3/2, gImageWidth*gImageHeight/2);
#endif

#ifdef INPUT_YUV420
        YUV420to444(G_data, B_data, gImageWidth, gImageHeight,0);
#else
        YUV422to444(G_data, B_data, gImageWidth, gImageHeight,0);
#endif
        YUV2RGB(R_data, G_data, B_data, gImageWidth, gImageHeight);


        for(i=0;i<gImageHeight;i++)
        {
            for(j=0;j<gImageWidth*3;j++)
            {
                place  = i*pad_width+j;
                place1 = (gImageHeight-i-1)*gImageWidth+j/3;
                if(j%3==0)
                    BMP_data_o[place] = B_data[place1];
                else if(j%3==1)
                    BMP_data_o[place] = G_data[place1];
                else
                    BMP_data_o[place] = R_data[place1];
            }
        }

        FILE* stream_write2;
        //strcpy(string1,filename1);
        if( (stream_write2 = fopen( filename1, "wb" )) == NULL ) {
            printf( "The file _wrinkle_removal.bmp was not opened\n" );
        } else {
            //fwrite(BMP_header,1,54,stream_write2);
            fwrite(BMP_data_o,pad_width,gImageHeight,stream_write2);
            fclose(stream_write2);
        }
        delete[]buffer;
        delete[]BMP_data_o;

    }
    void YUV2RGB(unsigned char* Y_data2, unsigned char* U_data2, unsigned char* V_data2, int width, int height)
    {
        int i,j;
        int place, Y, Cb, Cr;
        int    Y2R = 256;
        int    Y2G = 256;
        int    Y2B = 256;
        int    Cb2R = 0;
        int    Cb2G = -88;
        int    Cb2B = 455;
        int    Cr2R = 360;
        int    Cr2G = -184;
        int    Cr2B = 0;

        for(i=0;i<height;i++)
        {
            for(j=0;j<width;j++)
            {
                place = i*width + j;
                //            R = Y + (1.4075 * (V - 128));
                //            G = Y - (0.3455 * (U - 128) - (0.7169 * (V - 128));
                //            B = Y + (1.7790 * (U - 128);
                if(i==65 && j==211) {
                    //i = i;
                }
                Y  = (int)(Y2R*Y_data2[place] + Cb2R*(U_data2[place]-128) + Cr2R*(V_data2[place]-128) + 128)/256;
                Cb = (int)(Y2G*Y_data2[place] + Cb2G*(U_data2[place]-128) + Cr2G*(V_data2[place]-128) + 128)/256;
                Cr = (int)(Y2B*Y_data2[place] + Cb2B*(U_data2[place]-128) + Cr2B*(V_data2[place]-128) + 128)/256;

                if(Y>255) Y=255;
                else if(Y<0) Y=0;
                else {
                    //Y=Y;
                }
                if(Cb>255) Cb=255;
                else if(Cb<0) Cb=0;
                else {
                    // Cb=Cb;
                }
                if(Cr>255) Cr=255;
                else if(Cr<0) Cr=0;
                else {
                    //Cr=Cr;
                }
                Y_data2[place] = Y;
                U_data2[place] = Cb;
                V_data2[place] = Cr;
            }
        }
    };

    void YUV420to444(unsigned char* U_data, unsigned char* V_data, int width, int height, int copy)
    {
        int i,j;
        unsigned char* cb_data_444 = (unsigned char*)malloc(width*height*sizeof(unsigned char));
        unsigned char* cr_data_444 = (unsigned char*)malloc(width*height*sizeof(unsigned char));
        int place, place1;
        if (cb_data_444 != nullptr && cr_data_444 != nullptr) {
            //
            for(i=0;i<height;i+=2)
            {
                for(j=0;j<width;j++)
                {
                    if(j==width-1&&i==height-1)
                    {
                        //i = i;
                    }
                    place = i*width + j;
                    place1 = (i/2)*width/2 + j/2;
                    if(j%2==0)
                    {
                        cb_data_444[place] = U_data[place1];
                        cr_data_444[place] = V_data[place1];
                    }
                    else if(j+1==width || copy)
                    {
                        cb_data_444[place] = U_data[place1];
                        cr_data_444[place] = V_data[place1];
                    }
                    else
                    {
                        cb_data_444[place] = (U_data[place1]+U_data[place1+1]+1)/2;
                        cr_data_444[place] = (V_data[place1]+V_data[place1+1]+1)/2;
                    }
                }
            }
            //
            for(i=0;i<(height-1);i+=2)
            {
                for(j=0;j<width;j++)
                {
                    place = i*width + j;
                    place1 = (i+1)*width + j;

                    cb_data_444[place1] = (cb_data_444[place]+cb_data_444[place+width*2]+1)/2;
                    cr_data_444[place1] = (cr_data_444[place]+cr_data_444[place+width*2]+1)/2;
                }
            }
            //
            for(i=0;i<height;i++)
            {
                for(j=0;j<width;j++)
                {
                    place  = i*width + j;
                    U_data[place] = cb_data_444[place];
                    V_data[place] = cr_data_444[place];
                }
            }

        }
        if (cb_data_444 != nullptr) {
            free(cb_data_444);
        }

        if (cr_data_444 != nullptr) {
            free(cr_data_444);
        }
    }

    void debugFaceInfo(IMetadata* pImetadata, IImageBuffer *out) {
        MINT32 gFaceCount;
        MINT32 face_rect_length = 4;
        tryGetMetadata<MINT32>(pImetadata, MTK_POSTALGO_FACE_BEAUTY_FACE_DETECTION_SIZE, face_rect_length);
        gFaceCount = face_rect_length/4;
        MY_LOGD("process face = %d", gFaceCount);
        for(int i=0;i<gFaceCount;i++) {
            MRect faceRect;
            tryGetMetadata<MRect>(pImetadata, MTK_POSTALGO_FACE_BEAUTY_FACE_DETECTION, faceRect);
            char* pBufferVa = (char *) (out->getBufVA(0));
            MUINT32 stride = out->getBufStridesInBytes(0);
            MY_LOGD("Detected Face Rect[%d]: (xmin, ymin, xmax, ymax) => (%d, %d, %d, %d)",
                    i,
                    faceRect.p.x,
                    faceRect.p.y,
                    faceRect.s.w,
                    faceRect.s.h);
            // draw rectangles to output buffer
            memset(pBufferVa + stride * faceRect.p.y + faceRect.p.x,
                    255, faceRect.s.w + 1);

            memset(pBufferVa + stride * (faceRect.p.y + faceRect.s.h) + faceRect.p.x,
                    255, faceRect.s.w + 1);

            for (size_t j = faceRect.p.y + 1; j < (faceRect.p.y + faceRect.s.h) ; j++) {
                *(pBufferVa + stride * j + faceRect.p.x) = 255;
                *(pBufferVa + stride * j + faceRect.p.x + faceRect.s.w) = 255;
            }
        }
    }
    private:
    //string modeldir = "/storage/emulated/0/modeldir";
    int    mOpenid;
    int    mEnable;
    int gImageWidth;
    int gImageHeight;
    int gImageNrDsWidth;
    int gImageNrDsHeight;
    int gImageDsWidth;
    int gImageDsHeight;
    MTKFaceBeauty* pFaceBeautyInterface;
    MUINT8* WorkingBuffer;
    MUINT32 WorkingBufferSize;
    const static int gFDWidth = 320;
    int gFDHeight;
    //IImageBuffer* DSWorkingBuffer;
    sp<IImageBuffer> ImageNrDs420WorkingBuffer;
    sp<IImageBuffer> mFullSizeBlurWorkingBuffer;
    unsigned char* gImageSWNRBlurBuffer;
    sp<IImageBuffer> mImageDsRGBAWorkingBuffer;
    unsigned char* gImageDsBuffer;

    int gFaceCount = 1; //Number of face in current image
    //char string1[200];
    //unsigned char BMP_header[54];
    unsigned char* gAplhaMapBuffer;
    unsigned char* gAplhaMapColorBuffer;
    unsigned char* gImageBlurBuffer;
    sp<IImageBuffer> mYV12WorkingBuffer;
};

REGISTER_PLUGIN_PROVIDER(Yuv, FBProviderImpl);

