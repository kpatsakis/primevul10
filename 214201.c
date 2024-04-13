  explicit QuantizeAndDequantizeV4GradientOp(OpKernelConstruction* ctx)
      : OpKernel::OpKernel(ctx) {
    OP_REQUIRES_OK(ctx, ctx->GetAttr("axis", &axis_));
  }