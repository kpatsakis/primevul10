Variant HHVM_FUNCTION(explode,
                      const String& delimiter,
                      const String& str,
                      int64_t limit /* = PHP_INT_MAX */) {
  return StringUtil::Explode(str, delimiter, limit);
}