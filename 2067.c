string GetFunctionResourceInputDevice(
    const Tensor& input, const int arg_index, const FunctionDef& function_def,
    absl::flat_hash_map<string, std::vector<string>>* composite_devices) {
  const auto& handles = input.flat<ResourceHandle>();
  const ResourceHandle& handle0 = handles(0);
  string composite_device;
  auto iter = function_def.arg_attr().find(arg_index);
  if (iter != function_def.arg_attr().end()) {
    auto arg_attr = iter->second.attr().find("_composite_device");
    if (arg_attr != iter->second.attr().end()) {
      composite_device = arg_attr->second.s();
    }
  }
  if (!composite_device.empty()) {
    if (composite_devices->find(composite_device) == composite_devices->end()) {
      for (int i = 0; i < handles.size(); ++i) {
        (*composite_devices)[composite_device].push_back(handles(i).device());
      }
    }
    return composite_device;
  } else {
    return handle0.device();
  }
}