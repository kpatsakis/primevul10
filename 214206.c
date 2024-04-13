  explicit MaxPoolingNoMaskV2Op(OpKernelConstruction* context)
      : OpKernel(context) {
    string data_format;
    OP_REQUIRES_OK(context, context->GetAttr("data_format", &data_format));
    OP_REQUIRES(context, FormatFromString(data_format, &data_format_),
                errors::InvalidArgument("Invalid data format"));
    if (context->num_inputs() == 1) {
      OP_REQUIRES_OK(context, context->GetAttr("ksize", &ksize_));
      OP_REQUIRES(context, ksize_.size() == 4,
                  errors::InvalidArgument("Sliding window ksize field must "
                                          "specify 4 dimensions"));
      OP_REQUIRES_OK(context, context->GetAttr("strides", &stride_));
      OP_REQUIRES(context, stride_.size() == 4,
                  errors::InvalidArgument("Sliding window stride field must "
                                          "specify 4 dimensions"));
      const int32 ksize_n = GetTensorDim(ksize_, data_format_, 'N');
      const int32 stride_n = GetTensorDim(stride_, data_format_, 'N');
      OP_REQUIRES(context, ksize_n == 1 && stride_n == 1,
                  errors::Unimplemented(
                      "Pooling is not yet supported on the batch dimension."));
    }
    OP_REQUIRES_OK(context, context->GetAttr("padding", &padding_));
    TF_CHECK_OK(ReadBoolFromEnvVar("TF_ENABLE_MAXPOOL_NANPROP", false,
                                   &propagate_nans_));
  }