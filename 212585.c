  bool matchSeparatorSlow(char ch) {
    --p;
    skipSpace();
    if (LIKELY(*p++ == ch)) return true;
    --p;
    return false;
  }