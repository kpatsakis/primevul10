std::vector<Tensor> EagerKernelArgs::GetLocalTensors() const {
  std::vector<Tensor> lcoal_inputs;
  lcoal_inputs.reserve(tensor_args_.size());
  for (const TensorValue& tensor_value : tensor_args_) {
    lcoal_inputs.push_back(*tensor_value.tensor);
  }
  return lcoal_inputs;
}