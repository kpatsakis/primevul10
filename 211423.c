bool HHVM_FUNCTION(boolval, const Variant& v) {
  return v.toBoolean();
}