  void operator()(const CPUDevice& d,
                  typename TTypes<T, 3>::ConstTensor gradient,
                  typename TTypes<T, 3>::ConstTensor input,
                  const Tensor* input_min_tensor,
                  const Tensor* input_max_tensor,
                  typename TTypes<T, 3>::Tensor input_backprop,
                  typename TTypes<T>::Flat input_min_backprop,
                  typename TTypes<T>::Flat input_max_backprop) {
    QuantizeAndDequantizePerChannelGradientImpl<CPUDevice, T>::Compute(
        d, gradient, input, input_min_tensor, input_max_tensor, input_backprop,
        input_min_backprop, input_max_backprop);
  }