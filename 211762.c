inline size_t StringData::estimateCap(size_t size) {
  assertx(size <= MaxSize);
  return MemoryManager::sizeClass(size + kStringOverhead);
}