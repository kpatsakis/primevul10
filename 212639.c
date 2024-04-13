  Status ValidateInput(const OpInputList& ragged_values_list,
                       const OpInputList& ragged_splits_list,
                       const OpInputList& sparse_indices_list,
                       const OpInputList& sparse_values_list,
                       const OpInputList& sparse_shape_list,
                       const OpInputList& dense_list) {
    const auto num_ragged = ragged_values_list.size();
    const auto num_sparse = sparse_indices_list.size();

    // Validate tensor shapes.
    for (int i = 0; i < num_ragged; ++i) {
      if (!TensorShapeUtils::IsVector(ragged_values_list[i].shape())) {
        return errors::InvalidArgument(
            "tf.ragged.cross only supports inputs with rank=2.");
      }
      if (!TensorShapeUtils::IsVector(ragged_splits_list[i].shape()) ||
          (ragged_splits_list[i].NumElements() == 0)) {
        return errors::InvalidArgument("Invalid RaggedTensor");
      }
    }
    for (int i = 0; i < num_sparse; ++i) {
      if (!TensorShapeUtils::IsMatrix(sparse_indices_list[i].shape()) ||
          !TensorShapeUtils::IsVector(sparse_values_list[i].shape()) ||
          !TensorShapeUtils::IsVector(sparse_shape_list[i].shape())) {
        return errors::InvalidArgument("Invalid SparseTensor ", i);
      }
      if (sparse_shape_list[i].NumElements() != 2) {
        return errors::InvalidArgument(
            "tf.ragged.cross only supports inputs with rank=2.");
      }
    }
    for (int i = 0; i < dense_list.size(); ++i) {
      if (!TensorShapeUtils::IsMatrix(dense_list[i].shape())) {
        return errors::InvalidArgument(
            "tf.ragged.cross only supports inputs with rank=2.");
      }
    }

    // Check that batch sizes are consistent.
    int64 batch_size =
        CalculateBatchSize(ragged_splits_list, sparse_shape_list, dense_list);
    for (int i = 0; i < num_ragged; ++i) {
      if (ragged_splits_list[i].NumElements() - 1 != batch_size) {
        return errors::InvalidArgument(
            "inputs must all have the same batch dimension size.");
      }
    }
    for (int i = 0; i < num_sparse; ++i) {
      if (sparse_shape_list[i].flat<int64>()(0) != batch_size) {
        return errors::InvalidArgument(
            "inputs must all have the same batch dimension size.");
      }
    }
    for (int i = 0; i < dense_list.size(); ++i) {
      if (dense_list[i].dim_size(0) != batch_size) {
        return errors::InvalidArgument(
            "inputs must all have the same batch dimension size.");
      }
    }

    return Status::OK();
  }