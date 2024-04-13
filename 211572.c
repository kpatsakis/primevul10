bool HHVM_FUNCTION(HH_is_keyset, const Variant& v) {
  return is_keyset(v.asTypedValue());
}