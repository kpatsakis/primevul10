int64_t HHVM_FUNCTION(strcasecmp,
                      const String& str1,
                      const String& str2) {
  return bstrcasecmp(str1.data(), str1.size(), str2.data(), str2.size());
}