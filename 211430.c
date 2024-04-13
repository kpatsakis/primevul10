bool HHVM_FUNCTION(is_object, const Variant& v) {
  return is_object(v.asTypedValue());
}