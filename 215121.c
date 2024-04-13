  explicit LoadAndRemapMatrixOp(OpKernelConstruction* context)
      : OpKernel(context) {
    OP_REQUIRES_OK(context, context->GetAttr("num_rows", &num_rows_));
    OP_REQUIRES_OK(context, context->GetAttr("num_cols", &num_cols_));
    OP_REQUIRES_OK(
        context, context->GetAttr("max_rows_in_memory", &max_rows_in_memory_));
  }