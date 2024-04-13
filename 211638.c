  String& operator=(String&& src) {
    m_str = std::move(src.m_str);
    return *this;
  }