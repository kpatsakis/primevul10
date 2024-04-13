bool HHVM_FUNCTION(HH_is_vec_or_varray, const Variant& val) {
  return is_vec_or_varray(val.asTypedValue());
}