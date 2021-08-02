# Copyright 2017 The TensorFlow Authors. All Rights Reserved.
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
"""Base class for testing the input pipeline statistics gathering ops."""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import re
import numpy as np

from tensorflow.core.framework import summary_pb2
from tensorflow.python.data.experimental.ops import stats_aggregator
from tensorflow.python.data.kernel_tests import test_base
from tensorflow.python.framework import errors


class StatsDatasetTestBase(test_base.DatasetTestBase):
  """Base class for testing statistics gathered in `StatsAggregator`."""

  def regexForNodeName(self, op_name, stats_type=""):
    return "".join([op_name, r"/_\d+::", stats_type])

  def _assertSummaryContains(self, summary_str, tag):
    summary_proto = summary_pb2.Summary()
    summary_proto.ParseFromString(summary_str)
    for value in summary_proto.value:
      if re.match(tag, value.tag):
        return
    self.fail("Expected tag %r not found in summary %r" % (tag, summary_proto))

  def _assertSummaryHasCount(self,
                             summary_str,
                             tag,
                             expected_value,
                             greater_than=False):
    summary_proto = summary_pb2.Summary()
    summary_proto.ParseFromString(summary_str)
    for value in summary_proto.value:
      if re.match(tag, value.tag):
        if greater_than:
          self.assertGreaterEqual(value.histo.num, expected_value)
        else:
          self.assertEqual(expected_value, value.histo.num)
        return
    self.fail("Expected tag %r not found in summary %r" % (tag, summary_proto))

  def _assertSummaryHasRange(self, summary_str, tag, min_value, max_value):
    summary_proto = summary_pb2.Summary()
    summary_proto.ParseFromString(summary_str)
    for value in summary_proto.value:
      if re.match(tag, value.tag):
        self.assertLessEqual(min_value, value.histo.min)
        self.assertGreaterEqual(max_value, value.histo.max)
        return
    self.fail("Expected tag %r not found in summary %r" % (tag, summary_proto))

  def _assertSummaryHasSum(self, summary_str, tag, expected_value):
    summary_proto = summary_pb2.Summary()
    summary_proto.ParseFromString(summary_str)
    for value in summary_proto.value:
      if re.match(tag, value.tag):
        self.assertEqual(expected_value, value.histo.sum)
        return
    self.fail("Expected tag %r not found in summary %r" % (tag, summary_proto))

  def _assertSummaryHasScalarValue(self, summary_str, tag, expected_value):
    summary_proto = summary_pb2.Summary()
    summary_proto.ParseFromString(summary_str)
    for value in summary_proto.value:
      if re.match(tag, value.tag):
        self.assertEqual(expected_value, value.simple_value)
        return
    self.fail("Expected tag %r not found in summary %r" % (tag, summary_proto))

  def _testParallelCallsStats(self,
                              dataset_fn,
                              dataset_names,
                              num_output,
                              dataset_transformation,
                              function_processing_time=False,
                              check_elements=True):
    aggregator = stats_aggregator.StatsAggregator()
    dataset = dataset_fn()
    dataset = dataset_transformation(dataset, aggregator)
    next_element = self.getNext(dataset, requires_initialization=True)

    for i in range(num_output):
      next_ = self.evaluate(next_element())
      if check_elements:
        self.assertAllEqual(np.array([i] * i, dtype=np.int64), next_)
      summary_str = self.evaluate(aggregator.get_summary())
      for dataset_name in dataset_names:
        if function_processing_time:
          self._assertSummaryHasCount(
              summary_str,
              r"(.*)::execution_time$",
              float(i + 1),
              greater_than=True)
        self._assertSummaryContains(summary_str,
                                    dataset_name + "thread_utilization")
    with self.assertRaises(errors.OutOfRangeError):
      self.evaluate(next_element())
    if function_processing_time:
      summary_str = self.evaluate(aggregator.get_summary())
      for dataset_name in dataset_names:
        self._assertSummaryHasCount(
            summary_str,
            r"(.*)::execution_time$",
            float(num_output),
            greater_than=True)
