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

#include "../include/IIBuffer.h"

#include "../include/DebugControl.h"
#define PIPE_TRACE TRACE_IIBUFFER
#define PIPE_CLASS_TAG "IIBuffer"
#include "../include/PipeLog.h"

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {

IIBuffer::IIBuffer()
{
  TRACE_FUNC_ENTER();
  TRACE_FUNC_EXIT();
}

IIBuffer::~IIBuffer()
{
  TRACE_FUNC_ENTER();
  TRACE_FUNC_EXIT();
}

IImageBuffer* IIBuffer::getImageBufferPtr() const
{
  TRACE_FUNC_ENTER();
  IImageBuffer *ptr = NULL;
  ptr = this->getImageBuffer().get();
  TRACE_FUNC_EXIT();
  return ptr;
}

NB_SPTR IIBuffer::getGraphicBufferAddr()
{
  TRACE_FUNC_ENTER();
  NB_SPTR addr = NULL;
  android::sp<IImageBuffer> buffer;
  buffer = this->getImageBuffer();
  if( buffer != NULL && buffer->getImageBufferHeap() )
  {
    addr = (NB_SPTR)buffer->getImageBufferHeap()->getHWBuffer();
  }
  TRACE_FUNC_EXIT();
  return addr;
}

IIBuffer_IImageBuffer::IIBuffer_IImageBuffer(IImageBuffer *buffer)
  : mBuffer(buffer)
{
}

IIBuffer_IImageBuffer::~IIBuffer_IImageBuffer()
{
}

sp<IImageBuffer> IIBuffer_IImageBuffer::getImageBuffer() const
{
  return mBuffer;
}

sp<IImageBuffer> IIBuffer_IImageBuffer::operator->() const
{
  return mBuffer;
}

}; // namespace NSFeaturePipe
}; // namespace NSCamFeature
}; // namespace NSCam
