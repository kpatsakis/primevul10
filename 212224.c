TypedValue HHVM_FUNCTION(substr_replace,
                         const Variant& str, const Variant& replacement,
                         const Variant& start, const Variant& length) {
  return tvReturn(substr_replace(str, replacement, start, length));
}