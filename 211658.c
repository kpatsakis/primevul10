    bool operator()(const StringData* s1, const StringData* s2) {
      // ahm uses -1, -2, -3 as magic values
      return int64_t(s1) > 0 && (s1 == s2 || s1->same(s2));
    }