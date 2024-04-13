  char* data() {
    assertx(p < end);
    *p = 0;
    return begin;
  }