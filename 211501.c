  StringData* get() const {
    assertx(valid());
    return m_str.get();
  }