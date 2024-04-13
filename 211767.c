String HHVM_FUNCTION(stripslashes,
                     const String& str) {
  if (str.empty()) {
    return str;
  }

  return stringForEachBuffered(str.size(), str,
    [&] (StringBuffer& ret, const char*& src, const char* end) {
      if (*src == '\\' && *++src == '0') {
        ret.append('\0');
        return;
      }
      if (src < end) {
        ret.append(*src);
      }
    });
}