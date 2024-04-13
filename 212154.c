  String(char* s, AttachStringMode mode)
  : m_str(LIKELY((bool)s) ? StringData::Make(s, mode) : nullptr, NoIncRef{}) {}