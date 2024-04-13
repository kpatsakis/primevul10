  bool valid() const {
    if (s_globalInit) {
      assertx(m_str);
      return true;
    }
    return false;
  }