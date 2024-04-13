TypedValue HHVM_FUNCTION(str_replace,
                         const Variant& search, const Variant& replace,
                         const Variant& subject) {
  int64_t count;
  return tvReturn(str_replace(search, replace, subject, count));
}