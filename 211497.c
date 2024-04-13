  bool operator()(const StringData *s1, const StringData *s2) const {
    assertx(s1 && s2);
    return s1->same(s2);
  }