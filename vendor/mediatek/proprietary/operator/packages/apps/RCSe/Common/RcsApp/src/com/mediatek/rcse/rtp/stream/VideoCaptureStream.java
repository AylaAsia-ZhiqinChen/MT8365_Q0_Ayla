/*******************************************************************************
 * Software Name : RCS IMS Stack
 *
 * Copyright (C) 2010 France Telecom S.A.
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

package com.mediatek.rcse.rtp.stream;

import com.mediatek.rcse.rtp.format.Format;
import com.mediatek.rcse.rtp.media.MediaInput;
import com.mediatek.rcse.rtp.media.VideoSample;
import com.mediatek.rcse.rtp.util.Buffer;

/**
 * Video capture stream
 *
 * @author hlxn7157
 */
public class VideoCaptureStream extends MediaCaptureStream {
    /**
     * Constructor
     * 
     * @param format Input format
     * @param player Media player
     */
    public VideoCaptureStream(Format format, MediaInput player) {
        super(format, player);
    }

    /**
     * Read from the stream
     * 
     * @return Buffer
     * @throws Exception
     */
    public Buffer read() throws Exception {
        // Read a new sample from the media player
        VideoSample sample = (VideoSample) getPlayer().readSample();
        if (sample == null) {
            return null;
        }

        // Create a buffer
        buffer.setData(sample.getData());
        buffer.setLength(sample.getLength());
        buffer.setFormat(getFormat());
        buffer.setSequenceNumber(seqNo++);
        if (sample.isMarker()) {
            buffer.setFlags(Buffer.FLAG_RTP_MARKER);
        }
        buffer.setTimeStamp(sample.getTimeStamp());
        buffer.setVideoOrientation(sample.getVideoOrientation());
        return buffer;
    }
}
