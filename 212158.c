  bool string_strspn_check(int inputLength, int &start, int &scanLength) {
    if (start < 0) {
      start += inputLength;
      if (start < 0) {
        start = 0;
      }
    } else if (start > inputLength) {
      return false;
    }

    if (scanLength < 0) {
      scanLength += (inputLength - start);
      if (scanLength < 0) {
        scanLength = 0;
      }
    }
    if (scanLength > inputLength - start) {
      scanLength = inputLength - start;
    }
    return true;
  }