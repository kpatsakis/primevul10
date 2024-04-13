  String& operator=(req::ptr<StringData>&& src) {
    m_str = std::move(src);
    return *this;
  }