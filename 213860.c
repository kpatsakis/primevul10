  explicit CropAndResizeGradImageOp(OpKernelConstruction* context)
      : AsyncOpKernel(context) {
    OP_REQUIRES_OK(context, context->GetAttr("method", &method_));
    OP_REQUIRES(context, method_ == "bilinear" || method_ == "nearest",
                errors::InvalidArgument(
                    "method must be 'bilinear' or 'nearest'", method_));
  }