  void ValidateInputTensors(OpKernelContext* ctx, const Tensor& in0,
                            const Tensor& in1) {
    OP_REQUIRES(
        ctx, in0.dims() >= 2,
        errors::InvalidArgument("In[0] ndims must be >= 2: ", in0.dims()));

    OP_REQUIRES(
        ctx, in1.dims() >= 2,
        errors::InvalidArgument("In[1] ndims must be >= 2: ", in1.dims()));

    OP_REQUIRES(ctx, in0.NumElements() > 0,
                errors::InvalidArgument("In[0] must not be an empty tensor: ",
                                        in0.DebugString()));

    OP_REQUIRES(ctx, in1.NumElements() > 0,
                errors::InvalidArgument("In[1] must not be an empty tensor: ",
                                        in1.DebugString()));
  }