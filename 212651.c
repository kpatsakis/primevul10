  SparseFeatureReader(const Tensor& indices_t, const Tensor& values_t,
                      int64 batch_size)
      : values_(values_t.flat<ValuesType>()) {
    row_splits_.reserve(batch_size + 1);
    row_splits_.push_back(0);
    auto indices = indices_t.matrix<int64>();
    int64 num_values = values_.size();
    int64 i = 0;  // value index
    for (int row = 0; row < batch_size; row++) {
      while (i < num_values && indices(i, 0) <= row) ++i;
      row_splits_.push_back(i);
    }
  }