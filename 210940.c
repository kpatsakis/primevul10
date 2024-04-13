  void Compute(OpKernelContext* ctx) override {
    const Tensor* inputs;
    const Tensor* seq_len;
    Tensor* log_prob = nullptr;
    OpOutputList decoded_indices;
    OpOutputList decoded_values;
    OpOutputList decoded_shape;
    OP_REQUIRES_OK(ctx, decode_helper_.ValidateInputsGenerateOutputs(
                            ctx, &inputs, &seq_len, &log_prob, &decoded_indices,
                            &decoded_values, &decoded_shape));

    const TensorShape& inputs_shape = inputs->shape();

    std::vector<typename TTypes<T>::UnalignedConstMatrix> input_list_t;
    const int64 max_time = inputs_shape.dim_size(0);
    const int64 batch_size = inputs_shape.dim_size(1);
    const int64 num_classes_raw = inputs_shape.dim_size(2);
    OP_REQUIRES(
        ctx, FastBoundsCheck(num_classes_raw, std::numeric_limits<int>::max()),
        errors::InvalidArgument("num_classes cannot exceed max int"));
    const int num_classes = static_cast<const int>(num_classes_raw);

    auto inputs_t = inputs->tensor<T, 3>();

    input_list_t.reserve(max_time);
    for (std::size_t t = 0; t < max_time; ++t) {
      input_list_t.emplace_back(inputs_t.data() + t * batch_size * num_classes,
                                batch_size, num_classes);
    }
    auto seq_len_t = seq_len->vec<int32>();
    auto log_prob_t = log_prob->matrix<T>();

    log_prob_t.setZero();

    // Assumption: the blank index is num_classes - 1
    int blank_index = num_classes - 1;

    // Perform best path decoding
    std::vector<std::vector<std::vector<int> > > sequences(batch_size);
    auto decode = [&](const int64 begin, const int64 end) {
      for (int b = begin; b < end; ++b) {
        sequences[b].resize(1);
        auto &sequence = sequences[b][0];
        int prev_indices = -1;
        for (int t = 0; t < seq_len_t(b); ++t) {
          int max_class_indices;
          log_prob_t(b, 0) +=
              -RowMax<T>(input_list_t[t], b, &max_class_indices);
          if (max_class_indices != blank_index &&
              !(merge_repeated_ && max_class_indices == prev_indices)) {
            sequence.push_back(max_class_indices);
          }
          prev_indices = max_class_indices;
        }
      }
    };

    const int64 kCostPerUnit = 50 * max_time * num_classes;
    const int64 total = batch_size;
    const DeviceBase::CpuWorkerThreads& worker_threads =
        *ctx->device()->tensorflow_cpu_worker_threads();
    Shard(worker_threads.num_threads, worker_threads.workers, total,
          kCostPerUnit, decode);

    OP_REQUIRES_OK(
        ctx, decode_helper_.StoreAllDecodedSequences(
                 sequences, &decoded_indices, &decoded_values, &decoded_shape));
  }