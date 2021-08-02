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
"""Various classes representing distributed inputs."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from tensorflow.python.data.experimental.ops import batching
from tensorflow.python.data.ops import dataset_ops
from tensorflow.python.data.ops import multi_device_iterator_ops
from tensorflow.python.data.util import structure
from tensorflow.python.distribute import device_util
from tensorflow.python.distribute import distribution_strategy_context
from tensorflow.python.distribute import input_ops
from tensorflow.python.distribute import values
from tensorflow.python.eager import context
from tensorflow.python.framework import constant_op
from tensorflow.python.framework import device as tf_device
from tensorflow.python.framework import ops
from tensorflow.python.framework import tensor_shape
from tensorflow.python.framework import tensor_util
from tensorflow.python.ops import array_ops
from tensorflow.python.ops import control_flow_ops
from tensorflow.python.ops import math_ops
from tensorflow.python.util import nest


class InputWorkers(object):
  """A 1-to-many mapping from input worker devices to compute devices."""

  def __init__(self, device_map, worker_device_pairs=None, logical_device=0):
    """Initialize an `InputWorkers` object.

    Args:
      device_map: A `DeviceMap` with the computation devices fed by the
        input workers.
      worker_device_pairs: A sequence of pairs:
        `(input device, a tuple of compute devices fed by that input device)`.
      logical_device: The logical device of `device_map` to feed.
    """
    self._device_map = device_map
    self._logical_device = logical_device
    if worker_device_pairs is None:
      worker_device_pairs = ((
          device_util.canonicalize("/device:CPU:0"),
          device_map.logical_to_actual_devices(logical_device)),)
    self._input_worker_devices = tuple(d for d, _ in worker_device_pairs)
    self._fed_devices = tuple(tuple(device_util.canonicalize(d) for d in f)
                              for _, f in worker_device_pairs)
    flattened = tuple(d for l in self._fed_devices for d in l)
    assert (flattened ==
            device_map.logical_to_actual_devices(logical_device)), (
                "flattened: %s logical device %d: %s" %
                (flattened, logical_device,
                 device_map.logical_to_actual_devices(logical_device)))

  @property
  def device_map(self):
    return self._device_map

  @property
  def logical_device(self):
    return self._logical_device

  @property
  def num_workers(self):
    return len(self._input_worker_devices)

  @property
  def worker_devices(self):
    return self._input_worker_devices

  def compute_devices_for_worker(self, worker_index):
    return self._fed_devices[worker_index]

  def __repr__(self):
    devices = self.worker_devices
    debug_repr = ",\n".join("  %d %s: %s" %
                            (i, devices[i], self._fed_devices[i])
                            for i in range(len(devices)))
    return "%s:{\n%s\n  device_map: %s}" % (
        self.__class__.__name__, debug_repr, self._device_map)


class InputIterator(object):
  """An input iterator, intended to be passed to `DistributionStrategy.run`."""

  def get_next(self):
    """Returns the next inputs for all replicas."""
    raise NotImplementedError("must be implemented in descendants")

  def initialize(self):
    """Initialize the underlying input dataset, when applicable.

    In eager mode, this will create a new iterator and return it.
    In graph mode, this will initialize the same underlying iterator(s).

    Users are required to call this if
    - This iterator was returned from a call to `make_input_fn_iterator` with an
      input function that returns a dataset.
    - Or this iterator was returned from a call to `make_dataset_iterator`.

    Returns:
      A list of initialization ops to be executed.
    """
    raise NotImplementedError("must be implemented in descendants")


class InputIteratorImpl(InputIterator):
  """Common implementation for all input iterators."""

  def __init__(self, input_workers, iterators):
    assert isinstance(input_workers, InputWorkers)
    if not input_workers.worker_devices:
      raise ValueError("Should have at least one worker for input iterator.")

    self._iterators = iterators
    self._input_workers = input_workers

  def get_next(self, name=None):
    """Returns the next input from the iterator for all replicas."""
    replicas = []
    worker_has_values = []
    for i, worker in enumerate(self._input_workers.worker_devices):
      if name is not None:
        d = tf_device.DeviceSpec.from_string(worker)
        new_name = "%s_%s_%d" % (name, d.job, d.task)
      else:
        new_name = None
      with ops.device(worker):
        worker_has_value, next_element = (
            self._iterators[i].get_next_as_list(new_name))
        worker_has_values.append(worker_has_value)
        # Make `replicas` a flat list of values across all replicas.
        replicas.append(next_element)

    out_of_range_replicas = []

    def out_of_range_fn(worker_index, device):
      """This function will throw an OutOfRange error."""
      # As this will be only called when there is no data left, so calling
      # get_next() will trigger an OutOfRange error.
      data = self._iterators[worker_index].get_next(device)
      out_of_range_replicas.append(data)
      return data

    # `global_has_value` indicates whether there is data in this global batch.
    # We do a all-reduce across all the workers in the multi-worker case.
    # TODO(b/126259107): Do strategy.reduce for CollectiveAllReduceStrategy.
    if len(worker_has_values) > 1:
      with ops.device(self._input_workers.compute_devices_for_worker(0)[0]):
        # Place the tf.reduce_any op in device 0 to minimize communication
        # cost.
        # TODO(b/128545270): Investigate why placing it on worker 0 will cause
        # the entire data to copy back from device to host.
        global_has_value = math_ops.reduce_any(worker_has_values)
    else:
      global_has_value = worker_has_values[0]

    results = []
    for i, worker in enumerate(self._input_workers.worker_devices):
      with ops.device(worker):
        devices = self._input_workers.compute_devices_for_worker(i)
        for j, device in enumerate(devices):
          with ops.device(device):
            # pylint: disable=undefined-loop-variable
            # pylint: disable=cell-var-from-loop
            # It is fine for the lambda to capture variables from the loop as
            # the lambda is executed in the loop as well.
            result = control_flow_ops.cond(global_has_value,
                                           lambda: replicas[i][j],
                                           lambda: out_of_range_fn(i, device))
            # pylint: enable=cell-var-from-loop
            # pylint: enable=undefined-loop-variable
            results.append(result)
    replicas = results

    # Some dimensions in `replicas` will become unknown after we conditionally
    # return the real tensors or the dummy tensors. We fix the input shapes by
    # using the shapes from `out_of_range_replicas` because it is calling
    # get_next() inside.
    flattened_replicas = nest.flatten(replicas)
    for i, replica_data in enumerate(nest.flatten(out_of_range_replicas)):
      flattened_replicas[i].set_shape(replica_data.get_shape())
    replicas = nest.pack_sequence_as(replicas, flattened_replicas)

    return values.regroup(self._input_workers.device_map, replicas)

  def initialize(self):
    """Initialze underlying iterators.

    Returns:
      A list of any initializer ops that should be run.
    """
    init_ops = []
    for it in self._iterators:
      init_ops.extend(it.initialize())
    return init_ops

  # TODO(priyag): Remove when we switch to using `MultiDeviceIterator` for TPUs.
  @property
  def output_classes(self):
    return self._iterators[0].output_classes

  # TODO(priyag): Remove when we switch to using `MultiDeviceIterator` for TPUs.
  @property
  def output_shapes(self):
    return self._iterators[0].output_shapes

  # TODO(priyag): Remove when we switch to using `MultiDeviceIterator` for TPUs.
  @property
  def output_types(self):
    return self._iterators[0].output_types

  # TODO(priyag): Remove when we switch to using `MultiDeviceIterator` for TPUs.
  def get_iterator(self, worker):
    for i, w in enumerate(self._input_workers.worker_devices):
      if worker == w:
        return self._iterators[i]
    return None


class InputFunctionIterator(InputIteratorImpl):
  """Iterator created from input function."""

  def __init__(self, input_fn, input_workers, input_contexts):
    """Make an iterator for input provided via an input function.

    Currently implements PER_WORKER mode, in which the `input_fn` is called
    once on each worker.

    TODO(priyag): Add other replication modes.

    Args:
      input_fn: Input function that returns a `tf.data.Dataset` object.
      input_workers: an `InputWorkers` object.
      input_contexts: A list of `InputContext` instances to be passed to call(s)
        to `input_fn`. Length and order should match worker order in
        `worker_device_pairs`.
    """
    assert isinstance(input_workers, InputWorkers)
    if input_workers.num_workers != len(input_contexts):
      raise ValueError(
          "Number of input workers (%d) is not same as number of "
          "input_contexts (%d)" %
          (input_workers.num_workers, len(input_contexts)))

    iterators = []
    for i, ctx in enumerate(input_contexts):
      worker = input_workers.worker_devices[i]
      with ops.device(worker):
        result = input_fn(ctx)
        devices = input_workers.compute_devices_for_worker(i)
        if isinstance(result, dataset_ops.DatasetV2):
          iterator = _SingleWorkerDatasetIterator(result, worker, devices)
        elif callable(result):
          iterator = _SingleWorkerCallableIterator(result, worker, devices)
        else:
          raise ValueError(
              "input_fn must return a tf.data.Dataset or a callable.")
        iterators.append(iterator)

    super(InputFunctionIterator, self).__init__(input_workers, iterators)


class DatasetIterator(InputIteratorImpl):
  """Iterator created from input dataset."""

  def __init__(self, dataset, input_workers, split_batch_by=None):
    """Make an iterator for the dataset on given devices.

    If `split_batch_by` is not None, we "split" each batch of the
    dataset by `split_batch_by` value. To achieve this, we first unbatch the
    input dataset and then rebatch it with the per replica batch size that is
    calculated using `global_batch_size // split_batch_by`.
    The currently supported datasets are as follows:
    `dataset.batch()` is the last operation on the dataset OR
    `dataset.apply(map_and_batch)` is the last operation on the dataset OR
    `dataset.batch().prefetch()` are the last 2 operations on the dataset OR
    `dataset.apply(map_and_batch).prefetch()` are the last 2 operations.

    TODO(priyag): Support multi worker / host cases properly by cloning
    and sharding the dataset on each worker. Current setup will only work in
    some cases, such as in-graph multi worker GPU case. If the input pipeline
    has random shuffling (with a different seed on each worker), each worker
    will see random input from the same overall dataset in each step. Otherwise,
    each worker will see the same input in each step.

    Args:
      dataset: `tf.data.Dataset` that will be used as the input source.
      input_workers: an `InputWorkers` object.
      split_batch_by: Optional integer. If present, we "split" each batch of the
        dataset by `split_batch_by` value.
    """
    assert isinstance(input_workers, InputWorkers)
    if split_batch_by:
      dataset = batching._RebatchDataset(dataset, split_batch_by)  # pylint: disable=protected-access

    iterators = []
    for i, worker in enumerate(input_workers.worker_devices):
      with ops.device(worker):
        worker_devices = input_workers.compute_devices_for_worker(i)
        cloned_dataset = dataset
        if not context.executing_eagerly():
          cloned_dataset = input_ops._clone_dataset(dataset)  # pylint: disable=protected-access
          cloned_dataset = cloned_dataset.with_options(dataset.options())
        iterator = _SingleWorkerDatasetIterator(cloned_dataset, worker,
                                                worker_devices)
        iterators.append(iterator)

    self._element_structure = dataset._element_structure  # pylint: disable=protected-access

    super(DatasetIterator, self).__init__(input_workers, iterators)


def _dummy_tensor_fn(value_structure):
  """A function to create dummy tensors from `value_structure`."""

  def create_dummy_tensor(feature_shape, feature_type):
    """Create a dummy tensor with possible batch dimensions set to 0."""

    # Ideally we should set the batch dimension to 0, however as in
    # DistributionStrategy we don't know the batch dimension, we try to
    # guess it as much as possible. If the feature has unknown dimensions, we
    # will set them to 0. If the feature shape is already static, we guess the
    # first dimension as batch dimension and set it to 0.
    dims = []
    for dim in feature_shape.dims:
      if dim.value is None:
        dims.append(tensor_shape.Dimension(0))
      else:
        dims.append(dim)
    if feature_shape.is_fully_defined() and dims:
      dims[0] = tensor_shape.Dimension(0)

    # Create the dummy tensor.
    dummy_tensor = array_ops.zeros(tensor_shape.TensorShape(dims), feature_type)
    return dummy_tensor

  result = []
  # pylint: disable=protected-access
  for feature_shape, feature_type in zip(value_structure._flat_shapes,
                                         value_structure._flat_types):
    result.append(create_dummy_tensor(feature_shape, feature_type))

  if isinstance(value_structure, structure.NestedStructure):
    result = nest.pack_sequence_as(value_structure._nested_structure, result)
  else:
    result = result[0]
  # pylint: enable=protected-access

  return result


class _SingleWorkerDatasetIterator(object):
  """Iterator for a single `tf.data.Dataset`."""

  def __init__(self, dataset, worker, devices):
    """Create iterator for the `dataset` to fetch data to worker's `devices` .

    `MultiDeviceIterator` is used to prefetch input to the devices on the
    given worker.

    Args:
      dataset: A `tf.data.Dataset` instance.
      worker: Worker on which ops should be created.
      devices: Distribute data from `dataset` to these devices.
    """
    self._dataset = dataset
    self._worker = worker
    self._devices = devices
    self._make_iterator()

  def _make_iterator(self):
    """Make appropriate iterator on the dataset."""
    with ops.device(self._worker):
      self._iterator = multi_device_iterator_ops.MultiDeviceIterator(
          self._dataset, self._devices)

  def get_next(self, device, name=None):
    """Get next element for the given device."""
    del name
    with ops.device(self._worker):
      return self._iterator.get_next(device)

  def get_next_as_list(self, name=None):
    """Get next element from underlying iterator.

    If there is no data left, a list of dummy tensors with possible batch
    dimensions set to 0 will be returned.

    Args:
      name: not used.

    Returns:
      A boolean tensor indicates whether there is any data in next element and
      the real data as the next element or a list of dummy tensors if no data
      left.
    """
    del name
    with ops.device(self._worker):
      data_list = self._iterator.get_next_as_optional()
      result = []
      for i, data in enumerate(data_list):
        # Place the condition op in the same device as the data so the data
        # doesn't need to be sent back to the worker.
        with ops.device(self._devices[i]):
          # As MultiDeviceIterator will fetch data in order, so we only need to
          # check if the first replica has value to see whether there is data
          # left for this single worker.
          if i == 0:
            worker_has_value = data.has_value()

          # pylint: disable=unnecessary-lambda
          # pylint: disable=cell-var-from-loop
          real_data = control_flow_ops.cond(
              data.has_value(),
              lambda: data.get_value(),
              lambda: _dummy_tensor_fn(data.value_structure))
          result.append(real_data)
          # pylint: enable=cell-var-from-loop
          # pylint: enable=unnecessary-lambda

      return worker_has_value, result

  def initialize(self):
    """Initialze underlying iterator.

    In eager execution, this simply recreates the underlying iterator.
    In graph execution, it returns the initializer ops for the underlying
    iterator.

    Returns:
      A list of any initializer ops that should be run.
    """
    if context.executing_eagerly():
      self._iterator._eager_reset()  # pylint: disable=protected-access
      return []
    else:
      return [self._iterator.initializer]

  @property
  def output_classes(self):
    return dataset_ops.get_legacy_output_classes(self._iterator)

  @property
  def output_shapes(self):
    return dataset_ops.get_legacy_output_shapes(self._iterator)

  @property
  def output_types(self):
    return dataset_ops.get_legacy_output_types(self._iterator)


class _SingleWorkerCallableIterator(object):
  """Iterator for a single tensor-returning callable."""

  def __init__(self, fn, worker, devices):
    self._fn = fn
    self._worker = worker
    self._devices = devices

  def get_next(self, device, name=None):
    """Get next element for the given device from the callable."""
    del device, name
    with ops.device(self._worker):
      return self._fn()

  def get_next_as_list(self, name=None):
    """Get next element from the callable."""
    del name
    with ops.device(self._worker):
      data_list = [self._fn() for _ in self._devices]
      return constant_op.constant(True), data_list

  def initialize(self):
    # TODO(petebu) Should this throw an exception instead?
    return []


# TODO(sourabhbajaj): Remove this in lieu of distributed datasets
def _get_batched_dataset(d):
  """Get the batched dataset from `d`."""
  # pylint: disable=protected-access
  if isinstance(d, dataset_ops.DatasetV1Adapter):
    d = d._dataset

  if isinstance(d, (dataset_ops.BatchDataset, batching._MapAndBatchDataset)):
    return d
  elif isinstance(d, (dataset_ops.PrefetchDataset,
                      dataset_ops._OptionsDataset)):
    return _get_batched_dataset(d._input_dataset)

  raise ValueError(
      "Unable to get batched dataset from the input dataset. `batch` "
      "`map_and_batch` need to be the last operations on the dataset. "
      "The batch operations can be followed by a prefetch.")


def _get_batched_dataset_attributes(d):
  """Get `batch_size`, `drop_remainder` of dataset."""
  # pylint: disable=protected-access
  assert isinstance(d,
                    (dataset_ops.BatchDataset, batching._MapAndBatchDataset))
  if isinstance(d, dataset_ops.BatchDataset):
    batch_size = d._batch_size
    drop_remainder = d._drop_remainder
  elif isinstance(d, batching._MapAndBatchDataset):
    batch_size = d._batch_size_t
    drop_remainder = d._drop_remainder_t
  # pylint: enable=protected-access

  if tensor_util.is_tensor(batch_size):
    batch_size = tensor_util.constant_value(batch_size)

  if tensor_util.is_tensor(drop_remainder):
    drop_remainder = tensor_util.constant_value(drop_remainder)

  return batch_size, drop_remainder


# TODO(sourabhbajaj): Remove this in lieu of distributed datasets
def _get_dataset_attributes(dataset):
  """Get the underlying attributes from the dataset object."""
  # pylint: disable=protected-access

  # First, get batch_size and drop_remainder from the dataset. We need
  # to walk back the dataset creation process and find the batched version in
  # order to get the attributes.
  batched_dataset = _get_batched_dataset(dataset)
  batch_size, drop_remainder = _get_batched_dataset_attributes(batched_dataset)

  # Second, prefetch buffer should be get from the original dataset.
  prefetch_buffer = None
  if isinstance(dataset, dataset_ops.PrefetchDataset):
    prefetch_buffer = dataset._buffer_size
  elif (isinstance(dataset, dataset_ops.DatasetV1Adapter)
        and isinstance(dataset._dataset, dataset_ops.PrefetchDataset)):
    prefetch_buffer = dataset._dataset._buffer_size

  return batch_size, drop_remainder, prefetch_buffer


class MultiStepContext(object):
  """A context object that can be used to capture things when running steps.

  This context object is useful when running multiple steps at a time using the
  `experimental_run_steps_on_iterator` API. For e.g. it allows the user's step
  function to specify which outputs to emit at what frequency. Currently it
  supports capturing output from the last step, as well as capturing non tensor
  outputs.  In the future it will be augmented to support other use cases such
  as output each N steps.
  """

  def __init__(self):
    """Initialize an output context.

    Returns:
      A context object.
    """
    self._last_step_outputs = {}
    self._last_step_outputs_reduce_ops = {}
    self._non_tensor_outputs = {}

  @property
  def last_step_outputs(self):
    """A dictionary consisting of outputs to be captured on last step.

    Keys in the dictionary are names of tensors to be captured, as specified
    when `set_last_step_output` is called.
    Values in the dictionary are the tensors themselves. If
    `set_last_step_output` was called with a `reduce_op` for this output,
    then the value is the reduced value.

    Returns:
      A dictionary with last step outputs.
    """
    return self._last_step_outputs

  def _set_last_step_outputs(self, outputs):
    """Replace the entire dictionary of last step outputs."""
    if not isinstance(outputs, dict):
      raise ValueError("Need a dictionary to set last_step_outputs.")
    self._last_step_outputs = outputs

  def set_last_step_output(self, name, output, reduce_op=None):
    """Set `output` with `name` to be outputted from the last step.

    Args:
      name: String, name to identify the output. Doesn't need to match tensor
        name.
      output: The tensors that should be outputted with `name`. See below for
        actual types supported.
      reduce_op: Reduction method to use to reduce outputs from multiple
        replicas. Required if `set_last_step_output` is called in a replica
        context. Optional in cross_replica_context.
        When present, the outputs from all the replicas are reduced using the
        current distribution strategy's `reduce` method. Hence, the type of
        `output` must be what's supported by the corresponding `reduce` method.
        For e.g. if using MirroredStrategy and reduction is set, output
        must be a `PerReplica` value.
        The reduce method is also recorded in a dictionary
        `_last_step_outputs_reduce_ops` for later interpreting of the
        outputs as already reduced or not.
    """
    if distribution_strategy_context.in_cross_replica_context():
      self._last_step_outputs_reduce_ops[name] = reduce_op
      if reduce_op is None:
        self._last_step_outputs[name] = output
      else:
        distribution = distribution_strategy_context.get_strategy()
        self._last_step_outputs[name] = distribution.reduce(reduce_op, output)
    else:
      assert reduce_op is not None
      def merge_fn(distribution, value):
        self._last_step_outputs[name] = distribution.reduce(reduce_op, value)
        # Setting this inside the `merge_fn` because all replicas share the same
        # context object, so it's more robust to set it only once (even if all
        # the replicas are trying to set the same value).
        self._last_step_outputs_reduce_ops[name] = reduce_op

      distribution_strategy_context.get_replica_context().merge_call(
          merge_fn, args=(output,))

  @property
  def non_tensor_outputs(self):
    """A dictionary consisting of any non tensor outputs to be captured."""
    return self._non_tensor_outputs

  def set_non_tensor_output(self, name, output):
    """Set `output` with `name` to be captured as a non tensor output."""
    if distribution_strategy_context.in_cross_replica_context():
      self._non_tensor_outputs[name] = output
    else:
      def merge_fn(distribution, value):
        # NOTE(priyag): For non tensor outputs, we simply return all the values
        # in a list as reduction doesn't make sense on non tensors.
        self._non_tensor_outputs[name] = (
            distribution.experimental_local_results(value))
      distribution_strategy_context.get_replica_context().merge_call(
          merge_fn, args=(output,))
