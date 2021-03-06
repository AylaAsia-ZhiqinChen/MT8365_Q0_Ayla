/*
 * Copyright (C) 2014 The Android Open Source Project
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

package com.android.inputmethod.keyboard.layout;

import static com.android.inputmethod.keyboard.layout.DevanagariLetterConstants.*;

import com.android.inputmethod.keyboard.KeyboardId;
import com.android.inputmethod.keyboard.layout.Hindi.HindiSymbols;
import com.android.inputmethod.keyboard.layout.customizer.NepaliCustomizer;
import com.android.inputmethod.keyboard.layout.expected.ExpectedKey;
import com.android.inputmethod.keyboard.layout.expected.ExpectedKeyboardBuilder;

import java.util.Locale;

/**
 * The nepali_traditional keyboard.
 */
public final class NepaliTraditional extends LayoutBase {
    private static final String LAYOUT_NAME = "nepali_traditional";

    public NepaliTraditional(final Locale locale) {
        super(new NepaliTraditionalCustomizer(locale), HindiSymbols.class, SymbolsShifted.class);
    }

    @Override
    public String getName() { return LAYOUT_NAME; }

    private static class NepaliTraditionalCustomizer extends NepaliCustomizer {
        NepaliTraditionalCustomizer(final Locale locale) { super(locale); }

        @Override
        public ExpectedKey[] getRightShiftKeys(final boolean isPhone) {
            return isPhone ? EMPTY_KEYS : EXCLAMATION_AND_QUESTION_MARKS;
        }
    }

    @Override
    ExpectedKey[][] getCommonAlphabetLayout(final boolean isPhone) { return ALPHABET_COMMON; }

    @Override
    ExpectedKey[][] getCommonAlphabetShiftLayout(final boolean isPhone, final int elementId) {
        if (elementId == KeyboardId.ELEMENT_ALPHABET_AUTOMATIC_SHIFTED) {
            return getCommonAlphabetLayout(isPhone);
        }
        return ALPHABET_SHIFTED_COMMON;
    }

    private static final ExpectedKey[][] ALPHABET_COMMON = new ExpectedKeyboardBuilder()
            .setKeysOfRow(1,
                    // U+091F: "???" DEVANAGARI LETTER TTA
                    // U+0967: "???" DEVANAGARI DIGIT ONE
                    key("\u091F", joinMoreKeys("\u0967", "1")),
                    // U+0927: "???" DEVANAGARI LETTER DHA
                    // U+0968: "???" DEVANAGARI DIGIT TWO
                    key("\u0927", joinMoreKeys("\u0968", "2")),
                    // U+092D: "???" DEVANAGARI LETTER BHA
                    // U+0969: "???" DEVANAGARI DIGIT THREE
                    key("\u092D", joinMoreKeys("\u0969", "3")),
                    // U+091A: "???" DEVANAGARI LETTER CA
                    // U+096A: "???" DEVANAGARI DIGIT FOUR
                    key("\u091A", joinMoreKeys("\u096A", "4")),
                    // U+0924: "???" DEVANAGARI LETTER TA
                    // U+096B: "???" DEVANAGARI DIGIT FIVE
                    key("\u0924", joinMoreKeys("\u096B", "5")),
                    // U+0925: "???" DEVANAGARI LETTER THA
                    // U+096C: "???" DEVANAGARI DIGIT SIX
                    key("\u0925", joinMoreKeys("\u096C", "6")),
                    // U+0917: "???" DEVANAGARI LETTER G
                    // U+096D: "???" DEVANAGARI DIGIT SEVEN
                    key("\u0917", joinMoreKeys("\u096D", "7")),
                    // U+0937: "???" DEVANAGARI LETTER SSA
                    // U+096E: "???" DEVANAGARI DIGIT EIGHT
                    key("\u0937", joinMoreKeys("\u096E", "8")),
                    // U+092F: "???" DEVANAGARI LETTER YA
                    // U+096F: "???" DEVANAGARI DIGIT NINE
                    key("\u092F", joinMoreKeys("\u096F", "9")),
                    // U+0909: "???" DEVANAGARI LETTER U
                    // U+0966: "???" DEVANAGARI DIGIT ZERO
                    key("\u0909", joinMoreKeys("\u0966", "0")),
                    // U+0907: "???" DEVANAGARI LETTER I
                    // U+0914: "???" DEVANAGARI LETTER AU
                    key("\u0907", moreKey("\u0914")))
            .setKeysOfRow(2,
                    // U+092C: "???" DEVANAGARI LETTER BA
                    // U+0915: "???" DEVANAGARI LETTER KA
                    // U+092E: "???" DEVANAGARI LETTER MA
                    "\u092C", "\u0915", "\u092E",
                    // U+093E: "???" DEVANAGARI VOWEL SIGN AA
                    key(VOWEL_SIGN_AA, "\u093E"),
                    // U+0928: "???" DEVANAGARI LETTER NA
                    // U+091C: "???" DEVANAGARI LETTER JA
                    // U+0935: "???" DEVANAGARI LETTER VA
                    // U+092A: "???" DEVANAGARI LETTER PA
                    "\u0928", "\u091C", "\u0935", "\u092A",
                    // U+093F: "???" DEVANAGARI VOWEL SIGN I
                    key(VOWEL_SIGN_I, "\u093F"),
                    // U+0938: "???" DEVANAGARI LETTER SA
                    "\u0938",
                    // U+0941: "???" DEVANAGARI VOWEL SIGN U
                    key(VOWEL_SIGN_U, "\u0941"))
            .setKeysOfRow(3,
                    // U+0936: "???" DEVANAGARI LETTER SHA
                    // U+0939: "???" DEVANAGARI LETTER HA
                    // U+0905: "???" DEVANAGARI LETTER A
                    // U+0916: "???" DEVANAGARI LETTER KHA
                    // U+0926: "???" DEVANAGARI LETTER DA
                    // U+0932: "???" DEVANAGARI LETTER LA
                    "\u0936", "\u0939", "\u0905", "\u0916", "\u0926", "\u0932",
                    // U+0947: "???" DEVANAGARI VOWEL SIGN E
                    // U+0903: "??????" DEVANAGARI SIGN VISARGA
                    // U+093D: "???" DEVANAGARI SIGN AVAGRAHA
                    key(VOWEL_SIGN_E, "\u0947", joinMoreKeys(
                            moreKey(SIGN_VISARGA, "\u0903"), "\u093D")),
                    // U+094D: "???" DEVANAGARI SIGN VIRAMA
                    key(SIGN_VIRAMA, "\u094D"),
                    // U+0930: "???" DEVANAGARI LETTER RA
                    // U+0930/U+0941: "??????" DEVANAGARI LETTER RA/DEVANAGARI VOWEL SIGN U
                    key("\u0930", moreKey("\u0930\u0941")))
            .build();

    private static final ExpectedKey[][] ALPHABET_SHIFTED_COMMON = new ExpectedKeyboardBuilder()
            .setKeysOfRow(1,
                    // U+0924/U+094D/U+0924:
                    // "?????????" DEVANAGARI LETTER TA/DEVANAGARI SIGN VIRAMA/DEVANAGARI LETTER TA
                    // U+091E: "???" DEVANAGARI LETTER NYA
                    // U+091C/U+094D/U+091E: "?????????" DEVANAGARI LETTER JA/DEVANAGARI SIGN
                    // VIRAMA/DEVANAGARI LETTER NYA
                    // U+0965: "???" DEVANAGARI DOUBLE DANDA
                    key("\u0924\u094D\u0924",
                            joinMoreKeys("\u091E", "\u091C\u094D\u091E", "\u0965")),
                    // U+0921/U+094D/U+0922:
                    // "?????????" DEVANAGARI LETTER DDA/DEVANAGARI SIGN VIRAMA/DEVANAGARI LETTER DDHA
                    // U+0908: "???" DEVANAGARI LETTER II
                    key("\u0921\u094D\u0922", moreKey("\u0908")),
                    // U+0910: "???" DEVANAGARI LETTER AI
                    // U+0918: "???" DEVANAGARI LETTER GHA
                    key("\u0910", moreKey("\u0918")),
                    // U+0926/U+094D/U+0935:
                    // "?????????" DEVANAGARI LETTER DA/DEVANAGARI SIGN VIRAMA/DEVANAGARI LETTER VA
                    // U+0926/U+094D/U+0927:
                    // "?????????" DEVANAGARI LETTER DA/DEVANAGARI SIGN VIRAMA/DEVANAGARI LETTER DHA
                    key("\u0926\u094D\u0935", moreKey("\u0926\u094D\u0927")),
                    // U+091F/U+094D/U+091F:
                    // "?????????" DEVANAGARI LETTER TTA/DEVANAGARI SIGN VIRAMA/DEVANAGARI LETTER TTA
                    // U+091B: "???" DEVANAGARI LETTER CHA
                    key("\u091F\u094D\u091F", moreKey("\u091B")),
                    // U+0920/U+094D/U+0920:
                    // "?????????" DEVANAGARI LETTER TTHA/DEVANAGARI SIGN VIRAMA/DEVANAGARI LETTER TTHA
                    // U+091F: "???" DEVANAGARI LETTER TTA
                    key("\u0920\u094D\u0920", moreKey("\u091F")),
                    // U+090A: "???" DEVANAGARI LETTER UU
                    // U+0920: "???" DEVANAGARI LETTER TTHA
                    key("\u090A", moreKey("\u0920")),
                    // U+0915/U+094D/U+0937:
                    // "?????????" DEVANAGARI LETTER KA/DEVANAGARI SIGN VIRAMA/DEVANAGARI LETTER SSA
                    // U+0921: "???" DEVANAGARI LETTER DDA
                    key("\u0915\u094D\u0937", moreKey("\u0921")),
                    // U+0907: "???" DEVANAGARI LETTER I
                    // U+0922: "???" DEVANAGARI LETTER DDHA
                    key("\u0907", moreKey("\u0922")),
                    // U+090F: "???" DEVANAGARI LETTER E
                    // U+0923: "???" DEVANAGARI LETTER NNA
                    key("\u090F", moreKey("\u0923")),
                    // U+0943: "???" DEVANAGARI VOWEL SIGN VOCALIC R
                    // U+0913: "???" DEVANAGARI LETTER O
                    key(VOWEL_SIGN_VOCALIC_R, "\u0943", moreKey("\u0913")))
            .setKeysOfRow(2,
                    // U+0906: "???" DEVANAGARI LETTER AA
                    // U+0919/U+094D: "??????" DEVANAGARI LETTER NGA/DEVANAGARI SIGN VIRAMA
                    // U+0921/U+094D/U+0921:
                    //     "?????????" DEVANAGARI LETTER DDA/DEVANAGARI SIGN VIRAMA/DEVANAGARI LETTER DDA
                    "\u0906", "\u0919\u094D", "\u0921\u094D\u0921",
                    // U+0901: "???" DEVANAGARI SIGN CANDRABINDU
                    key(SIGN_CANDRABINDU, "\u0901"),
                    // U+0926/U+094D/U+0926:
                    //     "?????????" DEVANAGARI LETTER DA/DEVANAGARI SIGN VIRAMA/DEVANAGARI LETTER DA
                    // U+091D: "???" DEVANAGARI LETTER JHA
                    "\u0926\u094D\u0926", "\u091D",
                    // U+094B: "???" DEVANAGARI VOWEL SIGN O
                    key(VOWEL_SIGN_O, "\u094B"),
                    // U+092B: "???" DEVANAGARI LETTER PHA
                    "\u092B",
                    // U+0940: "???" DEVANAGARI VOWEL SIGN II
                    key(VOWEL_SIGN_II, "\u0940"),
                    // U+091F/U+094D/U+0920:
                    //     "?????????" DEVANAGARI LETTER TTA/DEVANAGARI SIGN VIRAMA/DEVANAGARI LETTER TTHA
                    "\u091F\u094D\u0920",
                    // U+0942: "???" DEVANAGARI VOWEL SIGN UU
                    key(VOWEL_SIGN_UU, "\u0942"))
            .setKeysOfRow(3,
                    // U+0915/U+094D: "??????" DEVANAGARI LETTER KA/DEVANAGARI SIGN VIRAMA
                    // U+0939/U+094D/U+092E:
                    //     "?????????" DEVANAGARI LETTER HA/DEVANAGARI SIGN VIRAMA/DEVANAGARI LETTER MA
                    // U+090B: "???" DEVANAGARI LETTER VOCALIC R
                    // U+0950: "???" DEVANAGARI OM
                    "\u0915\u094D", "\u0939\u094D\u092E", "\u090B", "\u0950",
                    // U+094C: "???" DEVANAGARI VOWEL SIGN AU
                    key(VOWEL_SIGN_AU, "\u094C"),
                    // U+0926/U+094D/U+092F:
                    //     "?????????" DEVANAGARI LETTER DA/DEVANAGARI SIGN VIRAMA/DEVANAGARI LETTER YA
                    "\u0926\u094D\u092F",
                    // U+0902: "???" DEVANAGARI SIGN ANUSVARA
                    key(SIGN_ANUSVARA, "\u0902"),
                    // U+0919: "???" DEVANAGARI LETTER NGA
                    "\u0919",
                    // U+0948: "???" DEVANAGARI VOWEL SIGN AI
                    // U+0936/U+094D/U+0930:
                    //     "?????????" DEVANAGARI LETTER SHA/DEVANAGARI SIGN VIRAMA/DEVANAGARI LETTER RA
                    key(VOWEL_SIGN_AI, "\u0948", moreKey("\u0936\u094D\u0930")))
            .build();
}
