inline bool StringData::same(const StringData* s) const {
  assertx(s);
  if (m_len != s->m_len) return false;
  // The underlying buffer and its length are 8-byte aligned, ensured by
  // StringData layout, req::malloc, or malloc. So compare words.
  assertx(uintptr_t(data()) % 8 == 0);
  assertx(uintptr_t(s->data()) % 8 == 0);
  return wordsame(data(), s->data(), m_len);
}