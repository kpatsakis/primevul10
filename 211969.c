Variant HHVM_FUNCTION(soundex,
                      const String& str) {
  if (str.empty()) return false;
  return string_soundex(str);
}