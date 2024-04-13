  static Status BuildRaggedFeatureReader(const Tensor& values,
                                         const Tensor& splits,
                                         FeatureReaders* features) {
    if (values.dtype() != DT_INT64 && values.dtype() != DT_STRING) {
      return errors::InvalidArgument("Unexpected dtype for input ",
                                     (features->size() + 1), ": ",
                                     values.dtype());
    }
    if (splits.dtype() != DT_INT64 && splits.dtype() != DT_INT32) {
      return errors::InvalidArgument("Unexpected row_splits.dtype for input ",
                                     (features->size() + 1), ": ",
                                     values.dtype());
    }
    if (values.dtype() == DT_INT64) {
      if (splits.dtype() == DT_INT64) {
        features->emplace_back(
            new RaggedFeatureReader<int64, int64>(values, splits));
      } else {
        features->emplace_back(
            new RaggedFeatureReader<int64, int32>(values, splits));
      }
    } else {
      if (splits.dtype() == DT_INT64) {
        features->emplace_back(
            new RaggedFeatureReader<tstring, int64>(values, splits));
      } else {
        features->emplace_back(
            new RaggedFeatureReader<tstring, int32>(values, splits));
      }
    }
    return Status::OK();
  }