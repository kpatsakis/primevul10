  String(const char *s, size_t length, CopyStringMode mode)
  : m_str(LIKELY((bool)s) ? StringData::Make(s, length, mode)
                          : nullptr, NoIncRef{}) { }