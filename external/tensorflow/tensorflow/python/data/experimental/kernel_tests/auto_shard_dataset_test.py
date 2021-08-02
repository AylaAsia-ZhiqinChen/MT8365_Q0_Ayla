# Copyright 2019 The TensorFlow Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================
"""Tests for the private `_AutoShardDataset` transformation."""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from absl.testing import parameterized

from tensorflow.python.data.experimental.kernel_tests import reader_dataset_ops_test_base
from tensorflow.python.data.experimental.ops import distribute
from tensorflow.python.data.experimental.ops import interleave_ops
from tensorflow.python.data.experimental.ops import readers
from tensorflow.python.data.experimental.ops import unique
from tensorflow.python.data.ops import dataset_ops
from tensorflow.python.data.ops import readers as core_readers
from tensorflow.python.framework import errors
from tensorflow.python.framework import test_util
from tensorflow.python.ops import string_ops
from tensorflow.python.platform import test


def chunk(l, n):
  for i in range(0, len(l), n):
    yield l[i:i + n]


@test_util.run_all_in_graph_and_eager_modes
class AutoShardDatasetTest(reader_dataset_ops_test_base.TFRecordDatasetTestBase,
                           parameterized.TestCase):

  def setUp(self):
    super(AutoShardDatasetTest, self).setUp()
    self._num_files = 10
    self._num_records = 10
    self.test_filenames = self._createFiles()

  def testFlatMapReaderPipeline(self):
    dataset = dataset_ops.Dataset.list_files(self.test_filenames, shuffle=True)
    dataset = dataset.flat_map(core_readers.TFRecordDataset)
    dataset = dataset.batch(5)
    dataset = distribute._AutoShardDataset(dataset, 5, 3)

    expected = [
        b"Record %d of file %d" % (r, f)  # pylint:disable=g-complex-comprehension
        for f in (3, 8)
        for r in range(0, 10)
    ]
    self.assertDatasetProduces(dataset, list(chunk(expected, 5)))

  def testZipReaderPipeline(self):
    dataset1 = dataset_ops.Dataset.list_files(self.test_filenames, shuffle=True)
    dataset1 = dataset1.apply(
        interleave_ops.parallel_interleave(core_readers.TFRecordDataset, 10))
    dataset2 = dataset_ops.Dataset.list_files(self.test_filenames, shuffle=True)
    dataset2 = dataset2.apply(
        interleave_ops.parallel_interleave(core_readers.TFRecordDataset, 10))

    dataset = dataset_ops.Dataset.zip((dataset1, dataset2))
    dataset = distribute._AutoShardDataset(dataset, 5, 3)

    expected = [
        (b"Record %d of file %d" % (r, f), b"Record %d of file %d" % (r, f))  # pylint:disable=g-complex-comprehension
        for r in range(0, 10)
        for f in (3, 8)
    ]

    self.assertDatasetProduces(dataset, expected)

  def testConcatenateReaderPipeline(self):
    dataset1 = dataset_ops.Dataset.list_files(self.test_filenames, shuffle=True)
    dataset1 = dataset1.apply(
        interleave_ops.parallel_interleave(core_readers.TFRecordDataset, 10))
    dataset1 = dataset1.batch(5)
    dataset2 = dataset_ops.Dataset.list_files(self.test_filenames, shuffle=True)
    dataset2 = dataset2.apply(
        interleave_ops.parallel_interleave(core_readers.TFRecordDataset, 10))
    dataset2 = dataset2.batch(5)

    dataset = dataset1.concatenate(dataset2)
    dataset = distribute._AutoShardDataset(dataset, 5, 3)

    expected = [
        b"Record %d of file %d" % (r, f)  # pylint:disable=g-complex-comprehension
        for r in range(0, 10)
        for f in (3, 8)
    ]
    expected += expected
    self.assertDatasetProduces(dataset, list(chunk(expected, 5)))

  def testPipelineWithMap(self):
    dataset = dataset_ops.Dataset.list_files(self.test_filenames, shuffle=True)
    dataset = dataset.apply(
        interleave_ops.parallel_interleave(core_readers.TFRecordDataset, 10))
    dataset = dataset.map(lambda x: string_ops.substr_v2(x, 2, 1000))
    dataset = dataset.batch(5)
    dataset = distribute._AutoShardDataset(dataset, 5, 3)

    expected = [
        b"cord %d of file %d" % (r, f)  # pylint:disable=g-complex-comprehension
        for r in range(0, 10)
        for f in (3, 8)
    ]
    self.assertDatasetProduces(dataset, list(chunk(expected, 5)))

  def testValidPipelineWithRangeDataset(self):
    dataset = dataset_ops.Dataset.range(self._num_files)
    dataset = dataset.map(lambda n: string_ops.string_join(  # pylint:disable=g-long-lambda
        [self.get_temp_dir(),
         string_ops.string_format("/tf_record.{}.txt", [n])]))
    dataset = dataset.apply(
        interleave_ops.parallel_interleave(core_readers.TFRecordDataset, 10))
    dataset = dataset.map(lambda x: string_ops.substr_v2(x, 2, 1000))
    dataset = dataset.batch(5)
    dataset = distribute._AutoShardDataset(dataset, 5, 3)

    expected = [
        b"cord %d of file %d" % (r, f)  # pylint:disable=g-complex-comprehension
        for r in range(0, 10)
        for f in (3, 8)
    ]
    self.assertDatasetProduces(dataset, list(chunk(expected, 5)))

  @parameterized.parameters((1, 0, 10, 10), (2, 1, 20, 5), (10, 1, 1, 10))
  def testStandardReaderPipeline(self, num_epochs, index, batch_size,
                                 parallel_reads):
    dataset = readers.make_tf_record_dataset(
        file_pattern=self.test_filenames,
        num_epochs=num_epochs,
        batch_size=batch_size,
        parser_fn=None,
        num_parallel_reads=parallel_reads,
        drop_final_batch=True,
        shuffle=False)
    dataset = distribute._AutoShardDataset(dataset, 2, index)
    outputs = self.getNext(dataset)
    self._verify_records(
        outputs,
        batch_size=batch_size,
        file_index=[i for i in range(index, self._num_records, 2)],
        num_epochs=num_epochs,
        interleave_cycle_length=parallel_reads,
        drop_final_batch=True,
        use_parser_fn=None)
    with self.assertRaises(errors.OutOfRangeError):
      self.evaluate(outputs())

  def testSampleResNetPipeline(self):
    dataset = dataset_ops.Dataset.list_files(self.test_filenames, shuffle=True)
    dataset = dataset.apply(
        interleave_ops.parallel_interleave(core_readers.TFRecordDataset, 10))
    dataset = dataset.batch(5)
    dataset = distribute._AutoShardDataset(dataset, 5, 3)

    expected = [
        b"Record %d of file %d" % (r, f)  # pylint:disable=g-complex-comprehension
        for r in range(0, 10)
        for f in (3, 8)
    ]
    self.assertDatasetProduces(dataset, list(chunk(expected, 5)))

  def testWorkersGreaterThanNumFiles(self):
    dataset = dataset_ops.Dataset.list_files(self.test_filenames, shuffle=True)
    dataset = dataset.apply(
        interleave_ops.parallel_interleave(core_readers.TFRecordDataset, 10))
    dataset = dataset.batch(5)
    dataset = distribute._AutoShardDataset(dataset, 500, 499)
    self.assertDatasetProduces(dataset, [])

  def testNoReaderPipelines(self):
    dataset = dataset_ops.Dataset.range(1024)
    with self.assertRaises(errors.NotFoundError):
      dataset = distribute._AutoShardDataset(dataset, 2, 0)
      self.evaluate(self.getNext(dataset)())

  def testUnsupportedOpInPipeline(self):
    dataset = dataset_ops.Dataset.list_files(self.test_filenames, shuffle=True)
    dataset = dataset.flat_map(core_readers.TFRecordDataset)
    dataset = dataset.batch(5)
    dataset = dataset.apply(unique.unique())

    with self.assertRaises(errors.NotFoundError):
      dataset = distribute._AutoShardDataset(dataset, 2, 0)
      self.evaluate(self.getNext(dataset)())

  def testInvalidWorkerIndex(self):
    dataset = dataset_ops.Dataset.list_files(self.test_filenames, shuffle=True)
    dataset = dataset.flat_map(core_readers.TFRecordDataset)
    dataset = dataset.batch(5)

    with self.assertRaises(errors.InvalidArgumentError):
      dataset = distribute._AutoShardDataset(dataset, 2, 2)
      self.evaluate(self.getNext(dataset)())


if __name__ == "__main__":
  test.main()
