  static void launch(OpKernelContext* context, const PoolParameters& params,
                     const Tensor& grad_in, const Tensor& argmax,
                     Tensor* grad_out, const bool include_batch_in_index) {
    const int input_size = params.tensor_in_batch * params.tensor_in_rows *
                           params.tensor_in_cols * params.depth;
    const int output_size = params.tensor_in_batch * params.out_height *
                            params.out_width * params.depth;
    const int top_offset =
        params.tensor_in_rows * params.tensor_in_cols * params.depth;
    const int bottom_offset =
        params.out_width * params.out_height * params.depth;
    bool status = functor::MaxPoolGradBackwardWithArgmax<T>()(
        output_size, input_size, grad_in.flat<T>().data(),
        reinterpret_cast<const int64*>(argmax.flat<int64>().data()), top_offset,
        bottom_offset, grad_out->flat<T>().data(), context->eigen_gpu_device(),
        include_batch_in_index);
    if (!status) {
      context->SetStatus(
          errors::Internal("Failed launching MaxPoolGradBackwardWithArgmax"));
    }
  }