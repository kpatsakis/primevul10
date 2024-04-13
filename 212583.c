  void setBuf(char* buf, size_t cap) {
    begin = p = buf;
#ifndef NDEBUG
    end = begin + cap;
#endif
  }