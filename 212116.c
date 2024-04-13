  folly::MutableStringPiece bufferSlice() {
    return m_str ? m_str->bufferSlice() : folly::MutableStringPiece();
  }