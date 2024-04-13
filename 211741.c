inline void StringData::invalidateHash() {
  assertx(!isImmutable());
  assertx(!hasMultipleRefs());
  m_hash = 0;
  assertx(checkSane());
}