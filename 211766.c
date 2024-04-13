  size_t operator()(const StringData *s) const {
    return s->hash();
  }