string Canonicalize(const string& funcname, AttrSlice attrs,
                    const FunctionLibraryRuntime::InstantiateOptions& options) {
  absl::InlinedVector<AttrKeyAndValue, 8> entries;
  entries.reserve(attrs.size() + static_cast<int>(!options.target.empty()) +
                  options.input_devices.size());
  for (const auto& p : attrs) {
    if (p.first != kExecutorAttr) {
      entries.push_back(AttrKeyAndValue(
          p.first, -1, Print(p.second, /*hash_string_attrs=*/true)));
    }
  }
  if (!options.target.empty()) {
    entries.push_back(AttrKeyAndValue("_target", -1, options.target,
                                      AttrKeyAndValue::kCEscape));
  }
  for (int i = 0; i < options.input_devices.size(); ++i) {
    entries.push_back(AttrKeyAndValue("_input_dev", i, options.input_devices[i],
                                      AttrKeyAndValue::kCEscape));
  }
  for (int i = 0; i < options.output_devices.size(); ++i) {
    entries.push_back(AttrKeyAndValue("_output_dev", i,
                                      options.output_devices[i],
                                      AttrKeyAndValue::kCEscape));
  }
  for (const auto& iter : options.input_resource_dtypes_and_shapes) {
    entries.push_back(AttrKeyAndValue("_input_resource_dtype", iter.first,
                                      DataTypeString(iter.second.dtype)));
    entries.push_back(AttrKeyAndValue("_input_resource_shape", iter.first,
                                      iter.second.shape.DebugString(),
                                      AttrKeyAndValue::kCEscape));
  }
  if (options.lib_def) {
    entries.push_back(AttrKeyAndValue(
        "_lib_def", -1,
        absl::StrCat("", reinterpret_cast<uintptr_t>(options.lib_def))));
  }
  if (!options.state_handle.empty()) {
    entries.push_back(
        AttrKeyAndValue("_state_handle", -1, options.state_handle));
  }
  string executor_type = FunctionLibraryRuntime::ExecutorType(options, attrs);
  if (!executor_type.empty()) {
    entries.push_back(AttrKeyAndValue(kExecutorAttr, -1, executor_type));
  }
  if (options.config_proto.ByteSize() > 0) {
    string config_proto_serialized;
    SerializeToStringDeterministic(options.config_proto,
                                   &config_proto_serialized);
    entries.push_back(AttrKeyAndValue("_config_proto", -1,
                                      config_proto_serialized,
                                      AttrKeyAndValue::kCEscape));
  }
  std::sort(entries.begin(), entries.end());
  string result = strings::StrCat(funcname, "[");
  bool first = true;
  for (const auto& entry : entries) {
    entry.AppendTo(first, &result);
    first = false;
  }
  result += "]";
  return result;
}