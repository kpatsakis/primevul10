Device* KernelAndDeviceFunc::InputDevice(int i) const {
  if ((input_dtypes_[i] == DT_RESOURCE) &&
      (composite_devices_.find(input_devices_[i]->name()) ==
       composite_devices_.end())) {
    return host_cpu_device_;
  } else {
    return input_devices_[i];
  }
}