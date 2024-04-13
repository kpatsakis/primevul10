  bool operator() (const PatAndRepl &a, const PatAndRepl &b) {
    uint16_t  hash_a = a.hash(m - B, B) & HASH_TAB_MASK,
              hash_b = b.hash(m - B, B) & HASH_TAB_MASK;

    if (hash_a > hash_b) {
      return false;
    }
    if (hash_a < hash_b) {
      return true;
    }
    // longer patterns must be sorted first
    if (a.getPat().size() > b.getPat().size()) {
      return true;
    }
    if (a.getPat().size() < b.getPat().size()) {
      return false;
    }
    return false;
  }