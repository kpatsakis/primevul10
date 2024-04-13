  String(size_t cap, ReserveStringMode /*mode*/)
      : m_str(StringData::Make(cap), NoIncRef{}) {}