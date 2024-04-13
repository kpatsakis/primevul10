inline size_t StringData::heapSize() const {
  return isFlat()
    ? isRefCounted()
      ? MemoryManager::sizeIndex2Size(m_aux16)
      : size() + kStringOverhead
    : sizeof(StringData) + sizeof(Proxy);
}