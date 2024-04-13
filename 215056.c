  void Compute(OpKernelContext* context) override {
    const Tensor& input = context->input(0);
    const Tensor& seq_lengths = context->input(1);

    // Preliminary validation of sizes.
    OP_REQUIRES(context, TensorShapeUtils::IsVector(seq_lengths.shape()),
                errors::InvalidArgument("seq_lengths must be 1-dim, not ",
                                        seq_lengths.dims()));

    auto seq_lens_t = seq_lengths.vec<Tlen>();

    CheckErrors<Device, Tlen>(context, batch_dim_, seq_dim_);
    if (!context->status().ok()) return;

    const int input_dims = input.dims();

    Tensor* output = nullptr;
    OP_REQUIRES_OK(context,
                   context->allocate_output(0, input.shape(), &output));

#define HANDLE_DIM(NDIM)                                                      \
  case NDIM:                                                                  \
    functor::ReverseSequence<Device, T, Tlen, NDIM>::Compute(                 \
        context->eigen_device<Device>(), input.tensor<T, NDIM>(), batch_dim_, \
        seq_dim_, seq_lens_t, output->tensor<T, NDIM>());                     \
    break;

    switch (input_dims) {
      HANDLE_DIM(2);
      HANDLE_DIM(3);
      HANDLE_DIM(4);
      HANDLE_DIM(5);

      default:
        OP_REQUIRES(context, false,
                    errors::InvalidArgument(
                        "ReverseSequenceOp : Unhandled input dimensions: ",
                        input_dims));
    }
  }