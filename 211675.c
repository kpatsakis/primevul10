bool HHVM_FUNCTION(HH_is_php_array, const Variant& v) {
  // We want this set to false as this is meant to be the "this can receive
  // both PHP and Hack arrays" version of `is_array`.
  return is_php_array(v.asTypedValue());
}