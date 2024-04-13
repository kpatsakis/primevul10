  explicit UnicodeDecodeWithOffsetsOp(OpKernelConstruction* ctx)
      : UnicodeDecodeBaseOp<SPLITS_TYPE>(ctx, true) {}