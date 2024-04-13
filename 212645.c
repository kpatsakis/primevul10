  Status BuildOutputTensors(const FeatureReaders& features, int64 batch_size,
                            OpKernelContext* context, Tensor** values_out,
                            Tensor** row_splits_out) {
    // Allocate and populate the row_splits output tensor.
    TF_RETURN_IF_ERROR(context->allocate_output(
        1, TensorShape({batch_size + 1}), row_splits_out));
    auto flat_row_splits = (*row_splits_out)->flat<SplitsType>();
    int64 cross_count_total = 0;
    flat_row_splits(0) = 0;
    for (int64 b = 0; b < batch_size; b++) {
      cross_count_total += CrossCountByBatchIndex(features, b);
      flat_row_splits(b + 1) = cross_count_total;
    }

    // Allocate the values output tensor.
    TF_RETURN_IF_ERROR(context->allocate_output(
        0, TensorShape({cross_count_total}), values_out));

    return Status::OK();
  }