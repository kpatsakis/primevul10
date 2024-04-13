bool HHVM_FUNCTION(HH_is_dict, const Variant& v) {
  return is_dict(v.asTypedValue());
}