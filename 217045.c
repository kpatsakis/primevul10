inline bool SparseTensor::ValidateAndInitializeToDense(Tensor* out,
                                                       bool initialize) {
  DCHECK_EQ(DataTypeToEnum<T>::v(), dtype())
      << "ToDense requested with the wrong datatype";

  DCHECK_EQ(out->shape().dims(), dims_)
      << "Incompatible dimensions between SparseTensor and output";

  DCHECK_EQ(out->dtype(), DataTypeToEnum<T>::v())
      << "Output must be type: " << DataTypeToEnum<T>::v()
      << " but got: " << out->dtype();

  // Make sure the dense output is the same rank and has room
  // to hold the SparseTensor.
  const auto& out_shape = out->shape();
  if (shape_.size() != out_shape.dims()) return false;
  for (int d = 0; d < shape_.size(); ++d) {
    if (shape_[d] > out_shape.dim_size(d)) return false;
  }

  if (initialize) {
    auto out_t = out->flat<T>();
    out_t.setConstant(T());
  }

  return true;
}