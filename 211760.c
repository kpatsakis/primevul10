String HHVM_FUNCTION(addslashes,
                     const String& str) {
  if (str.empty()) {
    return str;
  }

  return stringForEachBuffered(
    str.size(), str,
    [&](StringBuffer& ret, const char* src, const char* /*end*/) {
      switch (*src) {
        case '\0':
          ret.append('\\');
          ret.append('0');
          break;
        case '\\':
        case '\"':
        case '\'':
          ret.append('\\');
        /* fall through */
        default:
          ret.append(*src);
      }
    });
}