bool HHVM_FUNCTION(is_scalar, const Variant& v) {
  return v.isScalar();
}