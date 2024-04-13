TypedValue HHVM_FUNCTION(strcspn,
                         const String& str1,
                         const String& str2,
                         int start /* = 0 */,
                         int length /* = 0x7FFFFFFF */) {
  const char *s1 = str1.data();
  const char *s2 = str2.data();
  int s1_len = str1.size();
  int s2_len = str2.size();

  if (!string_strspn_check(s1_len, start, length)) {
    return make_tv<KindOfBoolean>(false);
  }

  s1 += start;
  for (int pos = 0; pos < length; ++pos) {
    if (memchr(s2, *(s1++), s2_len) != nullptr) {
      return make_tv<KindOfInt64>(pos);
    }
  }

  return make_tv<KindOfInt64>(length);
}