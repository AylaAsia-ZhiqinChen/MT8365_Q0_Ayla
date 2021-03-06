/*
 * Copyright 2015 The Android Open Source Project
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

package android.media.cts;

import static org.junit.Assert.assertNotEquals;

import android.content.res.AssetFileDescriptor;
import android.content.res.Resources;
import android.icu.util.ULocale;
import android.media.AudioFormat;
import android.media.AudioPresentation;
import android.media.MediaDataSource;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.media.cts.R;
import android.os.PersistableBundle;
import android.platform.test.annotations.AppModeFull;
import android.test.AndroidTestCase;
import android.util.Log;
import android.webkit.cts.CtsTestServer;

import androidx.test.filters.SmallTest;

import java.io.BufferedReader;
import java.io.Closeable;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.StreamTokenizer;
import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.SortedMap;
import java.util.TreeMap;

public class MediaExtractorTest extends AndroidTestCase {
    private static final String TAG = "MediaExtractorTest";

    protected Resources mResources;
    protected MediaExtractor mExtractor;

    @Override
    protected void setUp() throws Exception {
        super.setUp();
        mResources = getContext().getResources();
        mExtractor = new MediaExtractor();
    }

    @Override
    protected void tearDown() throws Exception {
        super.tearDown();
        mExtractor.release();
    }

    protected TestMediaDataSource getDataSourceFor(int resid) throws Exception {
        AssetFileDescriptor afd = mResources.openRawResourceFd(resid);
        return TestMediaDataSource.fromAssetFd(afd);
    }

    protected TestMediaDataSource setDataSource(int resid) throws Exception {
        TestMediaDataSource ds = getDataSourceFor(resid);
        mExtractor.setDataSource(ds);
        return ds;
    }

    public void testNullMediaDataSourceIsRejected() throws Exception {
        try {
            mExtractor.setDataSource((MediaDataSource)null);
            fail("Expected IllegalArgumentException.");
        } catch (IllegalArgumentException ex) {
            // Expected, test passed.
        }
    }

    public void testMediaDataSourceIsClosedOnRelease() throws Exception {
        TestMediaDataSource dataSource = setDataSource(R.raw.testvideo);
        mExtractor.release();
        assertTrue(dataSource.isClosed());
    }

    public void testExtractorFailsIfMediaDataSourceThrows() throws Exception {
        TestMediaDataSource dataSource = getDataSourceFor(R.raw.testvideo);
        dataSource.throwFromReadAt();
        try {
            mExtractor.setDataSource(dataSource);
            fail("Expected IOException.");
        } catch (IOException e) {
            // Expected.
        }
    }

    public void testExtractorFailsIfMediaDataSourceReturnsAnError() throws Exception {
        TestMediaDataSource dataSource = getDataSourceFor(R.raw.testvideo);
        dataSource.returnFromReadAt(-2);
        try {
            mExtractor.setDataSource(dataSource);
            fail("Expected IOException.");
        } catch (IOException e) {
            // Expected.
        }
    }

    // Smoke test MediaExtractor reading from a DataSource.
    public void testExtractFromAMediaDataSource() throws Exception {
        TestMediaDataSource dataSource = setDataSource(R.raw.testvideo);
        checkExtractorSamplesAndMetrics();
    }

    // Smoke test MediaExtractor reading from an AssetFileDescriptor.
    public void testExtractFromAssetFileDescriptor() throws Exception {
        AssetFileDescriptor afd = mResources.openRawResourceFd(R.raw.testvideo);
        mExtractor.setDataSource(afd);
        checkExtractorSamplesAndMetrics();
        afd.close();
    }

    private void checkExtractorSamplesAndMetrics() {
        // 1MB is enough for any sample.
        final ByteBuffer buf = ByteBuffer.allocate(1024*1024);
        final int trackCount = mExtractor.getTrackCount();

        for (int i = 0; i < trackCount; i++) {
            mExtractor.selectTrack(i);
        }

        for (int i = 0; i < trackCount; i++) {
            assertTrue(mExtractor.readSampleData(buf, 0) > 0);
            assertTrue(mExtractor.advance());
        }

        // verify some getMetrics() behaviors while we're here.
        PersistableBundle metrics = mExtractor.getMetrics();
        if (metrics == null) {
            fail("getMetrics() returns no data");
        } else {
            // ensure existence of some known fields
            int tracks = metrics.getInt(MediaExtractor.MetricsConstants.TRACKS, -1);
            if (tracks != trackCount) {
                fail("getMetrics() trackCount expect " + trackCount + " got " + tracks);
            }
        }
    }

    static boolean audioPresentationSetMatchesReference(
            Map<Integer, AudioPresentation> reference,
            List<AudioPresentation> actual) {
        if (reference.size() != actual.size()) {
            Log.w(TAG, "AudioPresentations set size is invalid, expected: " +
                    reference.size() + ", actual: " + actual.size());
            return false;
        }
        for (AudioPresentation ap : actual) {
            AudioPresentation refAp = reference.get(ap.getPresentationId());
            if (refAp == null) {
                Log.w(TAG, "AudioPresentation not found in the reference set, presentation id=" +
                        ap.getPresentationId());
                return false;
            }
            if (!refAp.equals(ap)) {
                Log.w(TAG, "AudioPresentations are different, reference: " +
                        refAp + ", actual: " + ap);
                return false;
            }
        }
        return true;
    }

    public void testGetAudioPresentations() throws Exception {
        final int resid = R.raw.MultiLangPerso_1PID_PC0_Select_AC4_H265_DVB_50fps;
        TestMediaDataSource dataSource = setDataSource(resid);
        int ac4TrackIndex = -1;
        for (int i = 0; i < mExtractor.getTrackCount(); i++) {
            MediaFormat format = mExtractor.getTrackFormat(i);
            String mime = format.getString(MediaFormat.KEY_MIME);
            if (MediaFormat.MIMETYPE_AUDIO_AC4.equals(mime)) {
                ac4TrackIndex = i;
                break;
            }
        }
        assertNotEquals(
                "AC4 track was not found in MultiLangPerso_1PID_PC0_Select_AC4_H265_DVB_50fps",
                -1, ac4TrackIndex);

        // The test file has two sets of audio presentations. The presentation set
        // changes for every 100 audio presentation descriptors between two presentations.
        // Instead of attempting to count the presentation descriptors, the test assumes
        // a particular order of the presentations and advances to the next reference set
        // once getAudioPresentations returns a set that doesn't match the current reference set.
        // Thus the test can match the set 0 several times, then it encounters set 1,
        // advances the reference set index, matches set 1 until it encounters set 2 etc.
        // At the end it verifies that all the reference sets were met.
        List<Map<Integer, AudioPresentation>> refPresentations = Arrays.asList(
                new HashMap<Integer, AudioPresentation>() {{  // First set.
                    put(10, new AudioPresentation.Builder(10)
                            .setLocale(ULocale.ENGLISH)
                            .setMasteringIndication(AudioPresentation.MASTERED_FOR_SURROUND)
                            .setHasDialogueEnhancement(true)
                            .build());
                    put(11, new AudioPresentation.Builder(11)
                            .setLocale(ULocale.ENGLISH)
                            .setMasteringIndication(AudioPresentation.MASTERED_FOR_SURROUND)
                            .setHasAudioDescription(true)
                            .setHasDialogueEnhancement(true)
                            .build());
                    put(12, new AudioPresentation.Builder(12)
                            .setLocale(ULocale.FRENCH)
                            .setMasteringIndication(AudioPresentation.MASTERED_FOR_SURROUND)
                            .setHasDialogueEnhancement(true)
                            .build());
                }},
                new HashMap<Integer, AudioPresentation>() {{  // Second set.
                    put(10, new AudioPresentation.Builder(10)
                            .setLocale(ULocale.GERMAN)
                            .setMasteringIndication(AudioPresentation.MASTERED_FOR_SURROUND)
                            .setHasAudioDescription(true)
                            .setHasDialogueEnhancement(true)
                            .build());
                    put(11, new AudioPresentation.Builder(11)
                            .setLocale(new ULocale("es"))
                            .setMasteringIndication(AudioPresentation.MASTERED_FOR_SURROUND)
                            .setHasSpokenSubtitles(true)
                            .setHasDialogueEnhancement(true)
                            .build());
                    put(12, new AudioPresentation.Builder(12)
                            .setLocale(ULocale.ENGLISH)
                            .setMasteringIndication(AudioPresentation.MASTERED_FOR_SURROUND)
                            .setHasDialogueEnhancement(true)
                            .build());
                }},
                null,
                null
        );
        refPresentations.set(2, refPresentations.get(0));
        refPresentations.set(3, refPresentations.get(1));
        boolean[] presentationsMatched = new boolean[refPresentations.size()];
        mExtractor.selectTrack(ac4TrackIndex);
        for (int i = 0; i < refPresentations.size(); ) {
            List<AudioPresentation> presentations = mExtractor.getAudioPresentations(ac4TrackIndex);
            assertNotNull(presentations);
            // Assumes all presentation sets have the same number of presentations.
            assertEquals(refPresentations.get(i).size(), presentations.size());
            if (!audioPresentationSetMatchesReference(refPresentations.get(i), presentations)) {
                    // Time to advance to the next presentation set.
                    i++;
                    continue;
            }
            Log.d(TAG, "Matched presentation " + i);
            presentationsMatched[i] = true;
            // No need to wait for another switch after the last presentation has been matched.
            if (i == presentationsMatched.length - 1 || !mExtractor.advance()) {
                break;
            }
        }
        for (int i = 0; i < presentationsMatched.length; i++) {
            assertTrue("Presentation set " + i + " was not found in the stream",
                    presentationsMatched[i]);
        }
    }

    @AppModeFull(reason = "Instant apps cannot bind sockets.")
    public void testExtractorGetCachedDuration() throws Exception {
        CtsTestServer foo = new CtsTestServer(getContext());
        String url = foo.getAssetUrl("ringer.mp3");
        mExtractor.setDataSource(url);
        long cachedDurationUs = mExtractor.getCachedDuration();
        assertTrue("cached duration should be non-negative", cachedDurationUs >= 0);
    }

    public void testExtractorHasCacheReachedEndOfStream() throws Exception {
        // Using file source to get deterministic result.
        AssetFileDescriptor afd = mResources.openRawResourceFd(R.raw.testvideo);
        mExtractor.setDataSource(afd);
        assertTrue(mExtractor.hasCacheReachedEndOfStream());
        afd.close();
    }

    /*
     * Makes sure if PTS(order) of a video file with BFrames matches the expected values in
     * the corresponding text file with just PTS values.
     */
    public void testVideoPresentationTimeStampsMatch() throws Exception {
        setDataSource(R.raw.binary_counter_320x240_30fps_600frames);
        // Select the only video track present in the file.
        final int trackCount = mExtractor.getTrackCount();
        for (int i = 0; i < trackCount; i++) {
            mExtractor.selectTrack(i);
        }

        Reader txtRdr = new BufferedReader(new InputStreamReader(mResources.openRawResource(
                R.raw.timestamps_binary_counter_320x240_30fps_600frames)));
        StreamTokenizer strTok = new StreamTokenizer(txtRdr);
        strTok.parseNumbers();

        boolean srcAdvance = false;
        long srcSampleTimeUs = -1;
        long testSampleTimeUs = -1;

        strTok.nextToken();
        do {
            srcSampleTimeUs = mExtractor.getSampleTime();
            testSampleTimeUs = (long) strTok.nval;

            // Ignore round-off error if any.
            if (Math.abs(srcSampleTimeUs - testSampleTimeUs) > 1) {
                Log.d(TAG, "srcSampleTimeUs:" + srcSampleTimeUs + " testSampleTimeUs:" +
                        testSampleTimeUs);
                fail("video presentation timestamp not equal");
            }

            srcAdvance = mExtractor.advance();
            strTok.nextToken();
        } while (srcAdvance);
    }

    /* package */ static class ByteBufferDataSource extends MediaDataSource {
        private final long mSize;
        private TreeMap<Long, ByteBuffer> mMap = new TreeMap<Long, ByteBuffer>();

        public ByteBufferDataSource(MediaCodecTest.ByteBufferStream bufferStream)
                throws IOException {
            long size = 0;
            while (true) {
                final ByteBuffer buffer = bufferStream.read();
                if (buffer == null) break;
                final int limit = buffer.limit();
                if (limit == 0) continue;
                size += limit;
                mMap.put(size - 1, buffer); // key: last byte of validity for the buffer.
            }
            mSize = size;
        }

        @Override
        public long getSize() {
            return mSize;
        }

        @Override
        public int readAt(long position, byte[] buffer, int offset, int size) {
            Log.v(TAG, "reading at " + position + " offset " + offset + " size " + size);

            // This chooses all buffers with key >= position (e.g. valid buffers)
            final SortedMap<Long, ByteBuffer> map = mMap.tailMap(position);
            int copied = 0;
            for (Map.Entry<Long, ByteBuffer> e : map.entrySet()) {
                // Get a read-only version of the byte buffer.
                final ByteBuffer bb = e.getValue().asReadOnlyBuffer();
                // Convert read position to an offset within that byte buffer, bboffs.
                final long bboffs = position - e.getKey() + bb.limit() - 1;
                if (bboffs >= bb.limit() || bboffs < 0) {
                    break; // (negative position)?
                }
                bb.position((int)bboffs); // cast is safe as bb.limit is int.
                final int tocopy = Math.min(size, bb.remaining());
                if (tocopy == 0) {
                    break; // (size == 0)?
                }
                bb.get(buffer, offset, tocopy);
                copied += tocopy;
                size -= tocopy;
                offset += tocopy;
                position += tocopy;
                if (size == 0) {
                    break; // finished copying.
                }
            }
            if (copied == 0) {
                copied = -1;  // signal end of file
            }
            return copied;
        }

        @Override
        public void close() {
            mMap = null;
        }
    }

    /* package */ static class MediaExtractorStream
                extends MediaCodecTest.ByteBufferStream implements Closeable {
        public boolean mIsFloat;
        public boolean mSawOutputEOS;
        public MediaFormat mFormat;

        private MediaExtractor mExtractor;
        private MediaCodecTest.MediaCodecStream mDecoderStream;

        public MediaExtractorStream(
                String inMime, String outMime,
                MediaDataSource dataSource) throws Exception {
            mExtractor = new MediaExtractor();
            mExtractor.setDataSource(dataSource);
            final int numTracks = mExtractor.getTrackCount();
            // Single track?
            // assertEquals("Number of tracks should be 1", 1, numTracks);
            for (int i = 0; i < numTracks; ++i) {
                final MediaFormat format = mExtractor.getTrackFormat(i);
                final String actualMime = format.getString(MediaFormat.KEY_MIME);
                mExtractor.selectTrack(i);
                mFormat = format;
                if (outMime.equals(actualMime)) {
                    break;
                } else { // no matching mime, try to use decoder
                    mDecoderStream = new MediaCodecTest.MediaCodecStream(
                            mExtractor, mFormat);
                    Log.w(TAG, "fallback to input mime type with decoder");
                }
            }
            assertNotNull("MediaExtractor cannot find mime type " + inMime, mFormat);
            mIsFloat = mFormat.getInteger(
                    MediaFormat.KEY_PCM_ENCODING, AudioFormat.ENCODING_PCM_16BIT)
                            == AudioFormat.ENCODING_PCM_FLOAT;
        }

        public MediaExtractorStream(
                String inMime, String outMime,
                MediaCodecTest.ByteBufferStream inputStream) throws Exception {
            this(inMime, outMime, new ByteBufferDataSource(inputStream));
        }

        @Override
        public ByteBuffer read() throws IOException {
            if (mSawOutputEOS) {
                return null;
            }
            if (mDecoderStream != null) {
                return mDecoderStream.read();
            }
            // To preserve codec-like behavior, we create ByteBuffers
            // equal to the media sample size.
            final long size = mExtractor.getSampleSize();
            if (size >= 0) {
                final ByteBuffer inputBuffer = ByteBuffer.allocate((int)size);
                final int red = mExtractor.readSampleData(inputBuffer, 0 /* offset */); // sic
                if (red >= 0) {
                    assertEquals("position must be zero", 0, inputBuffer.position());
                    assertEquals("limit must be read bytes", red, inputBuffer.limit());
                    mExtractor.advance();
                    return inputBuffer;
                }
            }
            mSawOutputEOS = true;
            return null;
        }

        @Override
        public void close() throws IOException {
            if (mExtractor != null) {
                mExtractor.release();
                mExtractor = null;
            }
            mFormat = null;
        }

        @Override
        protected void finalize() throws Throwable {
            if (mExtractor != null) {
                Log.w(TAG, "MediaExtractorStream wasn't closed");
                mExtractor.release();
            }
            mFormat = null;
        }
    }

    @SmallTest
    public void testFlacIdentity() throws Exception {
        final int PCM_FRAMES = 1152 * 4; // FIXME: requires 4 flac frames to work with OMX codecs.
        final int CHANNEL_COUNT = 2;
        final int SAMPLES = PCM_FRAMES * CHANNEL_COUNT;
        final int[] SAMPLE_RATES = {44100, 192000}; // ensure 192kHz supported.

        for (int sampleRate : SAMPLE_RATES) {
            final MediaFormat format = new MediaFormat();
            format.setString(MediaFormat.KEY_MIME, MediaFormat.MIMETYPE_AUDIO_FLAC);
            format.setInteger(MediaFormat.KEY_SAMPLE_RATE, sampleRate);
            format.setInteger(MediaFormat.KEY_CHANNEL_COUNT, CHANNEL_COUNT);

            Log.d(TAG, "Trying sample rate: " + sampleRate
                    + " channel count: " + CHANNEL_COUNT);
            format.setInteger(MediaFormat.KEY_FLAC_COMPRESSION_LEVEL, 5);

            // TODO: Add float mode when MediaExtractor supports float configuration.
            final MediaCodecTest.PcmAudioBufferStream audioStream =
                    new MediaCodecTest.PcmAudioBufferStream(
                            SAMPLES, sampleRate, 1000 /* frequency */, 100 /* sweep */,
                          false /* useFloat */);

            final MediaCodecTest.MediaCodecStream rawToFlac =
                    new MediaCodecTest.MediaCodecStream(
                            new MediaCodecTest.ByteBufferInputStream(audioStream),
                            format, true /* encode */);
            final MediaExtractorStream flacToRaw =
                    new MediaExtractorStream(MediaFormat.MIMETYPE_AUDIO_FLAC /* inMime */,
                            MediaFormat.MIMETYPE_AUDIO_RAW /* outMime */, rawToFlac);

            // Note: the existence of signed zero (as well as NAN) may make byte
            // comparisons invalid for floating point output. In our case, since the
            // floats come through integer to float conversion, it does not matter.
            assertEquals("Audio data not identical after compression",
                audioStream.sizeInBytes(),
                MediaCodecTest.compareStreams(new MediaCodecTest.ByteBufferInputStream(flacToRaw),
                    new MediaCodecTest.ByteBufferInputStream(
                            new MediaCodecTest.PcmAudioBufferStream(audioStream))));
        }
    }

    public void testFlacMovExtraction() throws Exception {
        AssetFileDescriptor testFd = mResources.openRawResourceFd(R.raw.sinesweepalac);

        MediaExtractor extractor = new MediaExtractor();
        extractor.setDataSource(testFd.getFileDescriptor(), testFd.getStartOffset(),
                testFd.getLength());
        testFd.close();
        extractor.selectTrack(0);
        boolean lastAdvanceResult = true;
        boolean lastReadResult = true;
        ByteBuffer buf = ByteBuffer.allocate(2*1024*1024);
        int totalSize = 0;
        while(true) {
            int n = extractor.readSampleData(buf, 0);
            if (n > 0) {
                totalSize += n;
            }
            if (!extractor.advance()) {
                break;
            }
        }
        assertTrue("could not read alac mov", totalSize > 0);
    }

    private void doTestAdvance(int res) throws Exception {
        AssetFileDescriptor testFd = mResources.openRawResourceFd(res);

        MediaExtractor extractor = new MediaExtractor();
        extractor.setDataSource(testFd.getFileDescriptor(), testFd.getStartOffset(),
                testFd.getLength());
        testFd.close();
        extractor.selectTrack(0);
        boolean lastAdvanceResult = true;
        boolean lastReadResult = true;
        ByteBuffer buf = ByteBuffer.allocate(2*1024*1024);
        while(lastAdvanceResult || lastReadResult) {
            int n = extractor.readSampleData(buf, 0);
            if (lastAdvanceResult) {
                // previous advance() was successful, so readSampleData() should succeed
                assertTrue("readSampleData() failed after successful advance()", n >= 0);
                assertTrue("getSampleTime() failed after succesful advance()",
                        extractor.getSampleTime() >= 0);
                assertTrue("getSampleSize() failed after succesful advance()",
                        extractor.getSampleSize() >= 0);
                assertTrue("getSampleTrackIndex() failed after succesful advance()",
                        extractor.getSampleTrackIndex() >= 0);
            } else {
                // previous advance() failed, so readSampleData() should fail too
                assertTrue("readSampleData() succeeded after failed advance()", n < 0);
                assertTrue("getSampleTime() succeeded after failed advance()",
                        extractor.getSampleTime() < 0);
                assertTrue("getSampleSize() succeeded after failed advance()",
                        extractor.getSampleSize() < 0);
                assertTrue("getSampleTrackIndex() succeeded after failed advance()",
                        extractor.getSampleTrackIndex() < 0);
            }
            lastReadResult = (n >= 0);
            lastAdvanceResult = extractor.advance();
        }
        extractor.release();
    }

    public void testAdvance() throws Exception {
        // audio-only
        doTestAdvance(R.raw.sinesweepm4a);
        doTestAdvance(R.raw.sinesweepmp3lame);
        doTestAdvance(R.raw.sinesweepmp3smpb);
        doTestAdvance(R.raw.sinesweepwav);
        doTestAdvance(R.raw.sinesweepflac);
        doTestAdvance(R.raw.sinesweepogg);
        doTestAdvance(R.raw.sinesweepoggmkv);

        // video-only
        doTestAdvance(R.raw.swirl_144x136_mpeg4);
        doTestAdvance(R.raw.video_640x360_mp4_hevc_450kbps_no_b);

        // audio+video
        doTestAdvance(R.raw.video_480x360_mp4_h264_500kbps_30fps_aac_stereo_128kbps_44100hz);
        doTestAdvance(R.raw.video_1280x720_mkv_h265_500kbps_25fps_aac_stereo_128kbps_44100hz);
    }
}
