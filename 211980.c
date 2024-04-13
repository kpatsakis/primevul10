inline strhash_t StringData::hash() const {
  strhash_t h = m_hash & STRHASH_MASK;
  return h ? h : hashHelper();
}