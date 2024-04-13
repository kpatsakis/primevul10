  void Compute(OpKernelContext* context) override {
    const Tensor& tensor_in = context->input(0);

    std::vector<int32> ksize = ksize_;
    std::vector<int32> stride = stride_;

    if (context->num_inputs() != 1) {
      const Tensor& tensor_ksize = context->input(1);
      auto value_ksize = tensor_ksize.flat<int32>();
      ksize.resize(tensor_ksize.shape().num_elements());
      std::copy_n(&value_ksize(0), ksize.size(), ksize.begin());

      const Tensor& tensor_stride = context->input(2);
      auto value_stride = tensor_stride.flat<int32>();
      stride.resize(tensor_stride.shape().num_elements());
      std::copy_n(&value_stride(0), stride.size(), stride.begin());
    }
    OP_REQUIRES(context, ksize.size() == 4,
                errors::InvalidArgument("Sliding window ksize field must "
                                        "specify 4 dimensions"));
    OP_REQUIRES(context, stride.size() == 4,
                errors::InvalidArgument("Sliding window stride field must "
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
    if (!context->status().ok()) {
      return;
    }

    TensorShape out_shape =
        ShapeFromFormat(data_format_, params.tensor_in_batch, params.out_height,
                        params.out_width, params.depth);
    if (data_format_ == FORMAT_NCHW) {
      DnnPoolingOp<T>::Compute(context, se::dnn::PoolingMode::kMaximum, ksize,
                               stride, padding_, explicit_paddings_,
                               data_format_, tensor_in, out_shape,
                               propagate_nans_);
    } else {
      CHECK(data_format_ == FORMAT_NHWC)
          << "MaxPool only supports NCHW or NHWC format";
      Tensor* output = nullptr;
      OP_REQUIRES_OK(context, context->allocate_output(0, out_shape, &output));
      LaunchMaxPoolingNoMask<Device, T>::launch(context, params, tensor_in,
                                                output, propagate_nans_);
    }
  }