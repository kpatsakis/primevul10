inline strhash_t StringData::hashStatic() const {
  assertx(isStatic());
  const strhash_t h = m_hash & STRHASH_MASK;
  assertx(h);
  return h;
}