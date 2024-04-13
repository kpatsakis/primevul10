Status TensorToNdarray(const Tensor& t, PyObject** ret) {
  Status status;
  Safe_TF_TensorPtr tf_tensor = make_safe(TF_TensorFromTensor(t, &status));
  if (!status.ok()) {
    return status;
  }
  return TF_TensorToPyArray(std::move(tf_tensor), ret);
}