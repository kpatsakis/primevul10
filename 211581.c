  /* implicit */ String(const std::string &s)
  : m_str(StringData::Make(s.data(), s.size(), CopyString), NoIncRef{}) { }