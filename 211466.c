  String(const char *s, CopyStringMode mode)
  : m_str(LIKELY((bool)s) ? StringData::Make(s, mode) : nullptr, NoIncRef{}) {}