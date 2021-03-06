/*
 * Copyright (C) 2010 The Android Open Source Project
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

package com.android.nfc;

import android.nfc.ErrorCodes;

/** Generic exception thrown in case something unexpected happened during an LLCP communication. */
public class LlcpException extends Exception {
    /**
     * Constructs a new LlcpException with the current stack trace and the specified detail message.
     *
     * @param s the detail message for this exception.
     */
    public LlcpException(String s) {
        super(s);
    }

    public LlcpException(int error) {
        super(ErrorCodes.asString(error));
    }
}
