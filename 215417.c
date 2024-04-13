void *UntrustedCacheMalloc::Malloc(size_t size) {
  // Don't access UnturstedCacheMalloc if not running on normal heap, otherwise
  // it will cause error when UntrustedCacheMalloc tries to free the memory on
  // the normal heap.
  if (is_destroyed_ || (size > kPoolEntrySize) || GetSwitchedHeapNext()) {
    return primitives::TrustedPrimitives::UntrustedLocalAlloc(size);
  }
  return GetBuffer();
}