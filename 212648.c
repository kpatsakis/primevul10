  explicit DenseFeatureReader(const Tensor& tensor)
      : values_(tensor.matrix<ValuesType>()),
        feature_count_(tensor.dim_size(1)) {}