bool HHVM_FUNCTION(is_null, const Variant& v) {
  return is_null(v.asTypedValue());
}