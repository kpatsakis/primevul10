  bool parseRawString(int* len) {
    assertx(p[-1] == '"'); // SimpleParser only handles "-quoted strings
    *len = 0;
    auto const charTop = reinterpret_cast<signed char*>(top);
    for (signed char ch = *p++; ch != '\"'; ch = *p++) {
      charTop[(*len)++] = ch; // overwritten later if `ch == '\\'`
      if (ch < ' ') {
        // `ch < ' '` catches null and also non-ASCII (since signed char)
        return false;
      } else if (ch == '\\') {
        if (!handleBackslash(charTop[*len - 1])) return false;
      }
    }
    return true;
  }