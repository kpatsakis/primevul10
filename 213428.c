Status TF_TensorToMaybeAliasedPyArray(Safe_TF_TensorPtr tensor,
                                      PyObject** out_ndarray) {
  auto dtype = TF_TensorType(tensor.get());
  if (dtype == TF_STRING || dtype == TF_RESOURCE) {
    return TF_TensorToPyArray(std::move(tensor), out_ndarray);
  }

  TF_Tensor* moved = tensor.release();
  int64 nelems = -1;
  gtl::InlinedVector<npy_intp, 4> dims;
  TF_RETURN_IF_ERROR(GetPyArrayDimensionsForTensor(moved, &dims, &nelems));
  return ArrayFromMemory(
      dims.size(), dims.data(), TF_TensorData(moved),
      static_cast<DataType>(dtype), [moved] { TF_DeleteTensor(moved); },
      out_ndarray);
}