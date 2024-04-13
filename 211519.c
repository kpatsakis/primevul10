static bool string_substr_check(int len, int& f, int& l) {
  assertx(len >= 0);

  if (l < 0 && -l > len) {
    return false;
  }
  if (f >= len) {
    return false;
  }

  if (l > len) {
    l = len;
  }

  if (f < 0 && -f > len) {
    f = 0;
    if (len == 0) {
      return false;
    }
  }

  if (l < 0 && l + len < f) {
    return false;
  }

  // If "from" position is negative, count start position from the end.
  if (f < 0) {
    f += len;
  }
  assertx(f >= 0);

  // If "length" position is negative, set it to the length needed to stop that
  // many chars from the end of the string.
  if (l < 0) {
    l += len - f;
    if (l < 0) {
      l = 0;
    }
  }
  assertx(l >= 0);

  return true;
}