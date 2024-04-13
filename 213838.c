  explicit UnicodeEncodeOp(OpKernelConstruction* ctx) : OpKernel(ctx) {
    string encoding_tmp;
    OP_REQUIRES_OK(ctx, ctx->GetAttr("output_encoding", &encoding_tmp));
    OP_REQUIRES_OK(ctx, ParseUnicodeEncoding(encoding_tmp, &encoding_));
    OP_REQUIRES_OK(ctx, GetErrorOptions(ctx, &error_options_));
  }