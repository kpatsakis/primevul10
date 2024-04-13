String HHVM_FUNCTION(quotemeta,
                     const String& str) {
  if (str.empty()) {
    return str;
  }

  return stringForEachBuffered(
    str.size(), str,
    [&](StringBuffer& ret, const char* src, const char* /*end*/) {
      switch (*src) {
        case '.':
        case '\\':
        case '+':
        case '*':
        case '?':
        case '[':
        case ']':
        case '^':
        case '$':
        case '(':
        case ')':
          ret.append('\\');
        /* fall through */
        default:
          ret.append(*src);
      }
    });
}