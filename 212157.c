inline uint32_t StringData::capacity() const {
  assertx(isRefCounted());
  return kSizeIndex2StringCapacity[m_aux16 & 0xff];
}