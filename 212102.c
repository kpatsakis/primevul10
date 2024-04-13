TypedValue HHVM_FUNCTION(str_ireplace,
                         const Variant& search, const Variant& replace,
                         const Variant& subject) {
  int64_t count;
  return tvReturn(str_ireplace(search, replace, subject, count));
}