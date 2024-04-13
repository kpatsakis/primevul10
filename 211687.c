bool HHVM_FUNCTION(HH_is_darray, const Variant& val) {
  return is_darray(val.asTypedValue());
}