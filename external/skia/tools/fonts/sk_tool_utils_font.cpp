/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "sk_tool_utils.h"

#include "Resources.h"
#include "SkCommonFlags.h"
#include "SkFontMgr.h"
#include "SkFontStyle.h"
#include "SkMutex.h"
#include "SkOSFile.h"
#include "SkTestFontMgr.h"
#include "SkTypeface.h"
#include "SkUTF.h"

namespace sk_tool_utils {

sk_sp<SkTypeface> emoji_typeface() {
    const char* filename;
#if defined(SK_BUILD_FOR_WIN)
    filename = "fonts/colr.ttf";
#elif defined(SK_BUILD_FOR_MAC) || defined(SK_BUILD_FOR_IOS)
    filename = "fonts/sbix.ttf";
#else
    filename = "fonts/cbdt.ttf";
#endif
    sk_sp<SkTypeface> typeface = MakeResourceAsTypeface(filename);
    if (typeface) {
        return typeface;
    }
    return SkTypeface::MakeFromName("Emoji", SkFontStyle());
}

const char* emoji_sample_text() {
    return "\xF0\x9F\x98\x80" " " "\xE2\x99\xA2"; // 😀 ♢
}
static sk_sp<SkTypeface> create_font(const char* name, SkFontStyle style) {
    static sk_sp<SkFontMgr> portableFontMgr = MakePortableFontMgr();
    return portableFontMgr->legacyMakeTypeface(name, style);
}

sk_sp<SkTypeface> create_portable_typeface(const char* name, SkFontStyle style) {
    return create_font(name, style);
}
}
