void CheckErrorsGPU(OpKernelContext* context, int batch_dim, int seq_dim) {
  const Tensor& input = context->input(0);
  const Tensor& seq_lengths = context->input(1);

  OP_REQUIRES(context, batch_dim != seq_dim,
              errors::InvalidArgument("batch_dim == seq_dim == ", seq_dim));
  OP_REQUIRES(context, seq_dim < input.dims(),
              errors::InvalidArgument("seq_dim must be < input rank", " ( ",
                                      seq_dim, " vs. ", input.dims(), ")"));
  OP_REQUIRES(context, batch_dim < input.dims(),
              errors::InvalidArgument("batch_dim must be < input rank", " ( ",
                                      batch_dim, " vs. ", input.dims(), ")"));

  OP_REQUIRES(
      context, seq_lengths.NumElements() == input.dim_size(batch_dim),
      errors::InvalidArgument("Length of seq_lengths != input.dims(", batch_dim,
                              "), ", "(", seq_lengths.NumElements(), " vs. ",
                              input.dim_size(batch_dim), ")"));
}