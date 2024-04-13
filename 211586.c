int64_t HHVM_FUNCTION(strnatcasecmp,
                      const String& str1,
                      const String& str2) {
  return string_natural_cmp(str1.data(), str1.size(), str2.data(), str2.size(),
                            true);
}