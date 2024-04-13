  String(char* s, size_t length, AttachStringMode mode)
  : m_str(LIKELY((bool)s) ? StringData::Make(s, length, mode)
                          : nullptr, NoIncRef{}) { }