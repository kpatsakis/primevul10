  void Compute(OpKernelContext* context) override {
    const Tensor& tensor_in = context->input(0);
    const Tensor& tensor_out = context->input(1);
    const Tensor& out_grad_backprop = context->input(2);

    // For maxpooling, tensor_in should have 4 dimensions.
    OP_REQUIRES(context, tensor_in.dims() == 4,
                errors::InvalidArgument("tensor_in must be 4-dimensional 4"));
    OP_REQUIRES(context, tensor_out.dims() == 4,
                errors::InvalidArgument("tensor_out must be 4-dimensional"));
    // For maxpooling, out_grad_backprop should have 4 dimensions.
    OP_REQUIRES(
        context, out_grad_backprop.dims() == 4,
        errors::InvalidArgument("out_grad_backprop must be 4-dimensional"));

    Tensor* output = nullptr;
    OP_REQUIRES_OK(context,
                   context->allocate_output(0, tensor_out.shape(), &output));

    std::vector<int32> ksize = ksize_;
    std::vector<int32> stride = stride_;
    if (context->num_inputs() == 5) {
      const Tensor& tensor_ksize = context->input(3);
      auto value_ksize = tensor_ksize.flat<int32>();
      ksize.resize(tensor_ksize.shape().num_elements());
      std::copy_n(&value_ksize(0), ksize.size(), ksize.begin());

      const Tensor& tensor_stride = context->input(4);
      auto value_stride = tensor_stride.flat<int32>();
      stride.resize(tensor_stride.shape().num_elements());
      std::copy_n(&value_stride(0), stride.size(), stride.begin());
    }

    OP_REQUIRES(context, ksize.size() == 4,
                errors::InvalidArgument("Sliding window ksize field must "
                                        "specify 4 dimensions"));
    OP_REQUIRES(context, stride.size() == 4,
                errors::InvalidArgument("Sliding window strides field must "
                                        "specify 4 dimensions"));
    const int32 ksize_n = GetTensorDim(ksize, data_format_, 'N');
    const int32 stride_n = GetTensorDim(stride, data_format_, 'N');
    OP_REQUIRES(context, ksize_n == 1 && stride_n == 1,
                errors::Unimplemented(
                    "Pooling is not yet supported on the batch dimension."));

    PoolParameters params{context,
                          ksize,
                          stride,
                          padding_,
                          /*explicit_paddings=*/{},
                          data_format_,
                          tensor_in.shape()};

    functor::MaxPoolGradBackwardNoMask<T>()(
        data_format_, tensor_in.flat<T>().data(), tensor_out.flat<T>().data(),
        params.tensor_in_batch, params.out_height, params.out_width,
        params.depth, params.tensor_in_rows, params.tensor_in_cols,
        params.window_rows, params.window_cols, params.row_stride,
        params.col_stride, params.pad_top, params.pad_left,
        out_grad_backprop.flat<T>().data(), output->flat<T>().data(),
        context->eigen_device<Eigen::GpuDevice>());
  }