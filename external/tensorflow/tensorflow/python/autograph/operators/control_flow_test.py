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
"""Tests for control_flow module."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from tensorflow.python.autograph.operators import control_flow
from tensorflow.python.data.ops import dataset_ops
from tensorflow.python.framework import constant_op
from tensorflow.python.framework import dtypes
from tensorflow.python.framework import test_util
from tensorflow.python.ops import variables
from tensorflow.python.platform import test


class ForLoopTest(test.TestCase):

  @test_util.run_deprecated_v1
  def test_tensor(self):
    s = control_flow.for_stmt(
        constant_op.constant([1, 2, 3, 4]),
        extra_test=lambda s: True,
        body=lambda i, s: (s + i,),
        init_state=(0,))
    with self.cached_session():
      self.assertEqual((10,), self.evaluate(s))

  def test_python(self):
    s = control_flow.for_stmt(
        range(5),
        extra_test=lambda s: True,
        body=lambda i, s: (s + i,),
        init_state=(0,))
    self.assertEqual((10,), s)

  def test_dataset(self):
    s = control_flow.for_stmt(
        dataset_ops.Dataset.range(5),
        extra_test=None,
        body=lambda i, s: (s + i,),
        init_state=(constant_op.constant(0, dtype=dtypes.int64),))
    self.assertEqual(self.evaluate(s), (10,))

  @test_util.run_v2_only
  def test_dataset_no_state(self):
    v = variables.Variable(0, dtype=dtypes.int64)
    def stateless_with_side_effects(i):
      v.assign(v.read_value() + i)
    s = control_flow.for_stmt(
        dataset_ops.Dataset.range(5),
        extra_test=None,
        body=stateless_with_side_effects,
        init_state=())
    self.evaluate(s)
    self.assertEqual(self.evaluate(v.read_value()), 10)


class WhileLoopTest(test.TestCase):

  @test_util.run_deprecated_v1
  def test_tensor(self):
    n = constant_op.constant(5)
    results = control_flow.while_stmt(
        test=lambda i, s: i < n,
        body=lambda i, s: (i + 1, s + i,),
        init_state=(0, 0),
        extra_deps=(n,))
    self.assertEqual((5, 10), self.evaluate(results))

  @test_util.run_deprecated_v1
  def test_python_with_tensor_state(self):
    n = 5
    results = control_flow.while_stmt(
        test=lambda i, s: i < n,
        body=lambda i, s: (i + 1, s + i),
        init_state=(0, constant_op.constant(0)),
        extra_deps=())
    result_i, result_s = results
    self.assertEqual(5, result_i)
    self.assertEqual(10, self.evaluate(result_s))

  @test_util.run_deprecated_v1
  def test_python_due_to_hidden_cond_type(self):
    n = 5

    # TODO(b/124002646): Improve the error message.
    with self.assertRaises(Exception):
      control_flow.while_stmt(
          test=lambda i, s: i < n,
          body=lambda i, s: (i + 1, s + i),
          init_state=(constant_op.constant(0), constant_op.constant(0)),
          extra_deps=())

  def test_python(self):
    n = 5
    results = control_flow.while_stmt(
        test=lambda i, s: i < n,
        body=lambda i, s: (i + 1, s + i),
        init_state=(0, 0),
        extra_deps=(n,))
    self.assertEqual((5, 10), results)


class IfStmtTest(test.TestCase):

  def single_return_if_stmt(self, cond):
    return control_flow.if_stmt(
        cond=cond,
        body=lambda: 1,
        orelse=lambda: -1,
        get_state=lambda: (),
        set_state=lambda _: None)

  def multi_return_if_stmt(self, cond):
    return control_flow.if_stmt(
        cond=cond,
        body=lambda: (1, 2),
        orelse=lambda: (-1, -2),
        get_state=lambda: (),
        set_state=lambda _: None)

  @test_util.run_deprecated_v1
  def test_tensor(self):
    with self.cached_session():
      t = self.single_return_if_stmt(constant_op.constant(True))
      self.assertEqual(1, self.evaluate(t))
      t = self.single_return_if_stmt(constant_op.constant(False))
      self.assertEqual(-1, self.evaluate(t))

  def test_python(self):
    self.assertEqual(1, self.single_return_if_stmt(True))
    self.assertEqual(-1, self.single_return_if_stmt(False))

  @test_util.run_deprecated_v1
  def test_tensor_multiple_returns(self):
    with self.cached_session():
      t = self.multi_return_if_stmt(constant_op.constant(True))
      self.assertAllEqual([1, 2], self.evaluate(t))
      t = self.multi_return_if_stmt(constant_op.constant(False))
      self.assertAllEqual([-1, -2], self.evaluate(t))

  def test_python_multiple_returns(self):
    self.assertEqual((1, 2), self.multi_return_if_stmt(True))
    self.assertEqual((-1, -2), self.multi_return_if_stmt(False))


if __name__ == '__main__':
  test.main()
