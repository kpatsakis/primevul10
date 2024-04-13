  explicit RaggedCrossOp(OpKernelConstruction* context) : OpKernel(context) {
    OP_REQUIRES_OK(context, context->GetAttr("num_buckets", &num_buckets_));
    // Read signed_hash_key_ as int64 since uint64 attributes are not
    // supported by REGISTER_OP.
    int64 signed_hash_key_;
    OP_REQUIRES_OK(context, context->GetAttr("hash_key", &signed_hash_key_));
    hash_key_ = static_cast<uint64>(signed_hash_key_);

    int num_sparse;
    OP_REQUIRES_OK(context, context->GetAttr("Nsparse", &num_sparse));

    OP_REQUIRES_OK(context, context->GetAttr("ragged_values_types",
                                             &ragged_values_types_));
    OP_REQUIRES_OK(context, context->GetAttr("ragged_splits_types",
                                             &ragged_splits_types_));
    OP_REQUIRES_OK(context, context->GetAttr("sparse_values_types",
                                             &sparse_values_types_));
    OP_REQUIRES_OK(context, context->GetAttr("dense_types", &dense_types_));
    OP_REQUIRES_OK(context, context->GetAttr("input_order", &input_order_));
    OP_REQUIRES(context,
                ragged_values_types_.size() == ragged_splits_types_.size(),
                errors::InvalidArgument(
                    "ragged values and splits must have the same length"));
    OP_REQUIRES(context, num_sparse == sparse_values_types_.size(),
                errors::InvalidArgument(
                    "sparse indices and values must have the same length"));
    OP_REQUIRES(context,
                ragged_values_types_.size() + sparse_values_types_.size() +
                        dense_types_.size() ==
                    input_order_.size(),
                errors::InvalidArgument("Invalid length for input_order"));
  }