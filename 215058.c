void CheckErrors(OpKernelContext* context, int batch_dim, int seq_dim) {
  const Tensor& input = context->input(0);
  const Tensor& seq_lengths = context->input(1);

  auto seq_lens_t = seq_lengths.vec<Tlen>();

  std::vector<Tlen> seq_lens_vec(seq_lens_t.size());

  // Copy seq_len info down for validity checks
  context->eigen_device<Device>().memcpyDeviceToHost(
      seq_lens_vec.data(), seq_lens_t.data(), sizeof(Tlen) * seq_lens_t.size());

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

  for (size_t d = 0; d < seq_lens_vec.size(); ++d) {
    OP_REQUIRES(context, seq_lens_vec[d] >= 0,
                errors::InvalidArgument("seq_lens(", d, ") < 0"));
    OP_REQUIRES(context, seq_lens_vec[d] <= input.dim_size(seq_dim),
                errors::InvalidArgument("seq_lens(", d, ") > input.dims(",
                                        seq_dim, ")"));
  }
}