String HHVM_FUNCTION(serialize, const Variant& value) {
  return serialize_impl(value, SerializeOptions());
}