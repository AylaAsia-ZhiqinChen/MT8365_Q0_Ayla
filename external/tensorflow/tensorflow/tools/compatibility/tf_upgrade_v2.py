# Copyright 2018 The TensorFlow Authors. All Rights Reserved.
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
"""Upgrader for Python scripts from 1.* TensorFlow to 2.0 TensorFlow."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import ast
import functools
import sys

import pasta

from tensorflow.tools.compatibility import ast_edits
from tensorflow.tools.compatibility import renames_v2
from tensorflow.tools.compatibility import reorders_v2

# These pylint warnings are a mistake.
# pylint: disable=g-explicit-bool-comparison,g-bool-id-comparison


class TFAPIChangeSpec(ast_edits.APIChangeSpec):
  """List of maps that describe what changed in the API."""

  def __init__(self):
    # Maps from a function name to a dictionary that describes how to
    # map from an old argument keyword to the new argument keyword.
    # If the new argument is None, it will be removed.
    # Only keyword args are handled, so make sure to also put any function in
    # function_reorders to ensure that all args are made into keywords first.
    self.function_keyword_renames = {
        "tf.string_split": {
            "delimiter": "sep",
        },
        "tf.test.assert_equal_graph_def": {
            "checkpoint_v2": None,
        },
        "tf.nn.embedding_lookup": {
            "validate_indices": None,
        },
        "tf.image.sample_distorted_bounding_box": {
            "seed2": None,
        },
        "tf.gradients": {
            "colocate_gradients_with_ops": None,
        },
        "tf.hessians": {
            "colocate_gradients_with_ops": None,
        },
        "*.minimize": {
            "colocate_gradients_with_ops": None,
        },
        "*.compute_gradients": {
            "colocate_gradients_with_ops": None,
        },
        "tf.cond": {
            "strict": None,
            "fn1": "true_fn",
            "fn2": "false_fn"
        },
        "tf.argmin": {
            "dimension": "axis",
        },
        "tf.argmax": {
            "dimension": "axis",
        },
        "tf.arg_min": {
            "dimension": "axis",
        },
        "tf.arg_max": {
            "dimension": "axis",
        },
        "tf.math.argmin": {
            "dimension": "axis",
        },
        "tf.math.argmax": {
            "dimension": "axis",
        },
        "tf.image.crop_and_resize": {
            "box_ind": "box_indices",
        },
        "tf.image.extract_image_patches": {
            "ksizes": "sizes",
        },
        "tf.image.resize": {
            "align_corners": None,
        },
        "tf.image.resize_images": {
            "align_corners": None,
        },
        "tf.extract_image_patches": {
            "ksizes": "sizes",
        },
        "tf.expand_dims": {
            "dim": "axis",
        },
        "tf.batch_to_space": {
            "block_size": "block_shape",
        },
        "tf.space_to_batch": {
            "block_size": "block_shape",
        },
        "tf.nn.space_to_batch": {
            "block_size": "block_shape",
        },
        "tf.constant": {
            "verify_shape": "verify_shape_is_now_always_true",
        },
        "tf.convert_to_tensor": {
            "preferred_dtype": "dtype_hint"
        },
        "tf.nn.softmax_cross_entropy_with_logits": {
            "dim": "axis",
            "_sentinel": None,
        },
        "tf.nn.softmax_cross_entropy_with_logits_v2": {
            "dim": "axis"
        },
        "tf.linalg.l2_normalize": {
            "dim": "axis",
        },
        "tf.linalg.norm": {
            "keep_dims": "keepdims",
        },
        "tf.norm": {
            "keep_dims": "keepdims",
        },
        "tf.load_file_system_library": {
            "library_filename": "library_location",
        },
        "tf.count_nonzero": {
            "input_tensor": "input",
            "keep_dims": "keepdims",
            "reduction_indices": "axis",
        },
        "tf.math.count_nonzero": {
            "input_tensor": "input",
            "keep_dims": "keepdims",
            "reduction_indices": "axis",
        },
        "tf.nn.erosion2d": {
            "kernel": "filters",
            "rates": "dilations",
        },
        "tf.math.l2_normalize": {
            "dim": "axis",
        },
        "tf.math.log_softmax": {
            "dim": "axis",
        },
        "tf.math.softmax": {
            "dim": "axis"
        },
        "tf.nn.l2_normalize": {
            "dim": "axis",
        },
        "tf.nn.log_softmax": {
            "dim": "axis",
        },
        "tf.nn.moments": {
            "keep_dims": "keepdims",
        },
        "tf.nn.pool": {
            "dilation_rate": "dilations"
        },
        "tf.nn.separable_conv2d": {
            "rate": "dilations"
        },
        "tf.nn.depthwise_conv2d": {
            "rate": "dilations"
        },
        "tf.nn.softmax": {
            "dim": "axis"
        },
        "tf.nn.sufficient_statistics": {
            "keep_dims": "keepdims"
        },
        "tf.debugging.assert_all_finite": {
            "t": "x",
            "msg": "message",
        },
        "tf.sparse.add": {
            "thresh": "threshold",
        },
        "tf.sparse_add": {
            "thresh": "threshold",
        },
        "tf.sparse.concat": {
            "concat_dim": "axis",
            "expand_nonconcat_dim": "expand_nonconcat_dims",
        },
        "tf.sparse_concat": {
            "concat_dim": "axis",
            "expand_nonconcat_dim": "expand_nonconcat_dims",
        },
        "tf.sparse.split": {
            "split_dim": "axis",
        },
        "tf.sparse_split": {
            "split_dim": "axis",
        },
        "tf.sparse.reduce_max": {
            "reduction_axes": "axis",
            "keep_dims": "keepdims",
        },
        "tf.sparse_reduce_max": {
            "reduction_axes": "axis",
            "keep_dims": "keepdims",
        },
        "tf.sparse.reduce_sum": {
            "reduction_axes": "axis",
            "keep_dims": "keepdims",
        },
        "tf.sparse_reduce_sum": {
            "reduction_axes": "axis",
            "keep_dims": "keepdims",
        },
        "tf.nn.max_pool_with_argmax": {
            "Targmax": "output_dtype",
        },
        "tf.nn.max_pool": {
            "value": "input"
        },
        "tf.nn.avg_pool": {
            "value": "input"
        },
        "tf.nn.avg_pool2d": {
            "value": "input"
        },
        "tf.multinomial": {
            "output_dtype": "dtype",
        },
        "tf.random.multinomial": {
            "output_dtype": "dtype",
        },
        "tf.reverse_sequence": {
            "seq_dim": "seq_axis",
            "batch_dim": "batch_axis",
        },
        "tf.nn.batch_norm_with_global_normalization": {
            "t": "input",
            "m": "mean",
            "v": "variance",
        },
        "tf.nn.dilation2d": {
            "filter": "filters",
            "rates": "dilations",
        },
        "tf.nn.conv3d": {
            "filter": "filters"
        },
        "tf.zeros_like": {
            "tensor": "input",
        },
        "tf.ones_like": {
            "tensor": "input",
        },
        "tf.nn.conv2d_transpose": {
            "value": "input",
            "filter": "filters",
        },
        "tf.nn.conv3d_transpose": {
            "value": "input",
            "filter": "filters",
        },
        "tf.nn.convolution": {
            "filter": "filters",
            "dilation_rate": "dilations",
        },
        "tf.gfile.Exists": {
            "filename": "path",
        },
        "tf.gfile.Remove": {
            "filename": "path",
        },
        "tf.gfile.Stat": {
            "filename": "path",
        },
        "tf.gfile.Glob": {
            "filename": "pattern",
        },
        "tf.gfile.MkDir": {
            "dirname": "path",
        },
        "tf.gfile.MakeDirs": {
            "dirname": "path",
        },
        "tf.gfile.DeleteRecursively": {
            "dirname": "path",
        },
        "tf.gfile.IsDirectory": {
            "dirname": "path",
        },
        "tf.gfile.ListDirectory": {
            "dirname": "path",
        },
        "tf.gfile.Copy": {
            "oldpath": "src",
            "newpath": "dst",
        },
        "tf.gfile.Rename": {
            "oldname": "src",
            "newname": "dst",
        },
        "tf.gfile.Walk": {
            "in_order": "topdown",
        },
        "tf.random.stateless_multinomial": {
            "output_dtype": "dtype",
        },
        "tf.string_to_number": {
            "string_tensor": "input",
        },
        "tf.strings.to_number": {
            "string_tensor": "input",
        },
        "tf.string_to_hash_bucket": {
            "string_tensor": "input",
        },
        "tf.strings.to_hash_bucket": {
            "string_tensor": "input",
        },
        "tf.reduce_all": {
            "reduction_indices": "axis",
            "keep_dims": "keepdims",
        },
        "tf.math.reduce_all": {
            "reduction_indices": "axis",
            "keep_dims": "keepdims",
        },
        "tf.reduce_any": {
            "reduction_indices": "axis",
            "keep_dims": "keepdims",
        },
        "tf.math.reduce_any": {
            "reduction_indices": "axis",
            "keep_dims": "keepdims",
        },
        "tf.reduce_min": {
            "reduction_indices": "axis",
            "keep_dims": "keepdims",
        },
        "tf.math.reduce_min": {
            "reduction_indices": "axis",
            "keep_dims": "keepdims",
        },
        "tf.reduce_max": {
            "reduction_indices": "axis",
            "keep_dims": "keepdims",
        },
        "tf.math.reduce_max": {
            "reduction_indices": "axis",
            "keep_dims": "keepdims",
        },
        "tf.reduce_sum": {
            "reduction_indices": "axis",
            "keep_dims": "keepdims",
        },
        "tf.math.reduce_sum": {
            "reduction_indices": "axis",
            "keep_dims": "keepdims",
        },
        "tf.reduce_mean": {
            "reduction_indices": "axis",
            "keep_dims": "keepdims",
        },
        "tf.math.reduce_mean": {
            "reduction_indices": "axis",
            "keep_dims": "keepdims",
        },
        "tf.reduce_prod": {
            "reduction_indices": "axis",
            "keep_dims": "keepdims",
        },
        "tf.math.reduce_prod": {
            "reduction_indices": "axis",
            "keep_dims": "keepdims",
        },
        "tf.reduce_logsumexp": {
            "reduction_indices": "axis",
            "keep_dims": "keepdims",
        },
        "tf.math.reduce_logsumexp": {
            "reduction_indices": "axis",
            "keep_dims": "keepdims",
        },
        "tf.reduce_join": {
            "keep_dims": "keepdims",
            "reduction_indices": "axis"
        },
        "tf.strings.reduce_join": {
            "keep_dims": "keepdims",
            "reduction_indices": "axis"
        },
        "tf.squeeze": {
            "squeeze_dims": "axis",
        },
        "tf.nn.weighted_moments": {
            "keep_dims": "keepdims"
        },
        "tf.nn.conv1d": {
            "value": "input",
            "use_cudnn_on_gpu": None,
        },
        "tf.nn.conv2d": {
            "filter": "filters",
            "use_cudnn_on_gpu": None,
        },
        "tf.nn.conv2d_backprop_input": {
            "use_cudnn_on_gpu": None,
            "input_sizes": "output_shape",
            "out_backprop": "input",
            "filter": "filters",
        },
        "tf.contrib.summary.audio": {
            "tensor": "data",
            "family": None,
        },
        "tf.contrib.summary.histogram": {
            "tensor": "data",
            "family": None,
        },
        "tf.contrib.summary.image": {
            "tensor": "data",
            "bad_color": None,
            "max_images": "max_outputs",
            "family": None,
        },
        "tf.contrib.summary.scalar": {
            "tensor": "data",
            "family": None,
        },
    }

    # pylint: disable=line-too-long
    # Add additional renames not in renames_v2.py here.
    # IMPORTANT: For the renames in here, if you also need to add to
    # function_reorders or function_keyword_renames, use the OLD function name.
    # These renames happen after the arguments have been processed.
    self.manual_symbol_renames = {
        "tf.batch_to_space_nd":
            "tf.batch_to_space",
        "tf.batch_gather":
            "tf.compat.v1.batch_gather",
        "tf.space_to_batch_nd":
            "tf.space_to_batch",
        "tf.nn.space_to_batch":
            "tf.space_to_batch",
        "tf.estimator.inputs":
            "tf.compat.v1.estimator.inputs",
        "tf.extract_image_patches":
            "tf.image.extract_image_patches",
        "tf.gfile.Copy":
            "tf.io.gfile.copy",
        "tf.gfile.DeleteRecursively":
            "tf.io.gfile.rmtree",
        "tf.gfile.Exists":
            "tf.io.gfile.exists",
        "tf.gfile.Glob":
            "tf.io.gfile.glob",
        "tf.gfile.GFile":
            "tf.io.gfile.GFile",
        "tf.gfile.IsDirectory":
            "tf.io.gfile.isdir",
        "tf.gfile.ListDirectory":
            "tf.io.gfile.listdir",
        "tf.gfile.MakeDirs":
            "tf.io.gfile.makedirs",
        "tf.gfile.MkDir":
            "tf.io.gfile.mkdir",
        "tf.gfile.Open":
            "tf.io.gfile.GFile",
        "tf.gfile.Remove":
            "tf.io.gfile.remove",
        "tf.gfile.Rename":
            "tf.io.gfile.rename",
        "tf.gfile.Stat":
            "tf.io.gfile.stat",
        "tf.gfile.Walk":
            "tf.io.gfile.walk",
        "tf.contrib.data.AUTOTUNE":
            "tf.data.experimental.AUTOTUNE",
        "tf.contrib.data.Counter":
            "tf.data.experimental.Counter",
        "tf.contrib.data.CheckpointInputPipelineHook":
            "tf.data.experimental.CheckpointInputPipelineHook",
        "tf.contrib.data.CsvDataset":
            "tf.data.experimental.CsvDataset",
        "tf.contrib.data.Optional":
            "tf.data.experimental.Optional",
        "tf.contrib.data.RandomDataset":
            "tf.data.experimental.RandomDataset",
        "tf.contrib.data.Reducer":
            "tf.data.experimental.Reducer",
        "tf.contrib.data.SqlDataset":
            "tf.data.experimental.SqlDataset",
        "tf.contrib.data.StatsAggregator":
            "tf.data.experimental.StatsAggregator",
        "tf.contrib.data.TFRecordWriter":
            "tf.data.experimental.TFRecordWriter",
        "tf.contrib.data.assert_element_shape":
            "tf.data.experimental.assert_element_shape",
        "tf.contrib.data.batch_and_drop_remainder":
            "tf.compat.v1.contrib.data.batch_and_drop_remainder",
        "tf.contrib.data.bucket_by_sequence_length":
            "tf.data.experimental.bucket_by_sequence_length",
        "tf.contrib.data.choose_from_datasets":
            "tf.data.experimental.choose_from_datasets",
        "tf.contrib.data.copy_to_device":
            "tf.data.experimental.copy_to_device",
        "tf.contrib.data.dense_to_sparse_batch":
            "tf.data.experimental.dense_to_sparse_batch",
        "tf.contrib.data.enumerate_dataset":
            "tf.data.experimental.enumerate_dataset",
        "tf.contrib.data.get_next_as_optional":
            "tf.data.experimental.get_next_as_optional",
        "tf.contrib.data.get_single_element":
            "tf.data.experimental.get_single_element",
        "tf.contrib.data.group_by_reducer":
            "tf.data.experimental.group_by_reducer",
        "tf.contrib.data.group_by_window":
            "tf.data.experimental.group_by_window",
        "tf.contrib.data.ignore_errors":
            "tf.data.experimental.ignore_errors",
        "tf.contrib.data.latency_stats":
            "tf.data.experimental.latency_stats",
        "tf.contrib.data.make_batched_features_dataset":
            "tf.data.experimental.make_batched_features_dataset",
        "tf.contrib.data.make_csv_dataset":
            "tf.data.experimental.make_csv_dataset",
        "tf.contrib.data.make_saveable_from_iterator":
            "tf.data.experimental.make_saveable_from_iterator",
        "tf.contrib.data.map_and_batch":
            "tf.data.experimental.map_and_batch",
        "tf.contrib.data.padded_batch_and_drop_remainder":
            "tf.compat.v1.contrib.data.padded_batch_and_drop_remainder",
        "tf.contrib.data.parallel_interleave":
            "tf.data.experimental.parallel_interleave",
        "tf.contrib.data.parse_example_dataset":
            "tf.data.experimental.parse_example_dataset",
        "tf.contrib.data.prefetch_to_device":
            "tf.data.experimental.prefetch_to_device",
        "tf.contrib.data.read_batch_features":
            "tf.compat.v1.contrib.data.read_batch_features",
        "tf.contrib.data.reduce_dataset":
            "tf.compat.v1.contrib.data.reduce_dataset",
        "tf.contrib.data.rejection_resample":
            "tf.data.experimental.rejection_resample",
        "tf.contrib.data.sample_from_datasets":
            "tf.data.experimental.sample_from_datasets",
        "tf.contrib.data.scan":
            "tf.data.experimental.scan",
        "tf.contrib.data.set_stats_aggregator":
            "tf.data.experimental.set_stats_aggregator",
        "tf.contrib.data.shuffle_and_repeat":
            "tf.data.experimental.shuffle_and_repeat",
        "tf.contrib.data.sliding_window_batch":
            "tf.compat.v1.contrib.data.sliding_window_batch",
        "tf.contrib.data.sloppy_interleave":
            "tf.compat.v1.contrib.data.sloppy_interleave",
        "tf.contrib.data.unbatch":
            "tf.data.experimental.unbatch",
        "tf.contrib.data.unique":
            "tf.data.experimental.unique",
        "tf.contrib.estimator.make_early_stopping_hook":
            "tf.estimator.experimental.make_early_stopping_hook",
        "tf.contrib.estimator.stop_if_higher_hook":
            "tf.estimator.experimental.stop_if_higher_hook",
        "tf.contrib.estimator.stop_if_lower_hook":
            "tf.estimator.experimental.stop_if_lower_hook",
        "tf.contrib.estimator.stop_if_no_decrease_hook":
            "tf.estimator.experimental.stop_if_no_decrease_hook",
        "tf.contrib.estimator.stop_if_no_increase_hook":
            "tf.estimator.experimental.stop_if_no_increase_hook",
        "tf.contrib.framework.CriticalSection":
            "tf.CriticalSection",
        "tf.contrib.framework.is_tensor":
            "tf.is_tensor",
        "tf.contrib.framework.nest.assert_same_structure":
            "tf.nest.assert_same_structure",
        "tf.contrib.framework.nest.flatten":
            "tf.nest.flatten",
        "tf.contrib.framework.nest.is_sequence":
            "tf.nest.is_nested",
        "tf.contrib.framework.nest.map_structure":
            "tf.nest.map_structure",
        "tf.contrib.framework.nest.pack_sequence_as":
            "tf.nest.pack_sequence_as",
        "tf.contrib.util.constant_value":
            "tf.get_static_value",
        "tf.contrib.saved_model.load_keras_model":
            "tf.keras.experimental.load_from_saved_model",
        "tf.contrib.saved_model.save_keras_model":
            "tf.keras.experimental.export_saved_model",
        "tf.contrib.rnn.RNNCell":
            "tf.compat.v1.nn.rnn_cell.RNNCell",
        "tf.contrib.rnn.LSTMStateTuple":
            "tf.nn.rnn_cell.LSTMStateTuple",
        "tf.contrib.rnn.BasicLSTMCell":
            "tf.compat.v1.nn.rnn_cell.BasicLSTMCell",
        "tf.contrib.rnn.BasicRNNCell":
            "tf.compat.v1.nn.rnn_cell.BasicRNNCell",
        "tf.contrib.rnn.GRUCell":
            "tf.compat.v1.nn.rnn_cell.GRUCell",
        "tf.contrib.rnn.LSTMCell":
            "tf.compat.v1.nn.rnn_cell.LSTMCell",
        "tf.contrib.rnn.MultiRNNCell":
            "tf.compat.v1.nn.rnn_cell.MultiRNNCell",
        "tf.contrib.framework.sort":
            "tf.sort",
        "tf.contrib.framework.argsort":
            "tf.argsort",
        "tf.contrib.summary.audio":
            "tf.compat.v2.summary.audio",
        "tf.contrib.summary.histogram":
            "tf.compat.v2.summary.histogram",
        "tf.contrib.summary.image":
            "tf.compat.v2.summary.image",
        "tf.contrib.summary.initialize":
            "tf.compat.v1.summary.initialize",
        "tf.contrib.summary.scalar":
            "tf.compat.v2.summary.scalar",
        "tf.count_nonzero":
            "tf.math.count_nonzero",
        "tf.manip.batch_to_space_nd":
            "tf.batch_to_space",
        "tf.quantize_v2":
            "tf.quantization.quantize",
        "tf.sparse_add":
            "tf.sparse.add",
        "tf.sparse_concat":
            "tf.sparse.concat",
        "tf.sparse_split":
            "tf.sparse.split",
        "tf.sparse_matmul":
            "tf.linalg.matmul",
        "tf.sparse_reduce_sum":
            "tf.sparse.reduce_sum",
        "tf.sparse_reduce_max":
            "tf.sparse.reduce_max",
        "tf.random.stateless_multinomial":
            "tf.random.stateless_categorical",
        "tf.substr":
            "tf.strings.substr",
        "tf.string_split":
            "tf.strings.split",
        "tf.string_to_hash_bucket":
            "tf.strings.to_hash_bucket",
        "tf.string_to_number":
            "tf.strings.to_number",
        "tf.multinomial":
            "tf.random.categorical",
        "tf.random.multinomial":
            "tf.random.categorical",
        "tf.reduce_join":
            "tf.strings.reduce_join",
        "tf.load_file_system_library":
            "tf.load_library",
        "tf.pywrap_tensorflow":
            "tf.compat.v1.pywrap_tensorflow",
        "tf.bincount":
            "tf.math.bincount",
        "tf.confusion_matrix":
            "tf.math.confusion_matrix",
        "tf.train.confusion_matrix":
            "tf.math.confusion_matrix",
        "tf.decode_csv":
            "tf.io.decode_csv",
        "tf.data.Iterator":
            "tf.compat.v1.data.Iterator",
        "tf.parse_example":
            "tf.io.parse_example",
        "tf.parse_single_example":
            "tf.io.parse_single_example",
        "tf.nn.fused_batch_norm":
            "tf.compat.v1.nn.fused_batch_norm",
        "tf.nn.softmax_cross_entropy_with_logits_v2":
            "tf.nn.softmax_cross_entropy_with_logits",
        "tf.nn.sigmoid_cross_entropy_with_logits":
            "tf.nn.sigmoid_cross_entropy_with_logits",
        "tf.losses.Reduction.MEAN":
            "tf.compat.v1.losses.Reduction.MEAN",
        "tf.losses.Reduction.SUM_BY_NONZERO_WEIGHTS":
            "tf.compat.v1.losses.Reduction.SUM_BY_NONZERO_WEIGHTS",
        "tf.losses.Reduction.SUM_OVER_NONZERO_WEIGHTS":
            "tf.compat.v1.losses.Reduction.SUM_OVER_NONZERO_WEIGHTS",
        "tf.lite.constants.FLOAT":
            "tf.float32",
        "tf.lite.constants.INT32":
            "tf.int32",
        "tf.lite.constants.INT64":
            "tf.int64",
        "tf.lite.constants.STRING":
            "tf.string",
        "tf.lite.constants.QUANTIZED_UINT8":
            "tf.uint8",
        "tf.arg_max":
            "tf.argmax",
        "tf.arg_min":
            "tf.argmin",
        # tf.nn.ctc_loss is still available in 2.0 but behavior
        # changed significantly.
        "tf.nn.ctc_loss":
            "tf.compat.v1.nn.ctc_loss",
        # tf.saved_model.load in 1.x has no equivalent in 2.x, but there is a
        # symbol with the same name.
        "tf.saved_model.load":
            "tf.compat.v1.saved_model.load",
        "tf.saved_model.load_v2":
            "tf.compat.v2.saved_model.load",
        "tf.image.resize_images":
            "tf.image.resize",
        "tf.random_poisson":
            "tf.random.poisson",
        "tf.debugging.assert_greater":
            "tf.compat.v1.debugging.assert_greater",
        "tf.debugging.assert_greater_equal":
            "tf.compat.v1.debugging.assert_greater_equal",
        "tf.debugging.assert_integer":
            "tf.compat.v1.debugging.assert_integer",
        "tf.debugging.assert_less":
            "tf.compat.v1.debugging.assert_less",
        "tf.debugging.assert_less_equal":
            "tf.compat.v1.debugging.assert_less_equal",
        "tf.debugging.assert_near":
            "tf.compat.v1.debugging.assert_near",
        "tf.debugging.assert_negative":
            "tf.compat.v1.debugging.assert_negative",
        "tf.debugging.assert_non_negative":
            "tf.compat.v1.debugging.assert_non_negative",
        "tf.debugging.assert_non_positive":
            "tf.compat.v1.debugging.assert_non_positive",
        "tf.debugging.assert_none_equal":
            "tf.compat.v1.debugging.assert_none_equal",
        "tf.debugging.assert_type":
            "tf.compat.v1.debugging.assert_type",
        "tf.debugging.assert_positive":
            "tf.compat.v1.debugging.assert_positive",
        "tf.debugging.assert_equal":
            "tf.compat.v1.debugging.assert_equal",
        "tf.debugging.assert_scalar":
            "tf.compat.v1.debugging.assert_scalar",
        "tf.assert_equal":
            "tf.compat.v1.assert_equal",
        "tf.assert_less":
            "tf.compat.v1.assert_less",
        "tf.assert_greater":
            "tf.compat.v1.assert_greater",
        "tf.debugging.assert_rank":
            "tf.compat.v1.debugging.assert_rank",
        "tf.debugging.assert_rank_at_least":
            "tf.compat.v1.debugging.assert_rank_at_least",
        "tf.debugging.assert_rank_in":
            "tf.compat.v1.debugging.assert_rank_in",
        "tf.assert_rank":
            "tf.compat.v1.assert_rank",
        "tf.nn.max_pool":
            "tf.nn.max_pool2d",
        "tf.nn.avg_pool":
            "tf.nn.avg_pool2d",
        "tf.keras.initializers.zeros":
            "tf.compat.v1.keras.initializers.zeros",
        "tf.keras.initializers.Zeros":
            "tf.compat.v1.keras.initializers.Zeros",
        "tf.keras.initializers.ones":
            "tf.compat.v1.keras.initializers.ones",
        "tf.keras.initializers.Ones":
            "tf.compat.v1.keras.initializers.Ones",
        "tf.keras.initializers.constant":
            "tf.compat.v1.keras.initializers.constant",
        "tf.keras.initializers.Constant":
            "tf.compat.v1.keras.initializers.Constant",
        "tf.keras.initializers.VarianceScaling":
            "tf.compat.v1.keras.initializers.VarianceScaling",
        "tf.keras.initializers.Orthogonal":
            "tf.compat.v1.keras.initializers.Orthogonal",
        "tf.keras.initializers.orthogonal":
            "tf.compat.v1.keras.initializers.orthogonal",
        "tf.keras.initializers.Identity":
            "tf.compat.v1.keras.initializers.Identity",
        "tf.keras.initializers.identity":
            "tf.compat.v1.keras.initializers.identity",
        "tf.keras.initializers.glorot_uniform":
            "tf.compat.v1.keras.initializers.glorot_uniform",
        "tf.keras.initializers.glorot_normal":
            "tf.compat.v1.keras.initializers.glorot_normal",
        "tf.keras.initializers.lecun_normal":
            "tf.compat.v1.keras.initializers.lecun_normal",
        "tf.keras.initializers.lecun_uniform":
            "tf.compat.v1.keras.initializers.lecun_uniform",
        "tf.keras.initializers.he_normal":
            "tf.compat.v1.keras.initializers.he_normal",
        "tf.keras.initializers.he_uniform":
            "tf.compat.v1.keras.initializers.he_uniform",
        "tf.keras.initializers.TruncatedNormal":
            "tf.compat.v1.keras.initializers.TruncatedNormal",
        "tf.keras.initializers.truncated_normal":
            "tf.compat.v1.keras.initializers.truncated_normal",
        "tf.keras.initializers.RandomUniform":
            "tf.compat.v1.keras.initializers.RandomUniform",
        "tf.keras.initializers.uniform":
            "tf.compat.v1.keras.initializers.uniform",
        "tf.keras.initializers.random_uniform":
            "tf.compat.v1.keras.initializers.random_uniform",
        "tf.keras.initializers.RandomNormal":
            "tf.compat.v1.keras.initializers.RandomNormal",
        "tf.keras.initializers.normal":
            "tf.compat.v1.keras.initializers.normal",
        "tf.keras.initializers.random_normal":
            "tf.compat.v1.keras.initializers.random_normal",
        "tf.zeros_initializer":
            "tf.compat.v1.zeros_initializer",
        "tf.initializers.zeros":
            "tf.compat.v1.initializers.zeros",
        "tf.ones_initializer":
            "tf.compat.v1.ones_initializer",
        "tf.initializers.ones":
            "tf.compat.v1.initializers.ones",
        "tf.constant_initializer":
            "tf.compat.v1.constant_initializer",
        "tf.initializers.constant":
            "tf.compat.v1.initializers.constant",
        "tf.random_uniform_initializer":
            "tf.compat.v1.random_uniform_initializer",
        "tf.initializers.random_uniform":
            "tf.compat.v1.initializers.random_uniform",
        "tf.random_normal_initializer":
            "tf.compat.v1.random_normal_initializer",
        "tf.initializers.random_normal":
            "tf.compat.v1.initializers.random_normal",
        "tf.truncated_normal_initializer":
            "tf.compat.v1.truncated_normal_initializer",
        "tf.initializers.truncated_normal":
            "tf.compat.v1.initializers.truncated_normal",
        "tf.variance_scaling_initializer":
            "tf.compat.v1.variance_scaling_initializer",
        "tf.initializers.variance_scaling":
            "tf.compat.v1.initializers.variance_scaling",
        "tf.orthogonal_initializer":
            "tf.compat.v1.orthogonal_initializer",
        "tf.initializers.orthogonal":
            "tf.compat.v1.initializers.orthogonal",
        "tf.glorot_uniform_initializer":
            "tf.compat.v1.glorot_uniform_initializer",
        "tf.initializers.glorot_uniform":
            "tf.compat.v1.initializers.glorot_uniform",
        "tf.glorot_normal_initializer":
            "tf.compat.v1.glorot_normal_initializer",
        "tf.initializers.glorot_normal":
            "tf.compat.v1.initializers.glorot_normal",
        "tf.initializers.identity":
            "tf.compat.v1.initializers.identity",
        "tf.initializers.lecun_normal":
            "tf.compat.v1.initializers.lecun_normal",
        "tf.initializers.lecun_uniform":
            "tf.compat.v1.initializers.lecun_uniform",
        "tf.initializers.he_normal":
            "tf.compat.v1.initializers.he_normal",
        "tf.initializers.he_uniform":
            "tf.compat.v1.initializers.he_uniform",
        "tf.data.experimental.map_and_batch_with_legacy_function":
            "tf.compat.v1.data.experimental.map_and_batch_with_legacy_function",
        "tf.nn.conv2d_backprop_input":
            "tf.nn.conv2d_transpose",
        "tf.test.compute_gradient":
            "tf.compat.v1.test.compute_gradient",
    }
    # pylint: enable=line-too-long

    # Mapping from function to the new name of the function
    self.symbol_renames = renames_v2.renames
    self.symbol_renames.update(self.manual_symbol_renames)
    self.symbol_renames = {
        name: new_name
        for name, new_name in self.symbol_renames.items()
    }

    # Variables that should be changed to functions.
    self.change_to_function = {}

    # pylint: disable=line-too-long
    # This list should just contain names of functions that had
    # their arguments reordered. After adding a function name to the list
    # run the following to update reorders_v2.py:
    # bazel build tensorflow/tools/compatibility/update:generate_v2_reorders_map
    # bazel-bin/tensorflow/tools/compatibility/update/generate_v2_reorders_map
    # pylint: enable=line-too-long
    self.reordered_function_names = {
        "tf.io.serialize_sparse",
        "tf.io.serialize_many_sparse",
        "tf.argmax",
        "tf.argmin",
        "tf.batch_to_space",
        "tf.cond",
        "tf.nn.space_to_batch",
        "tf.boolean_mask",
        "tf.convert_to_tensor",
        "tf.nn.conv1d",
        "tf.nn.conv2d",
        "tf.nn.conv2d_backprop_input",
        "tf.nn.ctc_beam_search_decoder",
        "tf.nn.moments",
        "tf.nn.convolution",
        "tf.nn.crelu",
        "tf.nn.weighted_moments",
        "tf.nn.pool",
        "tf.nn.separable_conv2d",
        "tf.nn.depthwise_conv2d",
        "tf.multinomial",
        "tf.random.multinomial",
        "tf.pad",
        "tf.quantize_v2",
        "tf.feature_column.categorical_column_with_vocabulary_file",
        "tf.shape",
        "tf.size",
        "tf.string_split",
        "tf.random.poisson",
        "tf.sparse.add",
        "tf.sparse_add",
        "tf.sparse.concat",
        "tf.sparse_concat",
        "tf.sparse.segment_mean",
        "tf.sparse.segment_sqrt_n",
        "tf.sparse.segment_sum",
        "tf.sparse_matmul",
        "tf.sparse.reduce_max",
        "tf.sparse_reduce_max",
        "tf.io.decode_csv",
        "tf.strings.length",
        "tf.strings.reduce_join",
        "tf.strings.substr",
        "tf.substr",
        "tf.transpose",
        "tf.tuple",
        "tf.parse_example",
        "tf.parse_single_example",
        "tf.io.parse_example",
        "tf.io.parse_single_example",
        "tf.while_loop",
        "tf.reduce_all",
        "tf.math.reduce_all",
        "tf.reduce_any",
        "tf.math.reduce_any",
        "tf.reduce_min",
        "tf.math.reduce_min",
        "tf.reduce_max",
        "tf.math.reduce_max",
        "tf.reduce_sum",
        "tf.math.reduce_sum",
        "tf.reduce_mean",
        "tf.math.reduce_mean",
        "tf.reduce_prod",
        "tf.math.reduce_prod",
        "tf.reduce_logsumexp",
        "tf.math.reduce_logsumexp",
        "tf.reduce_join",
        "tf.confusion_matrix",
        "tf.math.confusion_matrix",
        "tf.math.in_top_k",
        "tf.nn.depth_to_space",
        "tf.nn.embedding_lookup",
        "tf.nn.embedding_lookup_sparse",
        "tf.nn.in_top_k",
        "tf.nn.space_to_depth",
        "tf.test.assert_equal_graph_def",
        "tf.linalg.norm",
        "tf.norm",
        "tf.reverse_sequence",
        "tf.sparse_split",
        # tf.nn.softmax_cross_entropy_with_logits *must* be called with
        # keyword arguments. Add keyword arguments in rare case when they
        # are not specified.
        "tf.nn.softmax_cross_entropy_with_logits",
        "tf.nn.fractional_avg_pool",
        "tf.nn.fractional_max_pool",
        "tf.image.sample_distorted_bounding_box",
        "tf.gradients",
        "tf.hessians",
        "tf.nn.max_pool",
        "tf.nn.avg_pool",
        "tf.estimator.LinearClassifier",
        "tf.estimator.LinearRegressor",
        "tf.estimator.DNNLinearCombinedClassifier",
        "tf.estimator.DNNLinearCombinedRegressor",
        "tf.estimator.DNNRegressor",
        "tf.estimator.DNNClassifier",
        "tf.estimator.BaselineClassifier",
        "tf.estimator.BaselineRegressor",
        "tf.initializers.uniform_unit_scaling",
        "tf.uniform_unit_scaling_initializer",
    }

    # Manual mapping of function names to be reordered to their list of argument
    # names, in order. Only use this if argument names cannot be autodetected,
    # e.g. if the functions are in contrib.
    self.manual_function_reorders = {
        "tf.contrib.summary.audio": [
            "name", "tensor", "sample_rate", "max_outputs", "family", "step"],
        "tf.contrib.summary.histogram": [
            "name", "tensor", "family", "step"],
        "tf.contrib.summary.image": [
            "name", "tensor", "bad_color", "max_images", "family", "step"],
        "tf.contrib.summary.scalar": [
            "name", "tensor", "family", "step"],
    }
    # Functions that were reordered should be changed to the new keyword args
    # for safety, if positional arguments are used. If you have reversed the
    # positional arguments yourself, this could do the wrong thing.
    self.function_reorders = dict(reorders_v2.reorders)
    self.function_reorders.update(self.manual_function_reorders)

    contrib_warning = (
        ast_edits.ERROR,
        "<function name> cannot be converted automatically. tf.contrib will not"
        " be distributed with TensorFlow 2.0, please consider an alternative in"
        " non-contrib TensorFlow, a community-maintained repository, or fork "
        "the required code."
    )

    flags_warning = (
        ast_edits.ERROR,
        "tf.flags has been removed, please use the argparse or absl"
        " modules if you need command line parsing.")

    decay_function_comment = (
        ast_edits.INFO,
        "To use learning rate decay schedules with TensorFlow 2.0, switch to "
        "the schedules in `tf.keras.optimizers.schedules`.\n"
    )

    assert_return_type_comment = (
        ast_edits.INFO,
        "<function name> has been changed to return None, the "
        "data argument has been removed, and arguments have been reordered."
        "\nThe calls have been converted to compat.v1 for safety (even though "
        " they may already have been correct)."
    )

    assert_rank_comment = (
        ast_edits.INFO,
        "<function name> has been changed to return None, and"
        " the data and summarize arguments have been removed."
        "\nThe calls have been converted to compat.v1 for safety (even though "
        " they may already have been correct)."
    )

    initializers_no_dtype_comment = (
        ast_edits.INFO,
        "Initializers no longer have the "
        "dtype argument in the constructor or partition_info argument in the "
        "__call__ method.\nThe calls have been converted to compat.v1 for"
        "safety (even though they may already have been correct).")

    metrics_comment = (
        ast_edits.INFO,
        "tf.metrics have been replaced with object oriented versions in"
        " TF 2.0 and after. The metric function calls have been converted to "
        "compat.v1 for backward compatibility. Please update these calls to "
        "the TF 2.0 versions.")

    losses_comment = (
        ast_edits.INFO,
        "tf.losses have been replaced with object oriented versions in"
        " TF 2.0 and after. The loss function calls have been converted to "
        "compat.v1 for backward compatibility. Please update these calls to "
        "the TF 2.0 versions.")

    # This could be done with a _rename_if_arg_not_found_transformer
    deprecate_partition_strategy_comment = (
        ast_edits.WARNING,
        "`partition_strategy` has been removed from <function name>. "
        " The 'div' strategy will be used by default.")

    # make change instead
    uniform_unit_scaling_initializer_comment = (
        ast_edits.ERROR,
        "uniform_unit_scaling_initializer has been removed. Please use"
        " tf.initializers.variance_scaling instead with distribution=uniform "
        "to get equivalent behaviour.")

    # Make change instead (issue warning about strip_...)
    export_saved_model_renamed = (
        ast_edits.ERROR,
        "(Manual edit required) Please rename the method export_savedmodel() "
        "to export_saved_model(). Two things to note:\n\t(1) The argument "
        "strip_default_attributes has been removed. The function will always "
        "strip the default attributes from ops. If this breaks your code, "
        "please switch to tf.compat.v1.estimator.Estimator.\n\t(2) This change "
        "only effects core estimator. If you are using "
        "tf.contrib.learn.Estimator, please switch to using core estimator.")

    # TODO(b/124529441): if possible eliminate need for manual checking.
    contrib_summary_comment = (
        ast_edits.WARNING,
        "(Manual check required) tf.contrib.summary.* functions have been "
        "migrated best-effort to tf.compat.v2.summary.* equivalents where "
        "possible, but the resulting code may not always work. Please check "
        "manually; you can report migration failures on b/124529441.")

    # Function warnings. <function name> placeholder inside warnings will be
    # replaced by function name.
    # You can use *. to add items which do not check the FQN, and apply to e.g.,
    # methods.
    self.function_warnings = {
        "*.export_savedmodel":
            export_saved_model_renamed,
        "tf.assert_equal":
            assert_return_type_comment,
        "tf.assert_none_equal":
            assert_return_type_comment,
        "tf.assert_negative":
            assert_return_type_comment,
        "tf.assert_positive":
            assert_return_type_comment,
        "tf.assert_non_negative":
            assert_return_type_comment,
        "tf.assert_non_positive":
            assert_return_type_comment,
        "tf.assert_near":
            assert_return_type_comment,
        "tf.assert_less":
            assert_return_type_comment,
        "tf.assert_less_equal":
            assert_return_type_comment,
        "tf.assert_greater":
            assert_return_type_comment,
        "tf.assert_greater_equal":
            assert_return_type_comment,
        "tf.assert_integer":
            assert_return_type_comment,
        "tf.assert_type":
            assert_return_type_comment,
        "tf.assert_scalar":
            assert_return_type_comment,
        "tf.assert_rank":
            assert_rank_comment,
        "tf.assert_rank_at_least":
            assert_rank_comment,
        "tf.assert_rank_in":
            assert_rank_comment,
        "tf.contrib.summary.audio":
            contrib_summary_comment,
        "tf.contrib.summary.histogram":
            contrib_summary_comment,
        "tf.contrib.summary.image":
            contrib_summary_comment,
        "tf.contrib.summary.scalar":
            contrib_summary_comment,
        "tf.debugging.assert_equal":
            assert_return_type_comment,
        "tf.debugging.assert_greater":
            assert_return_type_comment,
        "tf.debugging.assert_greater_equal":
            assert_return_type_comment,
        "tf.debugging.assert_integer":
            assert_return_type_comment,
        "tf.debugging.assert_less":
            assert_return_type_comment,
        "tf.debugging.assert_less_equal":
            assert_return_type_comment,
        "tf.debugging.assert_near":
            assert_return_type_comment,
        "tf.debugging.assert_negative":
            assert_return_type_comment,
        "tf.debugging.assert_non_negative":
            assert_return_type_comment,
        "tf.debugging.assert_non_positive":
            assert_return_type_comment,
        "tf.debugging.assert_none_equal":
            assert_return_type_comment,
        "tf.debugging.assert_positive":
            assert_return_type_comment,
        "tf.debugging.assert_type":
            assert_return_type_comment,
        "tf.debugging.assert_scalar":
            assert_return_type_comment,
        "tf.debugging.assert_rank":
            assert_rank_comment,
        "tf.debugging.assert_rank_at_least":
            assert_rank_comment,
        "tf.debugging.assert_rank_in":
            assert_rank_comment,
        "tf.train.exponential_decay":
            decay_function_comment,
        "tf.train.piecewise_constant_decay":
            decay_function_comment,
        "tf.train.polynomial_decay":
            decay_function_comment,
        "tf.train.natural_exp_decay":
            decay_function_comment,
        "tf.train.inverse_time_decay":
            decay_function_comment,
        "tf.train.cosine_decay":
            decay_function_comment,
        "tf.train.cosine_decay_restarts":
            decay_function_comment,
        "tf.train.linear_cosine_decay":
            decay_function_comment,
        "tf.train.noisy_linear_cosine_decay":
            decay_function_comment,
        "tf.nn.embedding_lookup":
            deprecate_partition_strategy_comment,
        "tf.nn.embedding_lookup_sparse":
            deprecate_partition_strategy_comment,
        "tf.nn.nce_loss":
            deprecate_partition_strategy_comment,
        "tf.nn.safe_embedding_lookup_sparse":
            deprecate_partition_strategy_comment,
        "tf.nn.sampled_softmax_loss":
            deprecate_partition_strategy_comment,
        "tf.keras.initializers.Zeros":
            initializers_no_dtype_comment,
        "tf.keras.initializers.zeros":
            initializers_no_dtype_comment,
        "tf.keras.initializers.Ones":
            initializers_no_dtype_comment,
        "tf.keras.initializers.ones":
            initializers_no_dtype_comment,
        "tf.keras.initializers.Constant":
            initializers_no_dtype_comment,
        "tf.keras.initializers.constant":
            initializers_no_dtype_comment,
        "tf.keras.initializers.VarianceScaling":
            initializers_no_dtype_comment,
        "tf.keras.initializers.Orthogonal":
            initializers_no_dtype_comment,
        "tf.keras.initializers.orthogonal":
            initializers_no_dtype_comment,
        "tf.keras.initializers.Identity":
            initializers_no_dtype_comment,
        "tf.keras.initializers.identity":
            initializers_no_dtype_comment,
        "tf.keras.initializers.glorot_uniform":
            initializers_no_dtype_comment,
        "tf.keras.initializers.glorot_normal":
            initializers_no_dtype_comment,
        "tf.initializers.zeros":
            initializers_no_dtype_comment,
        "tf.zeros_initializer":
            initializers_no_dtype_comment,
        "tf.initializers.ones":
            initializers_no_dtype_comment,
        "tf.ones_initializer":
            initializers_no_dtype_comment,
        "tf.initializers.constant":
            initializers_no_dtype_comment,
        "tf.constant_initializer":
            initializers_no_dtype_comment,
        "tf.initializers.random_uniform":
            initializers_no_dtype_comment,
        "tf.random_uniform_initializer":
            initializers_no_dtype_comment,
        "tf.initializers.random_normal":
            initializers_no_dtype_comment,
        "tf.random_normal_initializer":
            initializers_no_dtype_comment,
        "tf.initializers.truncated_normal":
            initializers_no_dtype_comment,
        "tf.truncated_normal_initializer":
            initializers_no_dtype_comment,
        "tf.initializers.variance_scaling":
            initializers_no_dtype_comment,
        "tf.variance_scaling_initializer":
            initializers_no_dtype_comment,
        "tf.initializers.orthogonal":
            initializers_no_dtype_comment,
        "tf.orthogonal_initializer":
            initializers_no_dtype_comment,
        "tf.initializers.identity":
            initializers_no_dtype_comment,
        "tf.glorot_uniform_initializer":
            initializers_no_dtype_comment,
        "tf.initializers.glorot_uniform":
            initializers_no_dtype_comment,
        "tf.glorot_normal_initializer":
            initializers_no_dtype_comment,
        "tf.initializers.glorot_normal":
            initializers_no_dtype_comment,
        "tf.losses.absolute_difference":
            losses_comment,
        "tf.losses.add_loss":
            losses_comment,
        "tf.losses.compute_weighted_loss":
            losses_comment,
        "tf.losses.cosine_distance":
            losses_comment,
        "tf.losses.get_losses":
            losses_comment,
        "tf.losses.get_regularization_loss":
            losses_comment,
        "tf.losses.get_regularization_losses":
            losses_comment,
        "tf.losses.get_total_loss":
            losses_comment,
        "tf.losses.hinge_loss":
            losses_comment,
        "tf.losses.huber_loss":
            losses_comment,
        "tf.losses.log_loss":
            losses_comment,
        "tf.losses.mean_pairwise_squared_error":
            losses_comment,
        "tf.losses.mean_squared_error":
            losses_comment,
        "tf.losses.sigmoid_cross_entropy":
            losses_comment,
        "tf.losses.softmax_cross_entropy":
            losses_comment,
        "tf.losses.sparse_softmax_cross_entropy":
            losses_comment,
        "tf.metrics.accuracy":
            metrics_comment,
        "tf.metrics.auc":
            metrics_comment,
        "tf.metrics.average_precision_at_k":
            metrics_comment,
        "tf.metrics.false_negatives":
            metrics_comment,
        "tf.metrics.false_negatives_at_thresholds":
            metrics_comment,
        "tf.metrics.false_positives":
            metrics_comment,
        "tf.metrics.false_positives_at_thresholds":
            metrics_comment,
        "tf.metrics.mean":
            metrics_comment,
        "tf.metrics.mean_absolute_error":
            metrics_comment,
        "tf.metrics.mean_cosine_distance":
            metrics_comment,
        "tf.metrics.mean_iou":
            metrics_comment,
        "tf.metrics.mean_per_class_accuracy":
            metrics_comment,
        "tf.metrics.mean_relative_error":
            metrics_comment,
        "tf.metrics.mean_squared_error":
            metrics_comment,
        "tf.metrics.mean_tensor":
            metrics_comment,
        "tf.metrics.percentage_below":
            metrics_comment,
        "tf.metrics.precision":
            metrics_comment,
        "tf.metrics.precision_at_k":
            metrics_comment,
        "tf.metrics.precision_at_thresholds":
            metrics_comment,
        "tf.metrics.precision_at_top_k":
            metrics_comment,
        "tf.metrics.recall":
            metrics_comment,
        "tf.metrics.recall_at_k":
            metrics_comment,
        "tf.metrics.recall_at_thresholds":
            metrics_comment,
        "tf.metrics.recall_at_top_k":
            metrics_comment,
        "tf.metrics.root_mean_squared_error":
            metrics_comment,
        "tf.metrics.sensitivity_at_specificity":
            metrics_comment,
        "tf.metrics.sparse_average_precision_at_k":
            metrics_comment,
        "tf.metrics.sparse_precision_at_k":
            metrics_comment,
        "tf.metrics.specificity_at_sensitivity":
            metrics_comment,
        "tf.metrics.true_negatives":
            metrics_comment,
        "tf.metrics.true_negatives_at_thresholds":
            metrics_comment,
        "tf.metrics.true_positives":
            metrics_comment,
        "tf.metrics.true_positives_at_thresholds":
            metrics_comment,
        "tf.get_variable":
            (ast_edits.WARNING,
             "<function name> returns ResourceVariables by default in 2.0, "
             "which have well-defined semantics and are stricter about shapes. "
             "You can disable this behavior by passing use_resource=False, or "
             "by calling tf.compat.v1.disable_resource_variables().")
    }

    # Warnings that are emitted only if a specific arg is found.
    self.function_arg_warnings = {
        "tf.nn.conv1d": {
            ("use_cudnn_on_gpu", 4): (
                ast_edits.WARNING,
                "use_cudnn_on_gpu has been removed, behavior is now equivalent"
                "to setting it to True."),
        },
        "tf.nn.conv2d": {
            ("use_cudnn_on_gpu", 4): (
                ast_edits.WARNING,
                "use_cudnn_on_gpu has been removed, behavior is now equivalent"
                "to setting it to True."),
        },
        "tf.nn.conv2d_backprop_filter": {
            ("use_cudnn_on_gpu", 5): (
                ast_edits.WARNING,
                "use_cudnn_on_gpu has been removed, behavior is now equivalent"
                "to setting it to True."),
        },
        "tf.nn.conv2d_backprop_input": {
            ("use_cudnn_on_gpu", 5): (
                ast_edits.WARNING,
                "use_cudnn_on_gpu has been removed, behavior is now equivalent"
                "to setting it to True."),
        },
        "tf.gradients": {
            ("colocate_gradients_with_ops", 4): (
                ast_edits.INFO,
                "tf.gradients no longer takes "
                "'colocate_gradients_with_ops' argument, it behaves as if it "
                "was set to True."),
        },
        "*.minimize": {
            ("colocate_gradients_with_ops", 5): (
                ast_edits.INFO,
                "Optimizer.minimize no longer takes "
                "'colocate_gradients_with_ops' argument, it behaves as if it "
                "was set to True."),
        },
        "*.compute_gradients": {
            ("colocate_gradients_with_ops", 4): (
                ast_edits.INFO,
                "Optimizer.compute_gradients no "
                "longer takes 'colocate_gradients_with_ops' argument, it "
                "behaves as if it was set to True."),
        },
        "tf.cond": {
            ("strict", 3): (
                ast_edits.WARNING,
                "tf.cond no longer takes 'strict' argument, it behaves as "
                "if was set to True.")
        },
        "tf.contrib.summary.audio": {
            ("family", 4): (
                ast_edits.WARNING,
                "tf.contrib.summary.* functions no longer take the 'family' "
                "argument; instead name scoping should be used. This call site "
                "specifies a family argument so it cannot be converted safely.")
        },
        "tf.contrib.summary.histogram": {
            ("family", 2): (
                ast_edits.WARNING,
                "tf.contrib.summary.* functions no longer take the 'family' "
                "argument; instead name scoping should be used. This call site "
                "specifies a family argument so it cannot be converted safely.")
        },
        "tf.contrib.summary.image": {
            ("bad_color", 2): (
                ast_edits.WARNING,
                "tf.contrib.summary.image no longer takes the 'bad_color' "
                "argument; caller must now preprocess if needed. This call "
                "site specifies a bad_color argument so it cannot be converted "
                "safely."),
            ("family", 4): (
                ast_edits.WARNING,
                "tf.contrib.summary.* functions no longer take the 'family' "
                "argument; instead name scoping should be used. This call site "
                "specifies a family argument so it cannot be converted safely.")
        },
        "tf.contrib.summary.scalar": {
            ("family", 2): (
                ast_edits.WARNING,
                "tf.contrib.summary.* functions no longer take the 'family' "
                "argument; instead name scoping should be used. This call site "
                "specifies a family argument so it cannot be converted safely.")
        },
        "tf.image.resize": {
            ("align_corners",
             3): (ast_edits.WARNING,
                  "align_corners is not supported by tf.image.resize, the new "
                  "default transformation is close to what v1 provided. If you "
                  "require exactly the same transformation as before, use "
                  "compat.v1.image.resize."),
        },
        "tf.image.resize_bilinear": {
            ("align_corners",
             2): (ast_edits.WARNING,
                  "align_corners is not supported by tf.image.resize, the new "
                  "default transformation is close to what v1 provided. If you "
                  "require exactly the same transformation as before, use "
                  "compat.v1.image.resize_bilinear."),
        },
        "tf.image.resize_area": {
            ("align_corners",
             2): (ast_edits.WARNING,
                  "align_corners is not supported by tf.image.resize, the new "
                  "default transformation is close to what v1 provided. If you "
                  "require exactly the same transformation as before, use "
                  "compat.v1.image.resize_area."),
        },
        "tf.image.resize_bicubic": {
            ("align_corners",
             2): (ast_edits.WARNING,
                  "align_corners is not supported by tf.image.resize, the new "
                  "default transformation is close to what v1 provided. If you "
                  "require exactly the same transformation as before, use "
                  "compat.v1.image.resize_bicubic."),
        },
        "tf.image.resize_nearest_neighbor": {
            ("align_corners",
             2): (ast_edits.WARNING,
                  "align_corners is not supported by tf.image.resize, the new "
                  "default transformation is close to what v1 provided. If you "
                  "require exactly the same transformation as before, use "
                  "compat.v1.image.resize_nearest_neighbor."),
        },
    }

    # Specially handled functions
    # Each transformer is a callable which will be called with the arguments
    #   transformer(parent, node, full_name, name, logs)
    # Where logs is a list to which (level, line, col, msg) tuples can be
    # appended, full_name is the FQN of the function called (or None if that is
    # unknown), name is the name of the function called (or None is that is
    # unknown). node is an ast.Call node representing this function call, and
    # parent is its parent in the AST.
    # The function may modify node (but not parent), and must return
    # - none, if nothing was modified
    # - node, if node was modified in place (make sure to use
    #   pasta.ast_utils.replace_child to swap out children, otherwise formatting
    #   may get messy)
    # - a replacement for node, if the whole call node was replaced. The caller
    #   will take care of changing parent.
    self.function_transformers = {
        "*.make_initializable_iterator": _iterator_transformer,
        "*.make_one_shot_iterator": _iterator_transformer,
        "tf.nn.dropout": _dropout_transformer,
        "tf.to_bfloat16": _cast_transformer,
        "tf.to_complex128": _cast_transformer,
        "tf.to_complex64": _cast_transformer,
        "tf.to_double": _cast_transformer,
        "tf.to_float": _cast_transformer,
        "tf.to_int32": _cast_transformer,
        "tf.to_int64": _cast_transformer,
        "tf.nn.softmax_cross_entropy_with_logits":
            _softmax_cross_entropy_with_logits_transformer,
        "tf.image.extract_glimpse": _extract_glimpse_transformer,
        "tf.image.resize_area": _image_resize_transformer,
        "tf.image.resize_bicubic": _image_resize_transformer,
        "tf.image.resize_bilinear": _image_resize_transformer,
        "tf.image.resize_nearest_neighbor": _image_resize_transformer,
        "tf.nn.fractional_avg_pool": _pool_seed_transformer,
        "tf.nn.fractional_max_pool": _pool_seed_transformer,
        "tf.name_scope": _name_scope_transformer,
        "tf.estimator.DNNEstimator":
            functools.partial(
                _rename_if_arg_found_transformer,
                arg_name="input_layer_partitioner",
                message="tf.estimator.DNNEstimator no longer takes "
                "input_layer_partitioner, so the call was converted to "
                "compat.v1."
            ),
        "tf.estimator.DNNClassifier":
            functools.partial(
                _rename_if_arg_found_and_add_loss_reduction_transformer,
                arg_name="input_layer_partitioner",
                message="tf.estimator.DNNClassifier no longer takes "
                "input_layer_partitioner, so the call was converted to "
                "compat.v1."
            ),
        "tf.estimator.DNNRegressor":
            functools.partial(
                _rename_if_arg_found_and_add_loss_reduction_transformer,
                arg_name="input_layer_partitioner",
                message="tf.estimator.DNNRegressor no longer takes "
                "input_layer_partitioner, so the call was converted to "
                "compat.v1."
            ),
        "tf.estimator.LinearEstimator":
            functools.partial(
                _rename_if_arg_found_transformer,
                arg_name="input_layer_partitioner",
                message="tf.estimator.LinearEstimator no longer takes "
                "input_layer_partitioner, so the call was converted to "
                "compat.v1."
            ),
        "tf.estimator.LinearClassifier":
            functools.partial(
                _rename_if_arg_found_and_add_loss_reduction_transformer,
                arg_name="input_layer_partitioner",
                message="tf.estimator.LinearClassifier no longer takes "
                "input_layer_partitioner, so the call was converted to "
                "compat.v1."
            ),
        "tf.estimator.LinearRegressor":
            functools.partial(
                _rename_if_arg_found_and_add_loss_reduction_transformer,
                arg_name="input_layer_partitioner",
                message="tf.estimator.LinearRegressor no longer takes "
                "input_layer_partitioner, so the call was converted to "
                "compat.v1."
            ),
        "tf.estimator.DNNLinearCombinedEstimator":
            functools.partial(
                _rename_if_arg_found_transformer,
                arg_name="input_layer_partitioner",
                message="tf.estimator.DNNLinearCombinedEstimator no longer "
                "takes input_layer_partitioner, so the call was converted to "
                "compat.v1."
            ),
        "tf.estimator.DNNLinearCombinedClassifier":
            functools.partial(
                _rename_if_arg_found_and_add_loss_reduction_transformer,
                arg_name="input_layer_partitioner",
                message="tf.estimator.DNNLinearCombinedClassifier no longer "
                "takes input_layer_partitioner, so the call was converted to "
                "compat.v1."
            ),
        "tf.estimator.DNNLinearCombinedRegressor":
            functools.partial(
                _rename_if_arg_found_and_add_loss_reduction_transformer,
                arg_name="input_layer_partitioner",
                message="tf.estimator.DNNLinearCombinedRegressor no longer "
                "takes input_layer_partitioner, so the call was converted to "
                "compat.v1."
            ),
        "tf.string_split": functools.partial(
            _rename_if_arg_found_transformer, arg_name="skip_empty",
            arg_ok_predicate=_is_ast_false, remove_if_ok=True,
            message="tf.string_split's replacement no longer takes the "
            "skip_empty argument. Since the argument was present, the call was "
            "converted to compat.v1."),
        "tf.device": functools.partial(
            _rename_if_arg_found_transformer, arg_name="device_name",
            arg_ok_predicate=_is_ast_str, remove_if_ok=False,
            message="tf.device no longer takes functions as an argument. "
            "We could not determine that the argument value is a string, so "
            "the call was converted to compat.v1."),
        "tf.zeros_like": functools.partial(
            _rename_if_arg_found_transformer, arg_name="optimize",
            arg_ok_predicate=_is_ast_true, remove_if_ok=True,
            message="tf.zeros_like no longer takes an optimize argument, and "
            "behaves as if optimize=True. This call site specifies something "
            "other than optimize=True, so it was converted to compat.v1."),
        "tf.ones_like": functools.partial(
            _rename_if_arg_found_transformer, arg_name="optimize",
            arg_ok_predicate=_is_ast_true, remove_if_ok=True,
            message="tf.ones_like no longer takes an optimize argument, and "
            "behaves as if optimize=True. This call site specifies something "
            "other than optimize=True, so it was converted to compat.v1."),
        "tf.while_loop": functools.partial(
            _rename_if_arg_found_transformer,
            arg_name="return_same_structure",
            arg_ok_predicate=_is_ast_true, remove_if_ok=True,
            message="tf.while_loop no longer takes 'return_same_structure' "
            "argument and behaves as if return_same_structure=True. This call "
            "site specifies something other than return_same_structure=True, "
            "so it was converted to compat.v1."),
        "tf.nn.ctc_beam_search_decoder": functools.partial(
            _rename_if_arg_found_transformer,
            arg_name="merge_repeated",
            arg_ok_predicate=_is_ast_false, remove_if_ok=True,
            message="tf.nn.ctc_beam_search_decoder no longer takes the "
            "'merge_repeated' argument and behaves as if merge_repeated=False. "
            "This call site specifies something other than "
            "merge_repeated=False, so it was converted to compat.v1."),
        "tf.nn.erosion2d": functools.partial(
            _add_argument_transformer,
            arg_name="data_format",
            arg_value_ast=ast.Str("NHWC")),
        "tf.contrib.summary.audio": _add_summary_step_transformer,
        "tf.contrib.summary.histogram": _add_summary_step_transformer,
        "tf.contrib.summary.image": _add_summary_step_transformer,
        "tf.contrib.summary.scalar": _add_summary_step_transformer,
        "tf.estimator.BaselineClassifier": _add_loss_reduction_transformer,
        "tf.estimator.BaselineRegressor": _add_loss_reduction_transformer,
        "tf.initializers.uniform_unit_scaling":
            _add_uniform_scaling_initializer_transformer,
        "tf.uniform_unit_scaling_initializer":
            _add_uniform_scaling_initializer_transformer,
    }

    self.module_deprecations = {
        "tf.contrib": contrib_warning,
        "tf.flags": flags_warning,
    }


def _is_ast_str(node):
  """Determine whether this node represents a string."""
  allowed_types = [ast.Str]
  if hasattr(ast, "Bytes"):
    allowed_types += [ast.Bytes]
  if hasattr(ast, "JoinedStr"):
    allowed_types += [ast.JoinedStr]
  if hasattr(ast, "FormattedValue"):
    allowed_types += [ast.FormattedValue]
  return isinstance(node, allowed_types)


def _is_ast_true(node):
  if hasattr(ast, "NameConstant"):
    return isinstance(node, ast.NameConstant) and node.value is True
  else:
    return isinstance(node, ast.Name) and node.id == "True"


def _is_ast_false(node):
  if hasattr(ast, "NameConstant"):
    return isinstance(node, ast.NameConstant) and node.value is False
  else:
    return isinstance(node, ast.Name) and node.id == "False"


# Lots of unused arguments below, since these are called in a standard manner.
# pylint: disable=unused-argument


def _rename_if_arg_found_transformer(parent, node, full_name, name, logs,
                                     arg_name=None,
                                     arg_ok_predicate=None,
                                     remove_if_ok=False,
                                     message=None):
  """Replaces the given call with tf.compat.v1 if the given arg is found.

  This requires the function to be called with all named args, so for using
  this transformer, the function should also be added to renames.

  If the arg is not found, the call site is left alone.

  If the arg is found, and if arg_ok_predicate is given, it is called with
  the ast Expression representing the argument value found. If it returns
  True, the function is left alone.

  If the arg is found, arg_ok_predicate is not None and returns ok, and
  remove_if_ok is True, the argument is removed from the call.

  Otherwise, `compat.v1` is inserted between tf and the function name.

  Args:
    parent: Parent of node.
    node: ast.Call node to maybe modify.
    full_name: full name of function to modify
    name: name of function to modify
    logs: list of logs to append to
    arg_name: name of the argument to look for
    arg_ok_predicate: predicate callable with the ast of the argument value,
      returns whether the argument value is allowed.
    remove_if_ok: remove the argument if present and ok as determined by
      arg_ok_predicate.
    message: message to print if a non-ok arg is found (and hence, the function
      is renamed to its compat.v1 version).

  Returns:
    node, if it was modified, else None.
  """
  # Check whether arg is there.
  arg_present, arg_value = ast_edits.get_arg_value(node, arg_name)
  if not arg_present:
    return

  # Check whether arg is problematic (and if not, maybe remove it).
  if arg_ok_predicate and arg_ok_predicate(arg_value):
    if remove_if_ok:
      for i, kw in enumerate(node.keywords):
        if kw.arg == arg_name:
          node.keywords.pop(i)
          logs.append((ast_edits.INFO, node.lineno, node.col_offset,
                       "Removed argument %s for function %s" % (
                           arg_name, full_name or name)))
          break
      return node
    else:
      return

  # All conditions met, insert v1 and log what we did.
  # We must have a full name, so the func is an attribute.
  new_name = full_name.replace("tf.", "tf.compat.v1.", 1)
  node.func = ast_edits.full_name_node(new_name)
  logs.append((
      ast_edits.INFO, node.lineno, node.col_offset,
      "Renaming %s to %s because argument %s is present. %s" %
      (full_name, new_name, arg_name, message if message is not None else "")
  ))
  return node


def _add_argument_transformer(parent, node, full_name, name, logs,
                              arg_name, arg_value_ast):
  """Adds an argument (as a final kwarg arg_name=arg_value_ast)."""
  node.keywords.append(ast.keyword(arg=arg_name, value=arg_value_ast))
  logs.append((
      ast_edits.INFO, node.lineno, node.col_offset,
      "Adding argument '%s' to call to %s." % (pasta.dump(node.keywords[-1],
                                                          full_name or name))
  ))
  return node


def _iterator_transformer(parent, node, full_name, name, logs):
  """Transform iterator methods to compat function calls."""
  # First, check that node.func.value is not already something we like
  # (tf.compat.v1.data), or something which is handled in the rename
  # (tf.data). This transformer only handles the method call to function call
  # conversion.
  if full_name and (full_name.startswith("tf.compat.v1.data") or
                    full_name.startswith("tf.data")):
    return

  # This should never happen, since we're only called for Attribute nodes.
  if not isinstance(node.func, ast.Attribute):
    return

  # Transform from x.f(y) to tf.compat.v1.data.f(x, y)
  # Fortunately, node.func.value should already have valid position info
  node.args = [node.func.value] + node.args
  node.func.value = ast_edits.full_name_node("tf.compat.v1.data")

  logs.append((ast_edits.WARNING, node.lineno, node.col_offset,
               "Changing dataset.%s() to tf.compat.v1.data.%s(dataset). "
               "Please check this transformation.\n" % (name, name)))

  return node


def _dropout_transformer(parent, node, full_name, name, logs):
  """Replace keep_prob with 1-rate."""
  def _replace_keep_prob_node(parent, old_value):
    """Replaces old_value with 1-(old_value)."""
    one = ast.Num(n=1)
    one.lineno = 0
    one.col_offset = 0
    new_value = ast.BinOp(left=one, op=ast.Sub(),
                          right=old_value)
    # This copies the prefix and suffix on old_value to new_value.
    pasta.ast_utils.replace_child(parent, old_value, new_value)
    ast.copy_location(new_value, old_value)
    # Put parentheses around keep_prob.value (and remove the old prefix/
    # suffix, they should only be around new_value).
    pasta.base.formatting.set(old_value, "prefix", "(")
    pasta.base.formatting.set(old_value, "suffix", ")")

  # Check if we have a keep_prob keyword arg
  for keep_prob in node.keywords:
    if keep_prob.arg == "keep_prob":
      logs.append((ast_edits.INFO, node.lineno, node.col_offset,
                   "Changing keep_prob arg of tf.nn.dropout to rate\n"))
      keep_prob.arg = "rate"
      _replace_keep_prob_node(keep_prob, keep_prob.value)
      return node

  # Maybe it was a positional arg
  if len(node.args) < 2:
    logs.append((ast_edits.ERROR, node.lineno, node.col_offset,
                 "tf.nn.dropout called without arguments, so "
                 "automatic fix was disabled. tf.nn.dropout has changed "
                 "the semantics of the second argument."))
  else:
    _replace_keep_prob_node(node, node.args[1])
    logs.append((ast_edits.INFO, node.lineno, node.col_offset,
                 "Changing keep_prob arg of tf.nn.dropout to rate, and "
                 "recomputing value.\n"))

    return node


def _cast_transformer(parent, node, full_name, name, logs):
  """Transforms to_int and to_float to cast(..., dtype=...)."""

  # Find out the dtype to cast to from the function name
  dtype_str = name[3:]
  # Special cases where the full dtype is not given
  if dtype_str == "float":
    dtype_str = "float32"
  elif dtype_str == "double":
    dtype_str = "float64"
  new_arg = ast.keyword(arg="dtype",
                        value=ast.Attribute(value=ast.Name(id="tf",
                                                           ctx=ast.Load()),
                                            attr=dtype_str, ctx=ast.Load()))
  # Ensures a valid transformation when a positional name arg is given
  if len(node.args) == 2:
    name_arg = ast.keyword(arg="name",
                           value=node.args[-1])
    node.args = node.args[:-1]
    node.keywords.append(name_arg)

  # Python3 ast requires the args for the Attribute, but codegen will mess up
  # the arg order if we just set them to 0.
  new_arg.value.lineno = node.lineno
  new_arg.value.col_offset = node.col_offset+100

  node.keywords.append(new_arg)
  if isinstance(node.func, ast.Attribute):
    node.func.attr = "cast"
  else:
    assert isinstance(node.func, ast.Name)
    node.func.id = "cast"

  logs.append((ast_edits.INFO, node.lineno, node.col_offset,
               "Changed %s call to tf.cast(..., dtype=tf.%s)." % (full_name,
                                                                  dtype_str)))
  return node


def _softmax_cross_entropy_with_logits_transformer(
    parent, node, full_name, name, logs):
  """Wrap labels argument with stop_gradients."""
  def _wrap_label(parent, old_value):
    """Wrap labels with tf.stop_gradient."""
    already_stop_grad = (isinstance(old_value, ast.Call) and
                         isinstance(old_value.func, ast.Attribute) and
                         old_value.func.attr == "stop_gradient" and
                         isinstance(old_value.func.value, ast.Name) and
                         old_value.func.value.id == "tf")
    if already_stop_grad:
      return False
    try:
      new_value = ast.Call(
          ast.Name(id="tf.stop_gradient", ctx=ast.Load()),
          [old_value], [])
    except TypeError:
      new_value = ast.Call(
          ast.Name(id="tf.stop_gradient", ctx=ast.Load()),
          [old_value], [], None, None)

    # This copies the prefix and suffix on old_value to new_value.
    pasta.ast_utils.replace_child(parent, old_value, new_value)
    ast.copy_location(new_value, old_value)
    return True

  # Check if we have a labels keyword arg
  for karg in node.keywords:
    if karg.arg == "labels":
      if _wrap_label(karg, karg.value):
        logs.append((ast_edits.INFO, node.lineno, node.col_offset,
                     "Changing labels arg of "
                     "tf.nn.softmax_cross_entropy_with_logits to "
                     "tf.stop_gradient(labels). Please check this "
                     "transformation.\n"))
      return node
  return node


def _image_resize_transformer(parent, node, full_name, name, logs):
  """Transforms image.resize_* to image.resize(..., method=*, ...)."""
  resize_method = name[7:].upper()
  new_arg = ast.keyword(arg="method",
                        value=ast.Attribute(
                            value=ast.Attribute(
                                value=ast.Attribute(
                                    value=ast.Name(id="tf", ctx=ast.Load()),
                                    attr="image", ctx=ast.Load()),
                                attr="ResizeMethod", ctx=ast.Load()),
                            attr=resize_method, ctx=ast.Load()))

  # Ensures a valid transformation when a positional name arg is given
  if len(node.args) == 4:
    pos_arg = ast.keyword(arg="preserve_aspect_ratio",
                          value=node.args[-1])
    node.args = node.args[:-1]
    node.keywords.append(pos_arg)
  if len(node.args) == 3:
    pos_arg = ast.keyword(arg="align_corners",
                          value=node.args[-1])
    node.args = node.args[:-1]

  new_keywords = []
  for kw in node.keywords:
    if kw.arg != "align_corners":
      new_keywords.append(kw)
  node.keywords = new_keywords

  # Python3 ast requires the args for the Attribute, but codegen will mess up
  # the arg order if we just set them to 0.
  new_arg.value.lineno = node.lineno
  new_arg.value.col_offset = node.col_offset+100

  node.keywords.append(new_arg)
  if isinstance(node.func, ast.Attribute):
    node.func.attr = "resize"
  else:
    assert isinstance(node.func, ast.Name)
    node.func.id = "resize"

  logs.append((ast_edits.INFO, node.lineno, node.col_offset,
               "Changed %s call to tf.image.resize(..., "
               "method=tf.image.ResizeMethod.%s)." % (full_name,
                                                      resize_method)))
  return node


def _pool_seed_transformer(parent, node, full_name, name, logs):
  """Removes seed2 and deterministic, and adds non-zero seed if needed."""
  # This requires that this function uses all kwargs (add to renames!).
  seed_arg = None
  deterministic = False
  modified = False
  new_keywords = []

  for kw in node.keywords:
    if sys.version_info[:2] >= (3, 5) and isinstance(kw, ast.Starred):
      pass
    elif kw.arg == "seed":
      seed_arg = kw
    elif kw.arg == "seed2" or kw.arg == "deterministic":
      lineno = getattr(kw, "lineno", node.lineno)
      col_offset = getattr(kw, "col_offset", node.col_offset)
      logs.append((ast_edits.INFO, lineno, col_offset,
                   "Removed argument %s for function %s" % (
                       kw.arg, full_name or name)))
      if kw.arg == "deterministic":
        if not _is_ast_false(kw.value):
          deterministic = True
      modified = True
      continue
    new_keywords.append(kw)

  if deterministic:
    if seed_arg is None:
      new_keywords.append(ast.keyword(arg="seed", value=ast.Num(42)))
      logs.add((
          ast_edits.INFO, node.lineno, node.col_offset,
          "Adding seed=42 to call to %s since determinism was requested" % (
              full_name or name)
      ))
    else:
      logs.add((
          ast_edits.WARNING, node.lineno, node.col_offset,
          "The deterministic argument is deprecated for %s, pass a "
          "non-zero seed for determinism. The deterministic argument is "
          "present, possibly not False, and the seed is already set. The "
          "converter cannot determine whether it is nonzero, please check."
      ))

  if modified:
    node.keywords = new_keywords
    return node
  else:
    return


def _extract_glimpse_transformer(parent, node, full_name, name, logs):

  def _replace_uniform_noise_node(parent, old_value):
    """Replaces old_value with 'uniform' or 'guassian'."""
    uniform = ast.Str(s="uniform")
    gaussian = ast.Str(s="gaussian")
    new_value = ast.IfExp(body=uniform, test=old_value, orelse=gaussian)
    # This copies the prefix and suffix on old_value to new_value.
    pasta.ast_utils.replace_child(parent, old_value, new_value)
    ast.copy_location(new_value, old_value)
    # Put parentheses around noise.value.test (and remove the old prefix/
    # suffix, they should only be around new_value.test), so that:
    # "uniform" if (a if b else c) else "gaussian" is valid.
    pasta.base.formatting.set(new_value.test, "prefix", "(")
    pasta.base.formatting.set(new_value.test, "suffix", ")")

  # Check if we have a uniform_noise keyword arg
  for uniform_noise in node.keywords:
    if uniform_noise.arg == "uniform_noise":
      logs.append((ast_edits.INFO, node.lineno, node.col_offset,
                   "Changing uniform_noise arg of tf.image.extract_glimpse "
                   "to noise, and recomputing value. Please check this "
                   "transformation.\n"))
      uniform_noise.arg = "noise"
      value = "uniform" if uniform_noise.value else "gaussian"
      _replace_uniform_noise_node(uniform_noise, uniform_noise.value)
      return node

  # Since `noise`/`uniform_noise` is optional arg, nothing needs to be
  # done if len(node.args) < 5.
  if len(node.args) >= 5:
    _replace_uniform_noise_node(node, node.args[5])
    logs.append((ast_edits.INFO, node.lineno, node.col_offset,
                 "Changing uniform_noise arg of tf.image.extract_glimpse to "
                 "noise, and recomputing value.\n"))
    return node


def _add_summary_step_transformer(parent, node, full_name, name, logs):
  """Adds a step argument to the summary API call if not specified.

  The inserted argument value is tf.compat.v1.train.get_or_create_global_step().
  """
  for keyword_arg in node.keywords:
    if keyword_arg.arg == "step":
      return node
  default_value = "tf.compat.v1.train.get_or_create_global_step()"
  # Parse with pasta instead of ast to avoid emitting a spurious trailing \n.
  ast_value = pasta.parse(default_value)
  node.keywords.append(ast.keyword(arg="step", value=ast_value))
  logs.append((
      ast_edits.WARNING, node.lineno, node.col_offset,
      "Summary API writing function %s now requires a 'step' argument; "
      "inserting default of %s." % (full_name or name, default_value)))
  return node


def _add_loss_reduction_transformer(parent, node, full_name, name, logs):
  """Adds a loss_reduction argument if not specified.

  Default value for tf.estimator.*Classifier and tf.estimator.*Regressor
  loss_reduction argument changed to SUM_OVER_BATCH_SIZE. So, we update
  existing calls to use the old default value `tf.losses.Reduction.SUM`.

  Note: to apply this transformation, symbol must be added
  to reordered_function_names above.
  """
  for keyword_arg in node.keywords:
    if keyword_arg.arg == "loss_reduction":
      return node
  # TODO(annarev): this should be updated to tf.keras.losses.Reduction.SUM
  # once b/125525822 is fixed.
  default_value = "tf.compat.v1.losses.Reduction.SUM"
  # Parse with pasta instead of ast to avoid emitting a spurious trailing \n.
  ast_value = pasta.parse(default_value)
  node.keywords.append(ast.keyword(arg="loss_reduction", value=ast_value))
  logs.append((
      ast_edits.INFO, node.lineno, node.col_offset,
      "%s: Default value of loss_reduction has been changed to "
      "SUM_OVER_BATCH_SIZE; inserting old default value %s.\n"
      % (full_name or name, default_value)))
  return node


def _rename_if_arg_found_and_add_loss_reduction_transformer(
    parent,
    node,
    full_name,
    name,
    logs,
    arg_name=None,
    arg_ok_predicate=None,
    remove_if_ok=False,
    message=None):
  """Combination of _rename_if_arg_found and _add_loss_reduction transformers.

  Args:
    parent: Parent of node.
    node: ast.Call node to maybe modify.
    full_name: full name of function to modify
    name: name of function to modify
    logs: list of logs to append to
    arg_name: name of the argument to look for
    arg_ok_predicate: predicate callable with the ast of the argument value,
      returns whether the argument value is allowed.
    remove_if_ok: remove the argument if present and ok as determined by
      arg_ok_predicate.
    message: message to print if a non-ok arg is found (and hence, the function
      is renamed to its compat.v1 version).

  Returns:
    node, if it was modified, else None.
  """

  add_loss_node = _add_loss_reduction_transformer(parent, node, full_name, name,
                                                  logs)
  rename_node = _rename_if_arg_found_transformer(
      parent, add_loss_node, full_name, name, logs, arg_name, arg_ok_predicate,
      remove_if_ok, message)

  return rename_node


def _add_uniform_scaling_initializer_transformer(
    parent, node, full_name, name, logs):
  """Updates references to uniform_unit_scaling_initializer.

  Transforms:
  tf.uniform_unit_scaling_initializer(factor, seed, dtype) to
  tf.compat.v1.keras.initializers.VarianceScaling(
      scale=factor, distribution="uniform", seed=seed)

  Note: to apply this transformation, symbol must be added
  to reordered_function_names above.
  """
  for keyword_arg in node.keywords:
    if keyword_arg.arg == "factor":
      keyword_arg.arg = "scale"

  distribution_value = "\"uniform\""
  # Parse with pasta instead of ast to avoid emitting a spurious trailing \n.
  ast_value = pasta.parse(distribution_value)
  node.keywords.append(ast.keyword(arg="distribution", value=ast_value))

  lineno = node.func.value.lineno
  col_offset = node.func.value.col_offset
  node.func.value = ast_edits.full_name_node("tf.compat.v1.keras.initializers")
  node.func.value.lineno = lineno
  node.func.value.col_offset = col_offset
  node.func.attr = "VarianceScaling"
  return node


def _name_scope_transformer(parent, node, full_name, name, logs):
  """Fix name scope invocation to use 'default_name' and omit 'values' args."""

  name_found, name = ast_edits.get_arg_value(node, "name", 0)
  default_found, default_name = ast_edits.get_arg_value(node, "default_name", 1)

  # If an actual name was given...
  if name_found and pasta.dump(name) != "None":
    logs.append((ast_edits.INFO, node.lineno, node.col_offset,
                 "`name` passed to `name_scope`. Because you may be re-entering"
                 " an existing scope, it is not safe to convert automatically, "
                 " the v2 name_scope does not support re-entering scopes by"
                 " name.\n"))
    # Rename to compat.v1
    new_name = "tf.compat.v1.name_scope"
    logs.append((ast_edits.INFO, node.func.lineno, node.func.col_offset,
                 "Renamed %r to %r" % (full_name, new_name)))
    new_name_node = ast_edits.full_name_node(new_name, node.func.ctx)
    ast.copy_location(new_name_node, node.func)
    pasta.ast_utils.replace_child(node, node.func, new_name_node)
    return node

  if default_found:
    # New name scope doesn't have name, but it has a default name. We use
    # name=default_name, and values can be dropped (it's only for
    # error reporting and useless outside of graph mode).
    logs.append((ast_edits.INFO, node.lineno, node.col_offset,
                 "Using default_name as name in call to name_scope.\n"))
    # Remove all args other than name
    node.args = []
    node.keywords = [ast.keyword(arg="name", value=default_name)]
    return node

  logs.append((ast_edits.ERROR, node.lineno, node.col_offset,
               "name_scope call with neither name nor default_name cannot be "
               "converted properly."))
