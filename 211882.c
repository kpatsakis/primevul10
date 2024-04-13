int64_t HHVM_FUNCTION(strcmp,
                      const String& str1,
                      const String& str2) {
  return string_strcmp(str1.data(), str1.size(), str2.data(), str2.size());
}