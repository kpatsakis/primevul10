  void append(char c) {
    assertx(p < end);
    *p++ = c;
  }