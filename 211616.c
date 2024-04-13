bool HHVM_FUNCTION(is_bool, const Variant& v) {
  return is_bool(v.asTypedValue());
}