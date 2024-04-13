int64_t HHVM_FUNCTION(ord,
                      const String& str) {
  return (int64_t)(unsigned char)str[0];
}