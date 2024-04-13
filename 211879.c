  String substr(int start, int length = StringData::MaxSize) const {
    return String::attach(
      m_str ? m_str->substr(start, length) : staticEmptyString());
  }