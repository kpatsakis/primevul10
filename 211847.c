inline bool StringData::isame(const StringData* s) const {
  assertx(s);
  if (this == s) return true;
  if (m_len != s->m_len) return false;
  return bstrcaseeq(data(), s->data(), m_len);
}