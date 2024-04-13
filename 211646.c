bool HHVM_FUNCTION(HH_is_dict_or_darray, const Variant& val) {
  return is_dict_or_darray(val.asTypedValue());
}