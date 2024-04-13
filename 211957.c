  uint32_t capacity() const {
    return m_px->capacity(); // intentionally skip nullptr check
  }