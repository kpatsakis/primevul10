  bool operator()(const StringData *s1, const StringData *s2) const {
    return bstrcasecmp(s1->data(), s1->size(), s2->data(), s2->size()) < 0;
  }