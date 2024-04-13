int64_t HHVM_FUNCTION(strcoll,
                      const String& str1,
                      const String& str2) {
  return strcoll(str1.c_str(), str2.c_str());
}