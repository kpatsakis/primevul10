  void DoFFTInternal(OpKernelContext* ctx, se::Stream* stream,
                     se::fft::Plan* plan, const se::fft::Type fft_type,
                     const uint64 output_distance, const Tensor& in,
                     Tensor* out) {
    const TensorShape& input_shape = in.shape();
    const TensorShape& output_shape = out->shape();
    auto src =
        AsDeviceMemory<InT>(in.flat<InT>().data(), input_shape.num_elements());
    auto dst = AsDeviceMemory<OutT>(out->flat<OutT>().data(),
                                    output_shape.num_elements());
    OP_REQUIRES(
        ctx, stream->ThenFft(plan, src, &dst).ok(),
        errors::Internal("fft failed : type=", static_cast<int>(fft_type),
                         " in.shape=", input_shape.DebugString()));
    if (!IsForward()) {
      typedef typename RealTypeFromComplexType<OutT>::RealT RealT;
      RealT alpha = 1.0 / output_distance;
      OP_REQUIRES(
          ctx,
          stream->ThenBlasScal(output_shape.num_elements(), alpha, &dst, 1)
              .ok(),
          errors::Internal("BlasScal failed : in.shape=",
                           input_shape.DebugString()));
    }
  }