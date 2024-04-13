  void DoFFT(OpKernelContext* ctx, const Tensor& in, uint64* fft_shape,
             Tensor* out) override {
    // Create the axes (which are always trailing).
    const auto axes = Eigen::ArrayXi::LinSpaced(FFTRank, 1, FFTRank);
    auto device = ctx->eigen_device<CPUDevice>();

    const bool is_complex128 =
        in.dtype() == DT_COMPLEX128 || out->dtype() == DT_COMPLEX128;

    if (!IsReal()) {
      // Compute the FFT using Eigen.
      constexpr auto direction =
          Forward ? Eigen::FFT_FORWARD : Eigen::FFT_REVERSE;
      if (is_complex128) {
        DCHECK_EQ(in.dtype(), DT_COMPLEX128);
        DCHECK_EQ(out->dtype(), DT_COMPLEX128);
        auto input = Tensor(in).flat_inner_dims<complex128, FFTRank + 1>();
        auto output = out->flat_inner_dims<complex128, FFTRank + 1>();
        output.device(device) =
            input.template fft<Eigen::BothParts, direction>(axes);
      } else {
        DCHECK_EQ(in.dtype(), DT_COMPLEX64);
        DCHECK_EQ(out->dtype(), DT_COMPLEX64);
        auto input = Tensor(in).flat_inner_dims<complex64, FFTRank + 1>();
        auto output = out->flat_inner_dims<complex64, FFTRank + 1>();
        output.device(device) =
            input.template fft<Eigen::BothParts, direction>(axes);
      }
    } else {
      if (IsForward()) {
        if (is_complex128) {
          DCHECK_EQ(in.dtype(), DT_DOUBLE);
          DCHECK_EQ(out->dtype(), DT_COMPLEX128);
          DoRealForwardFFT<double, complex128>(ctx, fft_shape, in, out);
        } else {
          DCHECK_EQ(in.dtype(), DT_FLOAT);
          DCHECK_EQ(out->dtype(), DT_COMPLEX64);
          DoRealForwardFFT<float, complex64>(ctx, fft_shape, in, out);
        }
      } else {
        if (is_complex128) {
          DCHECK_EQ(in.dtype(), DT_COMPLEX128);
          DCHECK_EQ(out->dtype(), DT_DOUBLE);
          DoRealBackwardFFT<complex128, double>(ctx, fft_shape, in, out);
        } else {
          DCHECK_EQ(in.dtype(), DT_COMPLEX64);
          DCHECK_EQ(out->dtype(), DT_FLOAT);
          DoRealBackwardFFT<complex64, float>(ctx, fft_shape, in, out);
        }
      }
    }
  }