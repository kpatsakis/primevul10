  RaggedFeatureReader(const Tensor& values, const Tensor& row_splits)
      : values_(values.flat<ValuesType>()),
        row_splits_(row_splits.flat<SplitsType>()) {}