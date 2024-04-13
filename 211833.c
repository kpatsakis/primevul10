  bool operator()(const StringData* a, const StringData* b) const {
    return a == b || a->same(b);
  }