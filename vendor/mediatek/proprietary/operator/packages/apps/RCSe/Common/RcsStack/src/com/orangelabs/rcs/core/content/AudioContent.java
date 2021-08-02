/*******************************************************************************
 * Software Name : RCS IMS Stack
 *
 * Copyright (C) 2010-2016 Orange.
 * Copyright (C) 2014 Sony Mobile Communications AB.
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
 *
 * NOTE: This file has been modified by Sony Mobile Communications AB.
 * Modifications are licensed under the License.
 ******************************************************************************/

package com.orangelabs.rcs.core.content;

import android.net.Uri;

/**
 * Audio content
 * 
 * @author opob7414
 */
public class AudioContent extends MmContent {

    /**
     * Constructor
     * 
     * @aparam encoding Encoding
     */
    public AudioContent(String encoding) {
        super(encoding);
    }

    /**
     * Constructor
     * 
     * @param audioFile URI
     * @param encoding Encoding
     * @param size Size
     * @param fileName Filename
     */
    public AudioContent(Uri audioFile, String encoding, long size, String fileName) {
        super(audioFile, encoding, size, fileName);
    }
    
    /**
     * Constructor
     * 
     * @param url URL
     * @aparam encoding Encoding
     * @param size Content size
     */
    public AudioContent(String url, String encoding, long size) {
        super(url, encoding, size);
    }
}
