Status GetPyArrayDimensionsForTensor(const TF_Tensor* tensor,
                                     gtl::InlinedVector<npy_intp, 4>* dims,
                                     tensorflow::int64* nelems) {
  dims->clear();
  const int ndims = TF_NumDims(tensor);
  if (TF_TensorType(tensor) == TF_RESOURCE) {
    if (ndims != 0) {
      return errors::InvalidArgument(
          "Fetching of non-scalar resource tensors is not supported.");
    }
    dims->push_back(TF_TensorByteSize(tensor));
    *nelems = dims->back();
  } else {
    *nelems = 1;
    for (int i = 0; i < ndims; ++i) {
      dims->push_back(TF_Dim(tensor, i));
      *nelems *= dims->back();
    }
  }
  return Status::OK();
}