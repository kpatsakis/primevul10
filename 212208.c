  folly::StringPiece slice() const {
    return m_str ? m_str->slice() : folly::StringPiece();
  }