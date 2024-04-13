  bool matchSeparator(char ch) {
    if (LIKELY(*p++ == ch)) return true;
    return matchSeparatorSlow(ch);
  }