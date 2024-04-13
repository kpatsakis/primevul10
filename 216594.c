Status KernelAndDeviceFunc::InstantiateFunc(const Context& ctx,
                                            const NodeDef& ndef,
                                            GraphCollector* graph_collector) {
  const OpDef* op_def = nullptr;
  const FunctionDef* function_def;
  if (flr_ == nullptr) {
    // If function is being executed without an explicit device request,
    // lookup the FunctionDef in the CPU's FLR. All FLRs share the same
    // library.
    function_def = pflr_->GetFLR(host_cpu_device_->name())
                       ->GetFunctionLibraryDefinition()
                       ->Find(ndef.op());
  } else {
    function_def = flr_->GetFunctionLibraryDefinition()->Find(ndef.op());
  }

  if (function_def != nullptr) {
    op_def = &(function_def->signature());
  } else {
    TF_RETURN_IF_ERROR(OpDefForOp(ndef.op(), &op_def));
  }
  TF_RETURN_IF_ERROR(
      InOutTypesForNode(ndef, *op_def, &input_dtypes_, &output_dtypes_));

  FunctionLibraryRuntime::InstantiateOptions options;
  options.target = device_ == nullptr ? "" : device_->name();
  options.is_multi_device_function = true;
  for (const Device* device : input_devices_) {
    options.input_devices.push_back(device->name());
  }
  options.composite_devices = composite_devices_;
  options.input_resource_dtypes_and_shapes = input_resource_dtypes_and_shapes_;

  const auto& it = ndef.attr().find("executor_type");
  if (it != ndef.attr().end()) {
    options.executor_type = it->second.s();
  }
  const auto& is_component_fn_it = ndef.attr().find("is_component_function");
  if (is_component_fn_it != ndef.attr().end()) {
    options.is_component_function = is_component_fn_it->second.b();
  }
#if !defined(IS_MOBILE_PLATFORM)
  // Android tf library does not include grappler.
  const auto& config_it = ndef.attr().find("config_proto");
  if (it != ndef.attr().end()) {
    if (!options.config_proto.ParseFromString(config_it->second.s())) {
      return errors::InvalidArgument(
          "Failed to parse config_proto attribute as tensorflow::ConfigProto "
          "proto.");
    }
    grappler::GrapplerItem::OptimizationOptions optimization_options;

    // Tensorflow 2.0 in eager mode with automatic control dependencies will
    // prune all nodes that are not in the transitive fanin of the fetch nodes.
    // However because the function will be executed via FunctionLibraryRuntime,
    // and current function implementation does not prune stateful and dataset
    // ops, we rely on Grappler to do the correct graph pruning.
    optimization_options.allow_pruning_stateful_and_dataset_ops = true;

    optimization_options.is_eager_mode = true;

    // All the nested function calls will be executed and optimized via
    // PartitionedCallOp, there is no need to optimize functions now.
    optimization_options.optimize_function_library = false;

    options.optimize_graph_fn = std::bind(
        grappler::OptimizeGraph, std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3, std::placeholders::_4, std::placeholders::_5,
        options.config_proto, function_def->signature().name(),
        optimization_options, std::placeholders::_6);
  }
#endif  // !IS_MOBILE_PLATFORM
  options.graph_collector = graph_collector;

  // In Eager mode we always inline all functions into the top-level
  // function body graph, to get a single executable graph, that could be
  // optimized across function boundaries (e.g. prune unused inputs and outputs
  // in a function call chain). This is required to mimic graph mode execution,
  // with aggressive pruning of nodes not in the transitive fanin of fetches.
  options.config_proto.mutable_graph_options()
      ->mutable_optimizer_options()
      ->set_do_function_inlining(true);

  options.config_proto.set_log_device_placement(ctx.log_device_placement);

  TF_RETURN_IF_ERROR(
      pflr_->Instantiate(ndef.op(), AttrSlice(ndef), options, &handle_));
  return pflr_->IsCrossProcess(handle_, &is_cross_process_);
}