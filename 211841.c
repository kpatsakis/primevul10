  String(const String& s, CopyStringMode mode)
  : m_str(LIKELY((bool)s) ? StringData::Make(s.c_str(), s.size(), mode)
                          : nullptr, NoIncRef{}) {}