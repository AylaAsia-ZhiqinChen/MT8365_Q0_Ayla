/*******************************************************************************
 * Software Name : RCS IMS Stack
 *
 * Copyright (C) 2010-2016 Orange.
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
 ******************************************************************************/

package com.orangelabs.rcs.core.ims.protocol.rtp.format.data;

import com.orangelabs.rcs.core.ims.protocol.rtp.format.Format;

/**
 * Data format for generic RCS streaming session
 */
public class DataFormat extends Format {

    /**
     * Encoding name
     */
    public static final String ENCODING = "X-RCS/9000";

    /**
     * Payload type
     */
    public static final int PAYLOAD = 97;

    /**
     * Constructor
     */
    public DataFormat() {
        super(ENCODING, PAYLOAD);
    }

    /**
     * Constructor
     *
     * @param encoding Encoding
     */
    public DataFormat(String encoding) {
        super(encoding, PAYLOAD);
    }
}
