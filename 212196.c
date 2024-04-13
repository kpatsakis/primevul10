  String rvalImpl(int key) const {
    if (m_str) {
      return String{m_str->getChar(key)};
    }
    return String();
  }