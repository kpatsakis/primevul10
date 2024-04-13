  explicit UnicodeTranscodeOp(OpKernelConstruction* ctx) : OpKernel(ctx) {
    OP_REQUIRES_OK(ctx, GetErrorOptions(ctx, &error_options_));

    string output_encoding;
    OP_REQUIRES_OK(ctx, ctx->GetAttr("output_encoding", &output_encoding));
    OP_REQUIRES_OK(ctx,
                   ParseUnicodeEncoding(output_encoding, &output_encoding_));

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