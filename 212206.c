TypedValue HHVM_FUNCTION(strchr,
                         const String& haystack,
                         const Variant& needle) {
  return HHVM_FN(strstr)(haystack, needle);
}