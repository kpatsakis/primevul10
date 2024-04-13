inline folly::StringPiece StringData::slice() const {
  return folly::StringPiece{data(), m_len};
}