String HHVM_FUNCTION(nl2br,
                     const String& str,
                     bool is_xhtml /* = true */) {
  if (str.empty()) {
    return str;
  }
  String htmlType;

  if (is_xhtml) {
    htmlType = s_br;
  } else {
    htmlType = s_non_xhtml_br;
  }

  return stringForEachBuffered(
    str.size(), str,
    [&](StringBuffer& ret, const char*& src, const char* /*end*/) {
      // PHP treats a carriage return beside a newline as the same break
      // no matter what order they're in.  Don't do it for two of the same in
      // a row, though...
      switch (*src) {
        case '\n':
          ret.append(htmlType);
          // skip next if carriage return
          if (*(src + 1) == '\r') {
            ret.append(*src);
            ++src;
          }
          ret.append(*src);
          break;
        case '\r':
          ret.append(htmlType);
          // skip next if newline
          if (*(src + 1) == '\n') {
            ret.append(*src);
            ++src;
          }
        /* fall through */
        default: ret.append(*src);
      }
    });
}