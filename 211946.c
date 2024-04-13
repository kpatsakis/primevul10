bool HHVM_FUNCTION(is_numeric, const Variant& v) {
  return v.isNumeric(true);
}