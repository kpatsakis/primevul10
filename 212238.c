String HHVM_FUNCTION(addcslashes,
                     const String& str,
                     const String& charlist) {
  if (str.empty() || (!charlist.isNull() && charlist.empty())) {
    return str;
  }

  int masklen = charlist.isNull() ? 10 : charlist.size();
  const char* list = charlist.isNull() ? "\\\x00\x01..\x1f\x7f..\xff"
                                       : charlist.c_str();

  char flags[256];
  string_charmask(list, masklen, flags);

  return stringForEachBuffered(
    str.size(), str,
    [&](StringBuffer& ret, const char* src, const char* /*end*/) {
      int c = (unsigned char)*src;

      if (flags[c]) {
        ret.append('\\');
        if ((c < 32) || (c > 126)) {
          switch (c) {
            case '\n': ret.append('n'); break;
            case '\t': ret.append('t'); break;
            case '\r': ret.append('r'); break;
            case '\a': ret.append('a'); break;
            case '\v': ret.append('v'); break;
            case '\b': ret.append('b'); break;
            case '\f': ret.append('f'); break;
            default:
              ret.append((char)('0' + (c / 64)));
              c %= 64;
              ret.append((char)('0' + (c / 8)));
              c %= 8;
              ret.append((char)('0' + c));
          }
          return;
        }
      }
      ret.append((char)c);
    });
}