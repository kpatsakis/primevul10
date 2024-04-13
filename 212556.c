  bool parseNumber(char firstChar, bool array_elem = false) {
    uint64_t x = 0;
    bool neg = false;
    const char* begin = p - 1;
    if (firstChar == '-') {
      neg = true;
    } else {
      x = firstChar - '0';  // first digit
    }
    // Parse maximal digit sequence into x (non-negative).
    while (*p >= '0' && *p <= '9') {
      x = (x * 10) + (*p - '0');
      ++p;
    }
    if (*p == '.' || *p == 'e' || *p == 'E') {
      pushDouble(zend_strtod(begin, &p));
      return true;
    }

    auto len = p - begin;

    // JSON arrays don't permit leading 0's in numbers.
    if (UNLIKELY(len > 1 && firstChar == '0' && array_elem)) {
      return false;
    }

    // Now 'x' is the usigned absolute value of a naively parsed integer, but
    // potentially overflowed mod 2^64.
    if (LIKELY(len < 19) || (len == 19 && firstChar <= '8')) {
      int64_t sx = x;
      pushInt64(neg ? -sx : sx);
    } else {
      parseBigInt(len);
    }
    return true;
  }