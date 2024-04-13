  explicit UnicodeDecodeBaseOp(OpKernelConstruction* ctx, bool generate_offsets)
      : OpKernel(ctx), generate_offsets_(generate_offsets) {
    OP_REQUIRES_OK(ctx, GetErrorOptions(ctx, &error_options_));
    OP_REQUIRES_OK(ctx, ctx->GetAttr("input_encoding", &input_encoding_));
    // Make a temporary UConverter to ensure it will create without error
    // at execution time (and to warm any data caches the converter needs).
    // This instance is not used.
    std::unique_ptr<WrappedConverter> input_encoder =
        absl::make_unique<WrappedConverter>();
    input_encoder->init(input_encoding_);
    OP_REQUIRES(ctx, input_encoder->converter_,
                errors::InvalidArgument(
                    "Could not create converter for input encoding: " +
                    input_encoding_));
  }