Status NdarrayToTensor(TFE_Context* ctx, PyObject* ndarray,
                       Safe_TF_TensorPtr* ret) {
  DCHECK(ret != nullptr);

  // Make sure we dereference this array object in case of error, etc.
  Safe_PyObjectPtr array_safe(make_safe(
      PyArray_FromAny(ndarray, nullptr, 0, 0, NPY_ARRAY_CARRAY_RO, nullptr)));
  if (!array_safe) return errors::InvalidArgument("Not a ndarray.");
  PyArrayObject* array = reinterpret_cast<PyArrayObject*>(array_safe.get());

  // Convert numpy dtype to TensorFlow dtype.
  TF_DataType dtype = TF_FLOAT;
  TF_RETURN_IF_ERROR(PyArray_TYPE_to_TF_DataType(array, &dtype));

  tensorflow::int64 nelems = 1;
  gtl::InlinedVector<int64_t, 4> dims;
  for (int i = 0; i < PyArray_NDIM(array); ++i) {
    dims.push_back(PyArray_SHAPE(array)[i]);
    nelems *= dims[i];
  }

  // Create a TF_Tensor based on the fed data. In the case of non-string data
  // type, this steals a reference to array, which will be relinquished when
  // the underlying buffer is deallocated. For string, a new temporary buffer
  // is allocated into which the strings are encoded.
  if (dtype == TF_RESOURCE) {
    size_t size = PyArray_NBYTES(array);
    array_safe.release();

    if (ctx) {
      *ret = make_safe(new TF_Tensor{tensorflow::unwrap(ctx)->CreateTensor(
          static_cast<tensorflow::DataType>(dtype), {}, 0, PyArray_DATA(array),
          size, &DelayedNumpyDecref, array)});
    } else {
      *ret = make_safe(TF_NewTensor(dtype, {}, 0, PyArray_DATA(array), size,
                                    &DelayedNumpyDecref, array));
    }

  } else if (dtype != TF_STRING) {
    size_t size = PyArray_NBYTES(array);
    array_safe.release();
    if (ctx) {
      *ret = make_safe(new TF_Tensor{tensorflow::unwrap(ctx)->CreateTensor(
          static_cast<tensorflow::DataType>(dtype), dims.data(), dims.size(),
          PyArray_DATA(array), size, &DelayedNumpyDecref, array)});
    } else {
      *ret = make_safe(TF_NewTensor(dtype, dims.data(), dims.size(),
                                    PyArray_DATA(array), size,
                                    &DelayedNumpyDecref, array));
    }

  } else {
    size_t size = 0;
    void* encoded = nullptr;
    TF_RETURN_IF_ERROR(EncodePyBytesArray(array, nelems, &size, &encoded));
    if (ctx) {
      *ret = make_safe(new TF_Tensor{tensorflow::unwrap(ctx)->CreateTensor(
          static_cast<tensorflow::DataType>(dtype), dims.data(), dims.size(),
          encoded, size,
          [](void* data, size_t len, void* arg) {
            delete[] reinterpret_cast<tensorflow::tstring*>(data);
          },
          nullptr)});
    } else {
      *ret = make_safe(TF_NewTensor(
          dtype, dims.data(), dims.size(), encoded, size,
          [](void* data, size_t len, void* arg) {
            delete[] reinterpret_cast<tensorflow::tstring*>(data);
          },
          nullptr));
    }
  }

  return Status::OK();
}