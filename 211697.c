String serialize_keep_dvarrays(const Variant& value) {
  SerializeOptions opts;
  opts.keepDVArrays = true;
  return serialize_impl(value, opts);
}