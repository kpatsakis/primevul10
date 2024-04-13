  void parseBigInt(int len) {
    assertx(*p > '9' || *p < '0');  // Aleady read maximal digit sequence.
    errno = 0;
    const int64_t sx = strtoll(p - len, nullptr, 10);
    if (errno == ERANGE) {
      const double dval = zend_strtod(p - len, nullptr);
      assertx(dval == floor(dval));
      pushDouble(dval);
    } else {
      pushInt64(sx);
    }
  }