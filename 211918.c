bool HHVM_FUNCTION(is_int, const Variant& v) {
  return is_int(v.asTypedValue());
}