  /* implicit */ String(folly::StringPiece s)
  : m_str(StringData::Make(s), NoIncRef{}) {}