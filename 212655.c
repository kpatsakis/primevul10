  int64 CalculateBatchSize(const OpInputList& ragged_splits_list,
                           const OpInputList& sparse_shape_list,
                           const OpInputList& dense_list) {
    if (ragged_splits_list.size() > 0) {
      return ragged_splits_list[0].NumElements() - 1;
    } else if (dense_list.size() > 0) {
      return dense_list[0].dim_size(0);
    } else if (sparse_shape_list.size() > 0) {
      return sparse_shape_list[0].flat<int64>()(0);
    } else {
      return 0;
    }
  }