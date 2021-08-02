# Copyright 2015 The TensorFlow Authors. All Rights Reserved.
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
"""Saves and restore variables inside traced @tf.functions."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from tensorflow.core.protobuf import saver_pb2
from tensorflow.python.eager import def_function
from tensorflow.python.framework import dtypes
from tensorflow.python.framework import ops
from tensorflow.python.framework import tensor_spec
from tensorflow.python.ops import array_ops
from tensorflow.python.ops import io_ops
from tensorflow.python.training.saving import saveable_object
from tensorflow.python.util import nest


class Saver(object):
  """A minimal utility class for saving and restoring checkpoints.

  Note that this is a low-level utility which stores Tensors in the keys
  specified by `SaveableObject`s. Higher-level utilities for object-based
  checkpointing are built on top of it.
  """

  def __init__(self, saveable_objects):
    """Specify a list of `SaveableObject`s to save and restore.

    Args:
      saveable_objects: A list of `SaveableObject`s.
    """
    saveable_objects = list(saveable_objects)
    for saveable in saveable_objects:
      if not isinstance(saveable, saveable_object.SaveableObject):
        raise ValueError(
            "Saver expected a list of SaveableObjects, got %s." % (saveable,))
    self._saveable_objects = saveable_objects

  def to_proto(self):
    """Serializes to a SaverDef referencing the current graph."""
    filename_tensor = array_ops.placeholder(
        shape=[], dtype=dtypes.string, name="saver_filename")
    # TODO(allenl): Add save and restore function names to the proto directly.
    signature = (tensor_spec.TensorSpec(shape=(), dtype=dtypes.string),)
    # Autograph is off because of reference cycles which must be collected when
    # a function is created and destroyed (as in tf.saved_model.save). It's also
    # not necessary, so having it off may be slightly faster.
    #
    # TODO(b/121302372): We should be able to decorate save() and restore()
    # unconditionally.
    save_tensor = def_function.function(
        self.save, input_signature=signature, autograph=False)(filename_tensor)
    restore_op = def_function.function(
        self.restore, input_signature=signature, autograph=False)(
            filename_tensor).op
    return saver_pb2.SaverDef(
        filename_tensor_name=filename_tensor.name,
        save_tensor_name=save_tensor.name,
        restore_op_name=restore_op.name,
        version=saver_pb2.SaverDef.V2)

  def save(self, file_prefix):
    """Save the saveable objects to a checkpoint with `file_prefix`.

    Args:
      file_prefix: A string or scalar string Tensor containing the prefix to
        save under.
    Returns:
      A scalar string Tensor containing `file_prefix` with control dependencies
      on the save ops.
    """
    tensor_names = []
    tensors = []
    tensor_slices = []
    for saveable in self._saveable_objects:
      for spec in saveable.specs:
        tensor_names.append(spec.name)
        tensors.append(spec.tensor)
        tensor_slices.append(spec.slice_spec)
    with ops.device("cpu:0"):
      with ops.control_dependencies([io_ops.save_v2(
          file_prefix, tensor_names, tensor_slices, tensors)]):
        return array_ops.identity(file_prefix)

  def restore(self, file_prefix):
    """Restore the saveable objects from a checkpoint with `file_prefix`.

    Args:
      file_prefix: A string or scalar string Tensor containing the prefix for
        files to read from.

    Returns:
      A scalar string Tensor containing `file_prefix` with control dependencies
      on the restore ops.
    """
    restore_ops = restore_from_saveable_objects(
        file_prefix, self._saveable_objects)
    with ops.device("cpu:0"):
      with ops.control_dependencies(restore_ops):
        return array_ops.identity(file_prefix)


def restore_from_saveable_objects(file_prefix, saveable_objects):
  """Reads from a checkpoint and returns restore ops for `saveable_objects`s."""
  restore_specs = []
  tensor_structure = []
  for saveable in saveable_objects:
    saveable_tensor_structure = []
    tensor_structure.append(saveable_tensor_structure)
    for spec in saveable.specs:
      saveable_tensor_structure.append(spec.name)
      restore_specs.append((spec.name, spec.slice_spec, spec.dtype))
  tensor_names, tensor_slices, tensor_dtypes = zip(*restore_specs)
  with ops.device("cpu:0"):
    restored_tensors = io_ops.restore_v2(
        file_prefix, tensor_names, tensor_slices, tensor_dtypes)
  structured_restored_tensors = nest.pack_sequence_as(
      tensor_structure, restored_tensors)
  restore_ops = []
  for saveable, restored_tensors in zip(saveable_objects,
                                        structured_restored_tensors):
    restore_ops.append(saveable.restore(restored_tensors,
                                        restored_shapes=None))
  return restore_ops
