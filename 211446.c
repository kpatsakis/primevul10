bool HHVM_FUNCTION(HH_is_varray, const Variant& val) {
  return is_varray(val.asTypedValue());
}