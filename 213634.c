  void DoFFT(OpKernelContext* ctx, const Tensor& in, uint64* fft_shape,
             Tensor* out) override {
    auto* stream = ctx->op_device_context()->stream();
    OP_REQUIRES(ctx, stream, errors::Internal("No GPU stream available."));

    const TensorShape& input_shape = in.shape();
    const TensorShape& output_shape = out->shape();

    const int fft_rank = Rank();
    int batch_size = 1;
    for (int i = 0; i < input_shape.dims() - fft_rank; ++i) {
      batch_size *= input_shape.dim_size(i);
    }
    uint64 input_embed[3];
    const uint64 input_stride = 1;
    uint64 input_distance = 1;
    uint64 output_embed[3];
    const uint64 output_stride = 1;
    uint64 output_distance = 1;

    for (int i = 0; i < fft_rank; ++i) {
      auto dim_offset = input_shape.dims() - fft_rank + i;
      input_embed[i] = input_shape.dim_size(dim_offset);
      input_distance *= input_shape.dim_size(dim_offset);
      output_embed[i] = output_shape.dim_size(dim_offset);
      output_distance *= output_shape.dim_size(dim_offset);
    }

    constexpr bool kInPlaceFft = false;
    const bool is_complex128 =
        in.dtype() == DT_COMPLEX128 || out->dtype() == DT_COMPLEX128;

    const auto kFftType =
        IsReal()
            ? (IsForward()
                   ? (is_complex128 ? se::fft::Type::kD2Z : se::fft::Type::kR2C)
                   : (is_complex128 ? se::fft::Type::kZ2D
                                    : se::fft::Type::kC2R))
            : (IsForward() ? (is_complex128 ? se::fft::Type::kZ2ZForward
                                            : se::fft::Type::kC2CForward)
                           : (is_complex128 ? se::fft::Type::kZ2ZInverse
                                            : se::fft::Type::kC2CInverse));

    CufftScratchAllocator scratch_allocator(CufftScratchSize, ctx);
    auto plan =
        stream->parent()->AsFft()->CreateBatchedPlanWithScratchAllocator(
            stream, fft_rank, fft_shape, input_embed, input_stride,
            input_distance, output_embed, output_stride, output_distance,
            kFftType, kInPlaceFft, batch_size, &scratch_allocator);

    if (IsReal()) {
      if (IsForward()) {
        if (is_complex128) {
          DCHECK_EQ(in.dtype(), DT_DOUBLE);
          DCHECK_EQ(out->dtype(), DT_COMPLEX128);
          DoFFTInternal<double, complex128>(ctx, stream, plan.get(), kFftType,
                                            output_distance, in, out);
        } else {
          DCHECK_EQ(in.dtype(), DT_FLOAT);
          DCHECK_EQ(out->dtype(), DT_COMPLEX64);
          DoFFTInternal<float, complex64>(ctx, stream, plan.get(), kFftType,
                                          output_distance, in, out);
        }
      } else {
        if (is_complex128) {
          DCHECK_EQ(in.dtype(), DT_COMPLEX128);
          DCHECK_EQ(out->dtype(), DT_DOUBLE);
          DoFFTInternal<complex128, double>(ctx, stream, plan.get(), kFftType,
                                            output_distance, in, out);
        } else {
          DCHECK_EQ(in.dtype(), DT_COMPLEX64);
          DCHECK_EQ(out->dtype(), DT_FLOAT);
          DoFFTInternal<complex64, float>(ctx, stream, plan.get(), kFftType,
                                          output_distance, in, out);
        }
      }
    } else {
      if (is_complex128) {
        DCHECK_EQ(in.dtype(), DT_COMPLEX128);
        DCHECK_EQ(out->dtype(), DT_COMPLEX128);
        DoFFTInternal<complex128, complex128>(ctx, stream, plan.get(), kFftType,
                                              output_distance, in, out);
      } else {
        DCHECK_EQ(in.dtype(), DT_COMPLEX64);
        DCHECK_EQ(out->dtype(), DT_COMPLEX64);
        DoFFTInternal<complex64, complex64>(ctx, stream, plan.get(), kFftType,
                                            output_distance, in, out);
      }
    }
  }