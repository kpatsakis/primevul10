bool HHVM_FUNCTION(is_string, const Variant& v) {
  return is_string(v.asTypedValue());
}