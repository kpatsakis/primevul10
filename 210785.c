  void DoRealBackwardFFT(OpKernelContext* ctx, uint64* fft_shape,
                         const Tensor& in, Tensor* out) {
    auto device = ctx->eigen_device<CPUDevice>();
    // Reconstruct the full FFT and take the inverse.
    auto input = Tensor(in).flat_inner_dims<ComplexT, FFTRank + 1>();
    auto output = out->flat_inner_dims<RealT, FFTRank + 1>();
    const auto input_dims = input.dimensions();

    // Calculate the shape of the temporary tensor for the full FFT and the
    // region we will slice from input given fft_shape. We slice input to
    // fft_shape on its inner-most dimensions, except the last (which we
    // slice to fft_shape[-1] / 2 + 1).
    Eigen::DSizes<Eigen::DenseIndex, FFTRank + 1> input_slice_sizes;
    input_slice_sizes[0] = input_dims[0];
    TensorShape full_fft_shape;
    full_fft_shape.AddDim(input_dims[0]);
    for (auto i = 1; i <= FFTRank; i++) {
      input_slice_sizes[i] =
          i == FFTRank ? fft_shape[i - 1] / 2 + 1 : fft_shape[i - 1];
      full_fft_shape.AddDim(fft_shape[i - 1]);
    }

    Tensor temp;
    OP_REQUIRES_OK(ctx, ctx->allocate_temp(DataTypeToEnum<ComplexT>::v(),
                                           full_fft_shape, &temp));
    auto full_fft = temp.flat_inner_dims<ComplexT, FFTRank + 1>();

    // Calculate the starting point and range of the source of
    // negative frequency part.
    auto neg_sizes = input_slice_sizes;
    neg_sizes[FFTRank] = fft_shape[FFTRank - 1] - input_slice_sizes[FFTRank];
    Eigen::DSizes<Eigen::DenseIndex, FFTRank + 1> neg_target_indices;
    neg_target_indices[FFTRank] = input_slice_sizes[FFTRank];

    const Eigen::DSizes<Eigen::DenseIndex, FFTRank + 1> start_indices;
    Eigen::DSizes<Eigen::DenseIndex, FFTRank + 1> neg_start_indices;
    neg_start_indices[FFTRank] = 1;

    full_fft.slice(start_indices, input_slice_sizes).device(device) =
        input.slice(start_indices, input_slice_sizes);

    // First, conduct IFFTs on outer dimensions. We save computation (and
    // avoid touching uninitialized memory) by slicing full_fft to the
    // subregion we wrote input to.
    if (FFTRank > 1) {
      const auto outer_axes =
          Eigen::ArrayXi::LinSpaced(FFTRank - 1, 1, FFTRank - 1);
      full_fft.slice(start_indices, input_slice_sizes).device(device) =
          full_fft.slice(start_indices, input_slice_sizes)
              .template fft<Eigen::BothParts, Eigen::FFT_REVERSE>(outer_axes);
    }

    // Reconstruct the full FFT by appending reversed and conjugated
    // spectrum as the negative frequency part.
    Eigen::array<bool, FFTRank + 1> reverse_last_axis;
    for (auto i = 0; i <= FFTRank; i++) {
      reverse_last_axis[i] = i == FFTRank;
    }

    if (neg_sizes[FFTRank] != 0) {
      full_fft.slice(neg_target_indices, neg_sizes).device(device) =
          full_fft.slice(neg_start_indices, neg_sizes)
              .reverse(reverse_last_axis)
              .conjugate();
    }

    auto inner_axis = Eigen::array<int, 1>{FFTRank};
    output.device(device) =
        full_fft.template fft<Eigen::RealPart, Eigen::FFT_REVERSE>(inner_axis);
  }