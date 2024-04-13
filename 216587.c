Status KernelAndDeviceOp::Init(const Context& ctx, const NodeDef& ndef,
                               GraphCollector* graph_collector) {
  OpKernel* k = nullptr;
  if (flr_ == nullptr) {
    return errors::Internal(
        "A valid FunctionLibraryRuntime must be provided when running ops "
        "based on OpKernel.");
  }
  std::shared_ptr<const NodeProperties> props;
  TF_RETURN_IF_ERROR(NodeProperties::CreateFromNodeDef(
      ndef, flr_->GetFunctionLibraryDefinition(), &props));
  TF_RETURN_IF_ERROR(flr_->CreateKernel(props, &k));
  kernel_.reset(k);

  input_alloc_attrs_.resize(kernel_->num_inputs());
  input_devices_.resize(kernel_->num_inputs(), device_);
  for (size_t i = 0; i < input_alloc_attrs_.size(); ++i) {
    bool host = kernel_->input_memory_types()[i] == tensorflow::HOST_MEMORY;
    input_alloc_attrs_[i].set_on_host(host);
    if (host) {
      input_devices_[i] = host_cpu_device_;
    }
  }
  output_alloc_attrs_.resize(kernel_->num_outputs());
  for (size_t i = 0; i < output_alloc_attrs_.size(); ++i) {
    output_alloc_attrs_[i].set_on_host(kernel_->output_memory_types()[i] ==
                                       tensorflow::HOST_MEMORY);
  }

  return Status::OK();
}