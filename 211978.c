  folly::MutableStringPiece reserve(size_t size) {
    if (!m_str) return folly::MutableStringPiece();
    auto const tmp = m_str->reserve(size);
    if (UNLIKELY(tmp != m_str)) {
      m_str = req::ptr<StringData>::attach(tmp);
    }
    return m_str->bufferSlice();
  }