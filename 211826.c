Variant HHVM_FUNCTION(metaphone, const String& str, int /*phones*/ /* = 0 */) {
  return string_metaphone(str.data(), str.size(), 0, 1);
}