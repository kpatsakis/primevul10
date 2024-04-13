  static Status BuildDenseFeatureReader(const Tensor& values,
                                        FeatureReaders* features) {
    if (values.dtype() == DT_INT64) {
      features->emplace_back(new DenseFeatureReader<int64>(values));
    } else if (values.dtype() == DT_STRING) {
      features->emplace_back(new DenseFeatureReader<tstring>(values));
    } else {
      return errors::InvalidArgument("Unexpected dtype for input ",
                                     (features->size() + 1), ": ",
                                     values.dtype());
    }
    return Status::OK();
  }