TypedValue HHVM_FUNCTION(strncasecmp,
                         const String& str1,
                         const String& str2,
                         int len) {
  if (len < 0) {
    raise_warning("Length must be greater than or equal to 0");
    return make_tv<KindOfBoolean>(false);
  }
  return tvReturn(string_strncasecmp(
    str1.data(), str1.size(),
    str2.data(), str2.size(), len
  ));
}