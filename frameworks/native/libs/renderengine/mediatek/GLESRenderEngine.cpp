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
  * MediaTek Inc. (C) 2018. All rights reserved.
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

#undef LOG_TAG
#define LOG_TAG "RenderEngine"
#define ATRACE_TAG ATRACE_TAG_GRAPHICS

#include "../gl/GLESRenderEngine.h"

#include <math.h>
#include <fstream>
#include <sstream>
#include <unordered_set>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android-base/stringprintf.h>
#include <cutils/compiler.h>
#include <cutils/properties.h>
#include <renderengine/Mesh.h>
#include <renderengine/Texture.h>
#include <renderengine/private/Description.h>
#include <sync/sync.h>
#include <ui/ColorSpace.h>
#include <ui/DebugUtils.h>
#include <ui/GraphicBuffer.h>
#include <ui/Rect.h>
#include <ui/Region.h>
#include <utils/KeyedVector.h>
#include <utils/Trace.h>

namespace android {
namespace renderengine {
namespace gl {

#ifdef MTK_SF_DEBUG_SUPPORT
/*void GLESRenderEngine::enableDebugLine(bool enable) {
    mEnableDebugLine = enable;
}*/

void GLESRenderEngine::setDebugLineConfig(int32_t width, int32_t height, std::uint32_t flipCount,
                                          uint32_t color, uint32_t steps) {
    mDebugLineWidth = width;
    mDebugLineHeight = height;
    mDebugLineFlipCount = flipCount;
    mDebugLineColor = color;
    mDebugLineSteps = steps;
}

/*bool GLESRenderEngine::isEnabledDebugLine() {
    return mEnableDebugLine;
}*/

void GLESRenderEngine::drawDebugLine() {
    if (mEnableDebugLine) {
        float w = mDebugLineWidth;
        float h = mDebugLineHeight;

        // debug line size and pos
        float cnt = mDebugLineFlipCount % mDebugLineSteps;
        float size = h / mDebugLineSteps;

        // flip y for GL coord
        h = (h - size) - (cnt * size);

        // get color for line
        float r = (uint8_t) mDebugLineColor        / 255.0;
        float g = (uint8_t)(mDebugLineColor >> 8 ) / 255.0;
        float b = (uint8_t)(mDebugLineColor >> 16) / 255.0;
        float a = (uint8_t)(mDebugLineColor >> 24) / 255.0;

        glEnable(GL_SCISSOR_TEST);
        glScissor(0, h, w, size);
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);
    }
}
#endif

} // namespace gl
} // namespace renderengine
} // namespace android
