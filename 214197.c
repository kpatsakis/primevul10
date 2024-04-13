  void operator()(const CPUDevice& d, typename TTypes<T>::ConstFlat gradient,
                  typename TTypes<T>::ConstFlat input,
                  typename TTypes<T>::ConstScalar input_min_tensor,
                  typename TTypes<T>::ConstScalar input_max_tensor,
                  typename TTypes<T>::Flat input_backprop,
                  typename TTypes<T>::Scalar input_min_backprop,
                  typename TTypes<T>::Scalar input_max_backprop) {
    QuantizeAndDequantizeOneScaleGradientImpl<CPUDevice, T>::Compute(
        d, gradient, input, input_min_tensor, input_max_tensor, input_backprop,
        input_min_backprop, input_max_backprop);
  }