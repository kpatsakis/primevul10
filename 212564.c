  static size_t BufferBytesForLength(int length) {
    return (length + 1) * sizeof(TypedValue) / 2;  // Worst case: "[0,0,...,0]"
  }