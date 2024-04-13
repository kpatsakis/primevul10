  explicit EncodePngOp(OpKernelConstruction* context) : OpKernel(context) {
    OP_REQUIRES_OK(context, context->GetAttr("compression", &compression_));
    OP_REQUIRES(context, -1 <= compression_ && compression_ <= 9,
                errors::InvalidArgument("compression should be in [-1,9], got ",
                                        compression_));

    DataType dt = context->input_type(0);
    OP_REQUIRES(context, dt == DataType::DT_UINT8 || dt == DataType::DT_UINT16,
                errors::InvalidArgument(
                    "image must have type uint8 or uint16, got ", dt));

    if (dt == DataType::DT_UINT8) {
      desired_channel_bits_ = 8;
    } else {
      desired_channel_bits_ = 16;
    }
  }