bool HHVM_FUNCTION(HH_is_vec, const Variant& v) {
  return is_vec(v.asTypedValue());
}