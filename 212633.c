  void Compute(OpKernelContext* context) override {
    OpInputList ragged_values_list;
    OpInputList ragged_splits_list;
    OpInputList sparse_indices_list;
    OpInputList sparse_values_list;
    OpInputList sparse_shape_list;
    OpInputList dense_list;
    OP_REQUIRES_OK(context,
                   context->input_list("ragged_values", &ragged_values_list));
    OP_REQUIRES_OK(
        context, context->input_list("ragged_row_splits", &ragged_splits_list));
    OP_REQUIRES_OK(context,
                   context->input_list("sparse_indices", &sparse_indices_list));
    OP_REQUIRES_OK(context,
                   context->input_list("sparse_values", &sparse_values_list));
    OP_REQUIRES_OK(context,
                   context->input_list("sparse_shape", &sparse_shape_list));
    OP_REQUIRES_OK(context, context->input_list("dense_inputs", &dense_list));
    OP_REQUIRES_OK(context,
                   ValidateInput(ragged_values_list, ragged_splits_list,
                                 sparse_indices_list, sparse_values_list,
                                 sparse_shape_list, dense_list));

    int64 batch_size =
        CalculateBatchSize(ragged_splits_list, sparse_shape_list, dense_list);

    FeatureReaders features;
    OP_REQUIRES_OK(context,
                   BuildFeatureReaders(ragged_values_list, ragged_splits_list,
                                       sparse_indices_list, sparse_values_list,
                                       dense_list, batch_size, &features));

    Tensor* values_out;
    Tensor* row_splits_out;
    OP_REQUIRES_OK(context, BuildOutputTensors(features, batch_size, context,
                                               &values_out, &row_splits_out));

    std::unique_ptr<OutputWriter> output_writer = MakeOutputWriter(
        features, num_buckets_, hash_key_, row_splits_out, values_out);

    auto do_work = [&output_writer](int64 begin, int64 end) {
      output_writer->WriteOutputSlice(begin, end);
    };

    // TODO(edloper): optimize cost_per_batch
    const int cost_per_batch = 5000 * ragged_values_list.size();
    auto thread_pool =
        context->device()->tensorflow_cpu_worker_threads()->workers;
    thread_pool->ParallelFor(batch_size, cost_per_batch, do_work);
  }