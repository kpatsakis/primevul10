Status CopyTF_TensorStringsToPyArray(const TF_Tensor* src, uint64 nelems,
                                     PyArrayObject* dst) {
  const void* tensor_data = TF_TensorData(src);
  DCHECK(tensor_data != nullptr);
  DCHECK_EQ(TF_STRING, TF_TensorType(src));

  const tstring* tstr = static_cast<const tstring*>(tensor_data);

  std::unique_ptr<TF_Status, decltype(&TF_DeleteStatus)> status(
      TF_NewStatus(), TF_DeleteStatus);
  auto iter = make_safe(PyArray_IterNew(reinterpret_cast<PyObject*>(dst)));
  for (int64 i = 0; i < static_cast<int64>(nelems); ++i) {
    const tstring& tstr_i = tstr[i];
    auto py_string =
        make_safe(PyBytes_FromStringAndSize(tstr_i.data(), tstr_i.size()));
    if (py_string == nullptr) {
      return errors::Internal(
          "failed to create a python byte array when converting element #", i,
          " of a TF_STRING tensor to a numpy ndarray");
    }

    if (PyArray_SETITEM(dst, static_cast<char*>(PyArray_ITER_DATA(iter.get())),
                        py_string.get()) != 0) {
      return errors::Internal("Error settings element #", i,
                              " in the numpy ndarray");
    }
    PyArray_ITER_NEXT(iter.get());
  }
  return Status::OK();
}