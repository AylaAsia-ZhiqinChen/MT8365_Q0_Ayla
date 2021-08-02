/* Copyright 2017 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

// These functions transform Python/Numpy data structures to XLA data
// structures and vice versa, performing copies where
// appropriate. Python tuples and Numpy ndarrays translate to XLA
// tuples and XLA literals, respectively, and Numpy shape/dtype
// information is translated to XLA shape information.

#ifndef TENSORFLOW_COMPILER_XLA_PYTHON_NUMPY_BRIDGE_H_
#define TENSORFLOW_COMPILER_XLA_PYTHON_NUMPY_BRIDGE_H_

#include <algorithm>
#include <memory>

#include "absl/types/span.h"
#include "tensorflow/compiler/xla/literal.h"
#include "tensorflow/compiler/xla/xla_data.pb.h"
#include "tensorflow/python/lib/core/numpy.h"

namespace xla {

namespace swig {

namespace numpy {

struct PyDecrefDeleter {
  void operator()(PyObject* p) const { Py_DECREF(p); }
};

// Safe container for an owned PyObject. On destruction, the reference count of
// the contained object will be decremented.
using Safe_PyObjectPtr = std::unique_ptr<PyObject, PyDecrefDeleter>;

Safe_PyObjectPtr make_safe(PyObject* object);

// Maps XLA primitive types (PRED, S8, F32, ..., and TUPLE) to numpy
// dtypes (NPY_BOOL, NPY_INT8, NPY_FLOAT32, ..., and NPY_OBJECT), and
// vice versa.
int PrimitiveTypeToNumpyType(PrimitiveType primitive_type);
PrimitiveType NumpyTypeToPrimitiveType(int np_type);

// Determines whether an integer-encoded Numpy dtype is valid,
// i.e. has a supported conversion to an XLA PrimitiveType.
bool NumpyTypeIsValid(int np_type);

// Converts XLA shape information into a Python pair of the form
// (numpy dtype, dimensions). If the XLA shape represents a tuple,
// then the numpy dtype is NPY_OBJECT ('O') and `dimensions` is a
// Python tuple of shape-description pairs, created
// recursively. Otherwise, `dimensions` is a Python tuple-of-integers
// providing the array dimensions.
//
// The return value is a new reference.
Safe_PyObjectPtr PyShapeInfoFromXlaShape(const Shape& shape);

// Returns a pair of (arg_shapes, result_shape), where arg_shapes is a tuple
// of argument shapes and result_shape is the result shape. Each shape is as
// described in in PyShapeInfoFromXlaShape's comment.
Safe_PyObjectPtr PyProgramShapeInfoFromXlaProgramShape(
    const ProgramShape& shape);

// Converts a Python object with a method interface mathing that of
// xla_client.Shape into an XLA Shape object.
//
// The return value is a new reference.
StatusOr<Shape> XlaShapeFromPyShape(PyObject* o);

// Converts a PyObject that represents operation metadata into protocol buffer
// form.
StatusOr<OpMetadata> OpMetadataFromPyObject(PyObject* o);

// Converts an XLA literal to a Python object, either a Numpy ndarray
// or a nested Python tuple thereof.
//
// To avoid transferring ownership of the data buffers that underlie
// PyArrays and XLA literals, this function makes deep copies of all
// array data.
//
// The return value is a new reference.
StatusOr<Safe_PyObjectPtr> PyObjectFromXlaLiteral(const LiteralSlice& literal);

// Converts a Numpy ndarray or a nested Python tuple thereof to a
// corresponding XLA literal.
//
// To avoid transferring ownership of the data buffers that underlie
// PyArrays and XLA literals, this function makes deep copies of all
// array data.
StatusOr<Literal> XlaLiteralFromPyObject(PyObject* o);

// The following functions copy array data from the buffers underlying Numpy
// ndarrays into those underlying XLA literals, and vice versa.

Status CopyNumpyArrayToLiteral(int np_type, PyArrayObject* py_array,
                               Literal* literal);

Status CopyLiteralToNumpyArray(int np_type, const LiteralSlice& literal,
                               PyArrayObject* py_array);

template <typename NativeT>
void CopyNumpyArrayToLiteral(PyArrayObject* py_array, Literal* literal) {
  NativeT* source = static_cast<NativeT*>(PyArray_DATA(py_array));
  auto dest = literal->data<NativeT>();
  std::copy(source, source + PyArray_SIZE(py_array), dest.data());
}

template <typename NativeT>
void CopyLiteralToNumpyArray(const LiteralSlice& literal,
                             PyArrayObject* py_array) {
  NativeT* dest = static_cast<NativeT*>(PyArray_DATA(py_array));
  auto source = literal.data<NativeT>();
  std::copy(source.begin(), source.end(), dest);
}

// Safely returns a repr of the given Python object o as a C++ string.
string PyObjectCppRepr(PyObject* o);

// Workarounds for Python 2 and 3 interop

PyObject* LongToPyIntOrPyLong(long x);  // NOLINT
long PyIntOrPyLongToLong(PyObject* o);  // NOLINT
bool CheckPyIntOrLong(PyObject* o);
PyObject* PyNumberToPyInt(PyObject* o);

}  // namespace numpy

// Miscellaneous swig helpers that don't have a better home.

bool GetIntAttr(PyObject* o, const char* field, int64* result);

// Returns "ok"; true if there is no error, false if there was an error.
bool HandleStringAttribute(PyObject* o, const char* attr_name,
                           std::function<void(string s)> f);
bool HandleBoolAttribute(PyObject* o, const char* attr_name,
                         std::function<void(bool b)> f);

bool HandleRepeatedInt64Attribute(
    PyObject* o, const char* attr_name,
    tensorflow::protobuf::RepeatedField<tensorflow::protobuf_int64>* field);

}  // namespace swig

}  // namespace xla

#endif  // TENSORFLOW_COMPILER_XLA_PYTHON_NUMPY_BRIDGE_H_
