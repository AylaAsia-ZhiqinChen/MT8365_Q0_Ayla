/****************************************************************************
* Copyright (C) 2014-2015 Intel Corporation.   All Rights Reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice (including the next
* paragraph) shall be included in all copies or substantial portions of the
* Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
* 
* @file builder.h
* 
* @brief Includes all the builder related functionality
* 
* Notes:
* 
******************************************************************************/

#include "jit_pch.hpp"
#include "builder.h"

namespace SwrJit
{
    using namespace llvm;

    //////////////////////////////////////////////////////////////////////////
    /// @brief Contructor for Builder.
    /// @param pJitMgr - JitManager which contains modules, function passes, etc.
    Builder::Builder(JitManager *pJitMgr)
        : mpJitMgr(pJitMgr)
    {
        SWR_ASSERT(pJitMgr->mVWidth == 8);

        mVWidth = pJitMgr->mVWidth;
        mVWidth16 = pJitMgr->mVWidth * 2;

        mpIRBuilder = &pJitMgr->mBuilder;

        // Built in types: scalar

        mVoidTy     = Type::getVoidTy(pJitMgr->mContext);
        mFP16Ty     = Type::getHalfTy(pJitMgr->mContext);
        mFP32Ty     = Type::getFloatTy(pJitMgr->mContext);
        mFP32PtrTy  = PointerType::get(mFP32Ty, 0);
        mDoubleTy   = Type::getDoubleTy(pJitMgr->mContext);
        mInt1Ty     = Type::getInt1Ty(pJitMgr->mContext);
        mInt8Ty     = Type::getInt8Ty(pJitMgr->mContext);
        mInt16Ty    = Type::getInt16Ty(pJitMgr->mContext);
        mInt32Ty    = Type::getInt32Ty(pJitMgr->mContext);
        mInt8PtrTy  = PointerType::get(mInt8Ty, 0);
        mInt16PtrTy = PointerType::get(mInt16Ty, 0);
        mInt32PtrTy = PointerType::get(mInt32Ty, 0);
        mInt64Ty    = Type::getInt64Ty(pJitMgr->mContext);

        // Built in types: simd8

        mSimdInt1Ty     = VectorType::get(mInt1Ty,  mVWidth);
        mSimdInt16Ty    = VectorType::get(mInt16Ty, mVWidth);
        mSimdInt32Ty    = VectorType::get(mInt32Ty, mVWidth);
        mSimdInt64Ty    = VectorType::get(mInt64Ty, mVWidth);
        mSimdFP16Ty     = VectorType::get(mFP16Ty,  mVWidth);
        mSimdFP32Ty     = VectorType::get(mFP32Ty,  mVWidth);
        mSimdVectorTy   = ArrayType::get(mSimdFP32Ty, 4);
        mSimdVectorTRTy = ArrayType::get(mSimdFP32Ty, 5);

        // Built in types: simd16

        mSimd16Int1Ty       = VectorType::get(mInt1Ty,  mVWidth16);
        mSimd16Int16Ty      = VectorType::get(mInt16Ty, mVWidth16);
        mSimd16Int32Ty      = VectorType::get(mInt32Ty, mVWidth16);
        mSimd16Int64Ty      = VectorType::get(mInt64Ty, mVWidth16);
        mSimd16FP16Ty       = VectorType::get(mFP16Ty,  mVWidth16);
        mSimd16FP32Ty       = VectorType::get(mFP32Ty,  mVWidth16);
        mSimd16VectorTy     = ArrayType::get(mSimd16FP32Ty, 4);
        mSimd16VectorTRTy   = ArrayType::get(mSimd16FP32Ty, 5);

        if (sizeof(uint32_t*) == 4)
        {
            mIntPtrTy = mInt32Ty;
            mSimdIntPtrTy = mSimdInt32Ty;
            mSimd16IntPtrTy = mSimd16Int32Ty;
        }
        else
        {
            SWR_ASSERT(sizeof(uint32_t*) == 8);

            mIntPtrTy = mInt64Ty;
            mSimdIntPtrTy = mSimdInt64Ty;
            mSimd16IntPtrTy = mSimd16Int64Ty;
        }
    }
}
