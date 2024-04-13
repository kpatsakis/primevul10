Status KernelAndDeviceFunc::Init(const Context& ctx, const NodeDef& ndef,
                                 GraphCollector* graph_collector) {
  TF_RETURN_IF_ERROR(InstantiateFunc(ctx, ndef, graph_collector));
  return pflr_->GetOutputDevices(handle_, &output_devices_,
                                 ctx.eager_lazy_copy);
}