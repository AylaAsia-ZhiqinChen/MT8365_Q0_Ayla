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
"""Tests for tensorflow.ops.linalg.linalg_impl.tridiagonal_solve."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import numpy as np

from tensorflow.python.client import session
from tensorflow.python.eager import context
from tensorflow.python.framework import constant_op
from tensorflow.python.framework import dtypes
from tensorflow.python.framework import errors_impl
from tensorflow.python.framework import ops
from tensorflow.python.ops import array_ops
from tensorflow.python.ops import control_flow_ops
from tensorflow.python.ops import random_ops
from tensorflow.python.ops import variables
from tensorflow.python.ops.linalg import linalg_impl
from tensorflow.python.platform import benchmark
from tensorflow.python.platform import test

_sample_diags = np.array([[2, 1, 4, 0], [1, 3, 2, 2], [0, 1, -1, 1]])
_sample_rhs = np.array([1, 2, 3, 4])
_sample_result = np.array([-9, 5, -4, 4])


def _tfconst(array):
  return constant_op.constant(array, dtypes.float64)


def _tf_ones(shape):
  return array_ops.ones(shape, dtype=dtypes.float64)


class TridiagonalSolveOpTest(test.TestCase):

  def _test(self,
            diags,
            rhs,
            expected,
            diags_format="compact",
            transpose_rhs=False,
            conjugate_rhs=False):
    with self.cached_session(use_gpu=False):
      result = linalg_impl.tridiagonal_solve(diags, rhs, diags_format,
                                             transpose_rhs, conjugate_rhs)
      self.assertAllClose(self.evaluate(result), expected)

  def _testWithLists(self,
                     diags,
                     rhs,
                     expected,
                     diags_format="compact",
                     transpose_rhs=False,
                     conjugate_rhs=False):
    self._test(
        _tfconst(diags), _tfconst(rhs), _tfconst(expected), diags_format,
        transpose_rhs, conjugate_rhs)

  def _assertRaises(self, diags, rhs, diags_format="compact"):
    with self.assertRaises(ValueError):
      linalg_impl.tridiagonal_solve(diags, rhs, diags_format)

  # Tests with various dtypes

  def testReal(self):
    for dtype in dtypes.float32, dtypes.float64:
      self._test(
          diags=constant_op.constant(_sample_diags, dtype),
          rhs=constant_op.constant(_sample_rhs, dtype),
          expected=constant_op.constant(_sample_result, dtype))

  def testComplex(self):
    for dtype in dtypes.complex64, dtypes.complex128:
      self._test(
          diags=constant_op.constant(_sample_diags, dtype) * (1 + 1j),
          rhs=constant_op.constant(_sample_rhs, dtype) * (1 - 1j),
          expected=constant_op.constant(_sample_result, dtype) * (1 - 1j) /
          (1 + 1j))

  # Tests with small matrix sizes

  def test3x3(self):
    self._testWithLists(
        diags=[[2, -1, 0], [1, 3, 1], [0, -1, -2]],
        rhs=[1, 2, 3],
        expected=[-3, 2, 7])

  def test2x2(self):
    self._testWithLists(
        diags=[[2, 0], [1, 3], [0, 1]], rhs=[1, 4], expected=[-5, 3])

  def test1x1(self):
    self._testWithLists(diags=[[0], [3], [0]], rhs=[6], expected=[2])

  def test0x0(self):
    self._test(
        diags=constant_op.constant(0, shape=(3, 0), dtype=dtypes.float32),
        rhs=constant_op.constant(0, shape=(0, 1), dtype=dtypes.float32),
        expected=constant_op.constant(0, shape=(0, 1), dtype=dtypes.float32))

  # Other edge cases

  def testCaseRequiringPivoting(self):
    # Without partial pivoting (e.g. Thomas algorithm) this would fail.
    self._testWithLists(
        diags=[[2, -1, 1, 0], [1, 4, 1, -1], [0, 2, -2, 3]],
        rhs=[1, 2, 3, 4],
        expected=[8, -3.5, 0, -4])

  def testCaseRequiringPivotingLastRows(self):
    self._testWithLists(
        diags=[[2, 1, -1, 0], [1, -1, 2, 1], [0, 1, -6, 1]],
        rhs=[1, 2, -1, -2],
        expected=[5, -2, -5, 3])

  def testNotInvertible(self):
    with self.assertRaises(errors_impl.InvalidArgumentError):
      self._testWithLists(
          diags=[[2, -1, 1, 0], [1, 4, 1, -1], [0, 2, 0, 3]],
          rhs=[1, 2, 3, 4],
          expected=[8, -3.5, 0, -4])

  def testDiagonal(self):
    self._testWithLists(
        diags=[[0, 0, 0, 0], [1, 2, -1, -2], [0, 0, 0, 0]],
        rhs=[1, 2, 3, 4],
        expected=[1, 1, -3, -2])

  def testUpperTriangular(self):
    self._testWithLists(
        diags=[[2, 4, -1, 0], [1, 3, 1, 2], [0, 0, 0, 0]],
        rhs=[1, 6, 4, 4],
        expected=[13, -6, 6, 2])

  def testLowerTriangular(self):
    self._testWithLists(
        diags=[[0, 0, 0, 0], [2, -1, 3, 1], [0, 1, 4, 2]],
        rhs=[4, 5, 6, 1],
        expected=[2, -3, 6, -11])

  # Multiple right-hand sides and batching

  def testWithTwoRightHandSides(self):
    self._testWithLists(
        diags=_sample_diags,
        rhs=np.transpose([_sample_rhs, 2 * _sample_rhs]),
        expected=np.transpose([_sample_result, 2 * _sample_result]))

  def testBatching(self):
    self._testWithLists(
        diags=np.array([_sample_diags, -_sample_diags]),
        rhs=np.array([_sample_rhs, 2 * _sample_rhs]),
        expected=np.array([_sample_result, -2 * _sample_result]))

  def testBatchingAndTwoRightHandSides(self):
    rhs = np.transpose([_sample_rhs, 2 * _sample_rhs])
    expected_result = np.transpose([_sample_result, 2 * _sample_result])
    self._testWithLists(
        diags=np.array([_sample_diags, -_sample_diags]),
        rhs=np.array([rhs, 2 * rhs]),
        expected=np.array([expected_result, -2 * expected_result]))

  # Various input formats

  def testSequenceFormat(self):
    self._test(
        diags=(_tfconst([2, 1, 4]), _tfconst([1, 3, 2, 2]), _tfconst([1, -1,
                                                                      1])),
        rhs=_tfconst([1, 2, 3, 4]),
        expected=_tfconst([-9, 5, -4, 4]),
        diags_format="sequence")

  def testSequenceFormatWithDummyElements(self):
    dummy = 20
    self._test(
        diags=(_tfconst([2, 1, 4, dummy]), _tfconst([1, 3, 2, 2]),
               _tfconst([dummy, 1, -1, 1])),
        rhs=_tfconst([1, 2, 3, 4]),
        expected=_tfconst([-9, 5, -4, 4]),
        diags_format="sequence")

  def testSequenceFormatWithBatching(self):
    self._test(
        diags=(_tfconst([[2, 1, 4], [-2, -1, -4]]),
               _tfconst([[1, 3, 2, 2], [-1, -3, -2, -2]]),
               _tfconst([[1, -1, 1], [-1, 1, -1]])),
        rhs=_tfconst([[1, 2, 3, 4], [1, 2, 3, 4]]),
        expected=_tfconst([[-9, 5, -4, 4], [9, -5, 4, -4]]),
        diags_format="sequence")

  def testMatrixFormat(self):
    self._testWithLists(
        diags=[[1, 2, 0, 0], [1, 3, 1, 0], [0, -1, 2, 4], [0, 0, 1, 2]],
        rhs=[1, 2, 3, 4],
        expected=[-9, 5, -4, 4],
        diags_format="matrix")

  def testMatrixFormatWithMultipleRightHandSides(self):
    self._testWithLists(
        diags=[[1, 2, 0, 0], [1, 3, 1, 0], [0, -1, 2, 4], [0, 0, 1, 2]],
        rhs=[[1, -1], [2, -2], [3, -3], [4, -4]],
        expected=[[-9, 9], [5, -5], [-4, 4], [4, -4]],
        diags_format="matrix")

  def testMatrixFormatWithBatching(self):
    self._testWithLists(
        diags=[[[1, 2, 0, 0], [1, 3, 1, 0], [0, -1, 2, 4], [0, 0, 1, 2]],
               [[-1, -2, 0, 0], [-1, -3, -1, 0], [0, 1, -2, -4], [0, 0, -1,
                                                                  -2]]],
        rhs=[[1, 2, 3, 4], [1, 2, 3, 4]],
        expected=[[-9, 5, -4, 4], [9, -5, 4, -4]],
        diags_format="matrix")

  def testRightHandSideAsColumn(self):
    self._testWithLists(
        diags=_sample_diags,
        rhs=np.transpose([_sample_rhs]),
        expected=np.transpose([_sample_result]),
        diags_format="compact")

  # Tests with transpose and adjoint

  def testTransposeRhs(self):
    self._testWithLists(
        diags=_sample_diags,
        rhs=np.array([_sample_rhs, 2 * _sample_rhs]),
        expected=np.array([_sample_result, 2 * _sample_result]),
        transpose_rhs=True)

  def testConjugateRhs(self):
    self._testWithLists(
        diags=_sample_diags,
        rhs=np.transpose([_sample_rhs * (1 + 1j), _sample_rhs * (1 - 2j)]),
        expected=np.transpose(
            [_sample_result * (1 - 1j), _sample_result * (1 + 2j)]),
        conjugate_rhs=True)

  def testAdjointRhs(self):
    self._testWithLists(
        diags=_sample_diags,
        rhs=np.array([_sample_rhs * (1 + 1j), _sample_rhs * (1 - 2j)]),
        expected=np.array(
            [_sample_result * (1 - 1j), _sample_result * (1 + 2j)]),
        transpose_rhs=True,
        conjugate_rhs=True)

  def testTransposeRhsWithBatching(self):
    self._testWithLists(
        diags=np.array([_sample_diags, -_sample_diags]),
        rhs=np.array([[_sample_rhs, 2 * _sample_rhs],
                      [3 * _sample_rhs, 4 * _sample_rhs]]),
        expected=np.array([[_sample_result, 2 * _sample_result],
                           [-3 * _sample_result, -4 * _sample_result]]),
        transpose_rhs=True)

  def testTransposeRhsWithRhsAsVector(self):
    self._testWithLists(
        diags=_sample_diags,
        rhs=_sample_rhs,
        expected=_sample_result,
        transpose_rhs=True)

  def testConjugateRhsWithRhsAsVector(self):
    self._testWithLists(
        diags=_sample_diags,
        rhs=_sample_rhs * (1 + 1j),
        expected=_sample_result * (1 - 1j),
        conjugate_rhs=True)

  def testTransposeRhsWithRhsAsVectorAndBatching(self):
    self._testWithLists(
        diags=np.array([_sample_diags, -_sample_diags]),
        rhs=np.array([_sample_rhs, 2 * _sample_rhs]),
        expected=np.array([_sample_result, -2 * _sample_result]),
        transpose_rhs=True)

  # Invalid input shapes

  def testInvalidShapesCompactFormat(self):

    def test_raises(diags_shape, rhs_shape):
      self._assertRaises(_tf_ones(diags_shape), _tf_ones(rhs_shape), "compact")

    test_raises((5, 4, 4), (5, 4))
    test_raises((5, 3, 4), (4, 5))
    test_raises((5, 3, 4), (5))
    test_raises((5), (5, 4))

  def testInvalidShapesSequenceFormat(self):

    def test_raises(diags_tuple_shapes, rhs_shape):
      diagonals = tuple(_tf_ones(shape) for shape in diags_tuple_shapes)
      self._assertRaises(diagonals, _tf_ones(rhs_shape), "sequence")

    test_raises(((5, 4), (5, 4)), (5, 4))
    test_raises(((5, 4), (5, 4), (5, 6)), (5, 4))
    test_raises(((5, 3), (5, 4), (5, 6)), (5, 4))
    test_raises(((5, 6), (5, 4), (5, 3)), (5, 4))
    test_raises(((5, 4), (7, 4), (5, 4)), (5, 4))
    test_raises(((5, 4), (7, 4), (5, 4)), (3, 4))

  def testInvalidShapesMatrixFormat(self):

    def test_raises(diags_shape, rhs_shape):
      self._assertRaises(_tf_ones(diags_shape), _tf_ones(rhs_shape), "matrix")

    test_raises((5, 4, 7), (5, 4))
    test_raises((5, 4, 4), (3, 4))
    test_raises((5, 4, 4), (5, 3))

  # Tests with placeholders

  def _testWithPlaceholders(self,
                            diags_shape,
                            rhs_shape,
                            diags_feed,
                            rhs_feed,
                            expected,
                            diags_format="compact"):
    if context.executing_eagerly():
      return
    diags = array_ops.placeholder(dtypes.float64, shape=diags_shape)
    rhs = array_ops.placeholder(dtypes.float64, shape=rhs_shape)
    x = linalg_impl.tridiagonal_solve(diags, rhs, diags_format)
    with self.cached_session(use_gpu=False) as sess:
      result = sess.run(x, feed_dict={diags: diags_feed, rhs: rhs_feed})
      self.assertAllClose(result, expected)

  def testCompactFormatAllDimsUnknown(self):
    self._testWithPlaceholders(
        diags_shape=[None, None],
        rhs_shape=[None],
        diags_feed=_sample_diags,
        rhs_feed=_sample_rhs,
        expected=_sample_result)

  def testCompactFormatUnknownMatrixSize(self):
    self._testWithPlaceholders(
        diags_shape=[3, None],
        rhs_shape=[4],
        diags_feed=_sample_diags,
        rhs_feed=_sample_rhs,
        expected=_sample_result)

  def testCompactFormatUnknownRhsCount(self):
    self._testWithPlaceholders(
        diags_shape=[3, 4],
        rhs_shape=[4, None],
        diags_feed=_sample_diags,
        rhs_feed=np.transpose([_sample_rhs, 2 * _sample_rhs]),
        expected=np.transpose([_sample_result, 2 * _sample_result]))

  def testCompactFormatUnknownBatchSize(self):
    self._testWithPlaceholders(
        diags_shape=[None, 3, 4],
        rhs_shape=[None, 4],
        diags_feed=np.array([_sample_diags, -_sample_diags]),
        rhs_feed=np.array([_sample_rhs, 2 * _sample_rhs]),
        expected=np.array([_sample_result, -2 * _sample_result]))

  def testMatrixFormatWithUnknownDims(self):
    if context.executing_eagerly():
      return

    def test_with_matrix_shapes(matrix_shape):
      matrix = np.array([[1, 2, 0, 0], [1, 3, 1, 0], [0, -1, 2, 4],
                         [0, 0, 1, 2]])
      rhs = np.array([1, 2, 3, 4])
      x = np.array([-9, 5, -4, 4])
      self._testWithPlaceholders(
          diags_shape=matrix_shape,
          rhs_shape=[None, None],
          diags_feed=matrix,
          rhs_feed=np.transpose([rhs, 2 * rhs]),
          expected=np.transpose([x, 2 * x]),
          diags_format="matrix")

    test_with_matrix_shapes(matrix_shape=[4, 4])
    test_with_matrix_shapes(matrix_shape=[None, 4])
    test_with_matrix_shapes(matrix_shape=[4, None])
    with self.assertRaises(ValueError):
      test_with_matrix_shapes(matrix_shape=[None, None])

  def testSequenceFormatWithUnknownDims(self):
    if context.executing_eagerly():
      return
    superdiag = array_ops.placeholder(dtypes.float64, shape=[None])
    diag = array_ops.placeholder(dtypes.float64, shape=[None])
    subdiag = array_ops.placeholder(dtypes.float64, shape=[None])
    rhs = array_ops.placeholder(dtypes.float64, shape=[None])

    x = linalg_impl.tridiagonal_solve((superdiag, diag, subdiag),
                                      rhs,
                                      diagonals_format="sequence")
    with self.cached_session(use_gpu=False) as sess:
      result = sess.run(
          x,
          feed_dict={
              subdiag: [20, 1, -1, 1],
              diag: [1, 3, 2, 2],
              superdiag: [2, 1, 4, 20],
              rhs: [1, 2, 3, 4]
          })
      self.assertAllClose(result, [-9, 5, -4, 4])

  # Benchmark

  class TridiagonalSolveBenchmark(test.Benchmark):
    sizes = [(100000, 1, 1), (1000000, 1, 1), (10000000, 1, 1), (100000, 10, 1),
             (100000, 100, 1), (10000, 1, 100), (10000, 1, 1000),
             (10000, 1, 10000)]

    def _generateData(self, matrix_size, batch_size, num_rhs, seed=42):
      data = random_ops.random_normal(
          shape=(batch_size, 3 + num_rhs, matrix_size),
          dtype=dtypes.float64,
          seed=seed)
      diags = array_ops.stack([data[:, 0], data[:, 1], data[:, 2]], axis=-2)
      rhs = data[:, 3:, :]
      return diags, rhs

    def benchmarkTridiagonalSolveOp(self):
      for matrix_size, batch_size, num_rhs in self.sizes:
        with ops.Graph().as_default(), \
                session.Session(config=benchmark.benchmark_config()) as sess, \
                ops.device("/cpu:0"):
          diags, rhs = self._generateData(matrix_size, batch_size, num_rhs)
          x = linalg_impl.tridiagonal_solve(diags, rhs, transpose_rhs=True)
          variables.global_variables_initializer().run()
          self.run_op_benchmark(
              sess,
              control_flow_ops.group(x),
              min_iters=10,
              store_memory_usage=False,
              name=("tridiagonal_solve_matrix_size_{}_batch_size_{}_"
                    "num_rhs_{}").format(matrix_size, batch_size, num_rhs))


if __name__ == "__main__":
  test.main()
