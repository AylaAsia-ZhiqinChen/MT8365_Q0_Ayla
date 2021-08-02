/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#ifndef SkBitmapProcShader_DEFINED
#define SkBitmapProcShader_DEFINED

#include "SkImagePriv.h"
#include "SkShaderBase.h"
#include "SkBitmapProcState.h"

class SkBitmapProvider;

class SkBitmapProcLegacyShader : public SkShaderBase {
private:
    friend class SkImageShader;

    static Context* MakeContext(const SkShaderBase&, TileMode tmx, TileMode tmy,
                                const SkBitmapProvider&, const ContextRec&, SkArenaAlloc* alloc);

    typedef SkShaderBase INHERITED;
};

/*mtk enhancement*/
class BitmapProcInfoContext : public SkShaderBase::Context {
public:
    // The info has been allocated elsewhere, but we are responsible for calling its destructor.
    BitmapProcInfoContext(const SkShaderBase& shader, const SkShaderBase::ContextRec& rec,
                            SkBitmapProcInfo* info);

    uint32_t getFlags() const override { return fFlags; }

private:
    SkBitmapProcInfo*   fInfo;
    uint32_t            fFlags;

    typedef SkShaderBase::Context INHERITED;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

class BitmapProcShaderContext : public BitmapProcInfoContext {
public:
    BitmapProcShaderContext(const SkShaderBase& shader, const SkShaderBase::ContextRec& rec,
                            SkBitmapProcState* state)
        : INHERITED(shader, rec, state)
        , fState(state)
    {}
    /*mtk enhancement for extension*/
    friend struct SkBitmapProcShaderExt;
    void shadeSpan(int x, int y, SkPMColor dstC[], int count) override;

private:
    SkBitmapProcState*  fState;

    typedef BitmapProcInfoContext INHERITED;
};
#endif
