  /* implicit */ String(const char* s)
  : m_str(LIKELY((bool)s) ? StringData::Make(s, CopyString)
                           : nullptr, NoIncRef{}) { }