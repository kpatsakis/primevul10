  String& operator=(StringData *data) {
    m_str = data;
    return *this;
  }