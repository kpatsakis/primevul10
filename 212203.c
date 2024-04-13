  uint32_t capacity() const {
    return m_str->capacity(); // intentionally skip nullptr check
  }