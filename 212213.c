bool HHVM_FUNCTION(is_float, const Variant& v) {
  return is_double(v.asTypedValue());
}