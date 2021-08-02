// Copyright 2016 Google Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package com.google.archivepatcher.generator;

import java.io.File;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.List;
import org.junit.Assert;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

/** Tests for {@link TotalRecompressionLimiter}. */
@RunWith(JUnit4.class)
@SuppressWarnings("javadoc")
public class TotalRecompressionLimiterTest {

  private static final File OLD_FILE = null;
  private static final File NEW_FILE = null;

  private static final MinimalZipEntry UNIMPORTANT = makeFakeEntry("/unimportant", 1337);
  private static final MinimalZipEntry ENTRY_A_100K = makeFakeEntry("/a/100k", 100 * 1024);
  private static final MinimalZipEntry ENTRY_B_200K = makeFakeEntry("/b/200k", 200 * 1024);
  private static final MinimalZipEntry ENTRY_C_300K = makeFakeEntry("/c/300k", 300 * 1024);
  private static final MinimalZipEntry ENTRY_D_400K = makeFakeEntry("/d/400k", 400 * 1024);
  private static final MinimalZipEntry IGNORED_A = makeFakeEntry("/ignored/a", 1234);
  private static final MinimalZipEntry IGNORED_B = makeFakeEntry("/ignored/b", 5678);
  private static final MinimalZipEntry IGNORED_C = makeFakeEntry("/ignored/c", 9101112);
  private static final MinimalZipEntry IGNORED_D = makeFakeEntry("/ignored/d", 13141516);

  // First four recommendations are all ones where recompression is required. Note that there is a
  // mix of UNCOMPRESS_NEW and UNCOMPRESS_BOTH, both of which will have the "new" entry flagged for
  // recompression (i.e., should be relevant to the filtering logic).
  private static final QualifiedRecommendation REC_A_100K =
      new QualifiedRecommendation(
          UNIMPORTANT,
          ENTRY_A_100K,
          Recommendation.UNCOMPRESS_BOTH,
          RecommendationReason.COMPRESSED_BYTES_CHANGED);
  private static final QualifiedRecommendation REC_B_200K =
      new QualifiedRecommendation(
          UNIMPORTANT,
          ENTRY_B_200K,
          Recommendation.UNCOMPRESS_NEW,
          RecommendationReason.UNCOMPRESSED_CHANGED_TO_COMPRESSED);
  private static final QualifiedRecommendation REC_C_300K =
      new QualifiedRecommendation(
          UNIMPORTANT,
          ENTRY_C_300K,
          Recommendation.UNCOMPRESS_BOTH,
          RecommendationReason.COMPRESSED_BYTES_CHANGED);
  private static final QualifiedRecommendation REC_D_400K =
      new QualifiedRecommendation(
          UNIMPORTANT,
          ENTRY_D_400K,
          Recommendation.UNCOMPRESS_BOTH,
          RecommendationReason.COMPRESSED_BYTES_CHANGED);

  // Remaining recommendations are all ones where recompression is NOT required. Note the mixture of
  // UNCOMPRESS_NEITHER and UNCOMPRESS_OLD, neither of which will have the "new" entry flagged for
  // recompression (ie., must be ignored by the filtering logic).
  private static final QualifiedRecommendation REC_IGNORED_A_UNCHANGED =
      new QualifiedRecommendation(
          UNIMPORTANT,
          IGNORED_A,
          Recommendation.UNCOMPRESS_NEITHER,
          RecommendationReason.COMPRESSED_BYTES_IDENTICAL);
  private static final QualifiedRecommendation REC_IGNORED_B_BOTH_UNCOMPRESSED =
      new QualifiedRecommendation(
          UNIMPORTANT,
          IGNORED_B,
          Recommendation.UNCOMPRESS_NEITHER,
          RecommendationReason.BOTH_ENTRIES_UNCOMPRESSED);
  private static final QualifiedRecommendation REC_IGNORED_C_UNSUITABLE =
      new QualifiedRecommendation(
          UNIMPORTANT,
          IGNORED_C,
          Recommendation.UNCOMPRESS_NEITHER,
          RecommendationReason.UNSUITABLE);
  private static final QualifiedRecommendation REC_IGNORED_D_CHANGED_TO_UNCOMPRESSED =
      new QualifiedRecommendation(
          UNIMPORTANT,
          IGNORED_D,
          Recommendation.UNCOMPRESS_OLD,
          RecommendationReason.COMPRESSED_CHANGED_TO_UNCOMPRESSED);

  /** Convenience reference to all the recommendations that should be ignored by filtering. */
  private static final List<QualifiedRecommendation> ALL_IGNORED_RECS =
      Collections.unmodifiableList(
          Arrays.asList(
              REC_IGNORED_A_UNCHANGED,
              REC_IGNORED_B_BOTH_UNCOMPRESSED,
              REC_IGNORED_C_UNSUITABLE,
              REC_IGNORED_D_CHANGED_TO_UNCOMPRESSED));

  /** Convenience reference to all the recommendations that are subject to filtering. */
  private static final List<QualifiedRecommendation> ALL_RECS =
      Collections.unmodifiableList(
          Arrays.asList(
              REC_IGNORED_A_UNCHANGED,
              REC_A_100K,
              REC_IGNORED_B_BOTH_UNCOMPRESSED,
              REC_D_400K,
              REC_IGNORED_C_UNSUITABLE,
              REC_B_200K,
              REC_IGNORED_D_CHANGED_TO_UNCOMPRESSED,
              REC_C_300K));

  /**
   * Given {@link QualifiedRecommendation}s, manufacture equivalents altered in the way that the
   * {@link TotalRecompressionLimiter} would.
   *
   * @param originals the original recommendations
   * @return the altered recommendations
   */
  private static final List<QualifiedRecommendation> suppressed(
      QualifiedRecommendation... originals) {
    List<QualifiedRecommendation> result = new ArrayList<>(originals.length);
    for (QualifiedRecommendation original : originals) {
      result.add(
          new QualifiedRecommendation(
              original.getOldEntry(),
              original.getNewEntry(),
              Recommendation.UNCOMPRESS_NEITHER,
              RecommendationReason.RESOURCE_CONSTRAINED));
    }
    return result;
  }

  /**
   * Make a structurally valid but totally bogus {@link MinimalZipEntry} for the purpose of testing
   * the {@link RecommendationModifier}.
   *
   * @param path the path to set on the entry, to help with debugging
   * @param uncompressedSize the uncompressed size of the entry, in bytes
   * @return the entry
   */
  private static MinimalZipEntry makeFakeEntry(String path, long uncompressedSize) {
    try {
      return new MinimalZipEntry(
          8, // == deflate
          0, // crc32OfUncompressedData (ignored for this test)
          0, // compressedSize (ignored for this test)
          uncompressedSize,
          path.getBytes("UTF8"),
          true, // generalPurposeFlagBit11 (true=UTF8)
          0 // fileOffsetOfLocalEntry (ignored for this test)
          );
    } catch (UnsupportedEncodingException e) {
      throw new RuntimeException(e); // Impossible on any modern system
    }
  }

  @Test
  public void testNegativeLimit() {
    try {
      new TotalRecompressionLimiter(-1);
      Assert.fail("Set a negative limit");
    } catch (IllegalArgumentException expected) {
      // Pass
    }
  }

  /**
   * Asserts that the two collections contain exactly the same elements. This isn't as rigorous as
   * it should be, but is ok for this test scenario. Checks the contents but not the iteration order
   * of the collections handed in.
   *
   * @param c1 the first collection
   * @param c2 the second collection
   */
  private static <T> void assertEquivalence(Collection<T> c1, Collection<T> c2) {
    Assert.assertEquals(c1.size(), c2.size());
    Assert.assertTrue(c1.containsAll(c2));
    Assert.assertTrue(c2.containsAll(c1));
  }

  @Test
  public void testZeroLimit() {
    TotalRecompressionLimiter limiter = new TotalRecompressionLimiter(0);
    List<QualifiedRecommendation> expected = new ArrayList<QualifiedRecommendation>();
    expected.addAll(suppressed(REC_A_100K, REC_B_200K, REC_C_300K, REC_D_400K));
    expected.addAll(ALL_IGNORED_RECS);
    assertEquivalence(expected, limiter.getModifiedRecommendations(OLD_FILE, NEW_FILE, ALL_RECS));
  }

  @Test
  public void testMaxLimit() {
    TotalRecompressionLimiter limiter = new TotalRecompressionLimiter(Long.MAX_VALUE);
    assertEquivalence(ALL_RECS, limiter.getModifiedRecommendations(OLD_FILE, NEW_FILE, ALL_RECS));
  }

  @Test
  public void testLimit_ExactlySmallest() {
    long limit = REC_A_100K.getNewEntry().getUncompressedSize(); // Exactly large enough
    TotalRecompressionLimiter limiter = new TotalRecompressionLimiter(limit);
    List<QualifiedRecommendation> expected = new ArrayList<QualifiedRecommendation>();
    expected.add(REC_A_100K);
    expected.addAll(suppressed(REC_B_200K, REC_C_300K, REC_D_400K));
    expected.addAll(ALL_IGNORED_RECS);
    assertEquivalence(expected, limiter.getModifiedRecommendations(OLD_FILE, NEW_FILE, ALL_RECS));
  }

  @Test
  public void testLimit_EdgeUnderSmallest() {
    long limit = REC_A_100K.getNewEntry().getUncompressedSize() - 1; // 1 byte too small
    TotalRecompressionLimiter limiter = new TotalRecompressionLimiter(limit);
    List<QualifiedRecommendation> expected = new ArrayList<QualifiedRecommendation>();
    expected.addAll(suppressed(REC_A_100K, REC_B_200K, REC_C_300K, REC_D_400K));
    expected.addAll(ALL_IGNORED_RECS);
    assertEquivalence(expected, limiter.getModifiedRecommendations(OLD_FILE, NEW_FILE, ALL_RECS));
  }

  @Test
  public void testLimit_EdgeOverSmallest() {
    long limit = REC_A_100K.getNewEntry().getUncompressedSize() + 1; // 1 byte extra room
    TotalRecompressionLimiter limiter = new TotalRecompressionLimiter(limit);
    List<QualifiedRecommendation> expected = new ArrayList<QualifiedRecommendation>();
    expected.add(REC_A_100K);
    expected.addAll(suppressed(REC_B_200K, REC_C_300K, REC_D_400K));
    expected.addAll(ALL_IGNORED_RECS);
    assertEquivalence(expected, limiter.getModifiedRecommendations(OLD_FILE, NEW_FILE, ALL_RECS));
  }

  @Test
  public void testLimit_ExactlyLargest() {
    long limit = REC_D_400K.getNewEntry().getUncompressedSize(); // Exactly large enough
    TotalRecompressionLimiter limiter = new TotalRecompressionLimiter(limit);
    List<QualifiedRecommendation> expected = new ArrayList<QualifiedRecommendation>();
    expected.add(REC_D_400K);
    expected.addAll(suppressed(REC_A_100K, REC_B_200K, REC_C_300K));
    expected.addAll(ALL_IGNORED_RECS);
    assertEquivalence(expected, limiter.getModifiedRecommendations(OLD_FILE, NEW_FILE, ALL_RECS));
  }

  @Test
  public void testLimit_EdgeUnderLargest() {
    long limit = REC_D_400K.getNewEntry().getUncompressedSize() - 1; // 1 byte too small
    TotalRecompressionLimiter limiter = new TotalRecompressionLimiter(limit);
    List<QualifiedRecommendation> expected = new ArrayList<QualifiedRecommendation>();
    expected.add(REC_C_300K);
    expected.addAll(suppressed(REC_A_100K, REC_B_200K, REC_D_400K));
    expected.addAll(ALL_IGNORED_RECS);
    assertEquivalence(expected, limiter.getModifiedRecommendations(OLD_FILE, NEW_FILE, ALL_RECS));
  }

  @Test
  public void testLimit_EdgeOverLargest() {
    long limit = REC_D_400K.getNewEntry().getUncompressedSize() + 1; // 1 byte extra room
    TotalRecompressionLimiter limiter = new TotalRecompressionLimiter(limit);
    List<QualifiedRecommendation> expected = new ArrayList<QualifiedRecommendation>();
    expected.add(REC_D_400K);
    expected.addAll(suppressed(REC_A_100K, REC_B_200K, REC_C_300K));
    expected.addAll(ALL_IGNORED_RECS);
    assertEquivalence(expected, limiter.getModifiedRecommendations(OLD_FILE, NEW_FILE, ALL_RECS));
  }

  @Test
  public void testLimit_Complex() {
    // A more nuanced test. Here we set up a limit of 600k - big enough to get the largest and the
    // THIRD largest files. The second largest will fail because there isn't enough space after
    // adding the first largest, and the fourth largest will fail because there is not enough space
    // after adding the third largest. Tricky.
    long limit =
        REC_D_400K.getNewEntry().getUncompressedSize()
            + REC_B_200K.getNewEntry().getUncompressedSize();
    TotalRecompressionLimiter limiter = new TotalRecompressionLimiter(limit);
    List<QualifiedRecommendation> expected = new ArrayList<QualifiedRecommendation>();
    expected.add(REC_B_200K);
    expected.add(REC_D_400K);
    expected.addAll(suppressed(REC_A_100K, REC_C_300K));
    expected.addAll(ALL_IGNORED_RECS);
    assertEquivalence(expected, limiter.getModifiedRecommendations(OLD_FILE, NEW_FILE, ALL_RECS));
  }
}
