  static Status BuildSparseFeatureReader(const Tensor& indices,
                                         const Tensor& values, int64 batch_size,
                                         FeatureReaders* features) {
    if (values.dtype() == DT_INT64) {
      features->emplace_back(
          new SparseFeatureReader<int64>(indices, values, batch_size));
    } else if (values.dtype() == DT_STRING) {
      features->emplace_back(
          new SparseFeatureReader<tstring>(indices, values, batch_size));
    } else {
      return errors::InvalidArgument("Unexpected dtype for input ",
                                     (features->size() + 1), ": ",
                                     values.dtype());
    }
    return Status::OK();
  }