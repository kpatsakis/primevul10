void UntrustedCacheMalloc::Free(void *buffer) {
  if (is_destroyed_ || GetSwitchedHeapNext()) {
    primitives::TrustedPrimitives::UntrustedLocalFree(buffer);
    return;
  }
  LockGuard spin_lock(&lock_);

  // Add the buffer to the free list if it was not allocated from the buffer
  // pool and was allocated via UntrustedLocalAlloc. If the
  // buffer was allocated from the buffer pool push it back to the pool.
  if (busy_buffers_.find(buffer) == busy_buffers_.end()) {
    PushToFreeList(buffer);
    return;
  }
  busy_buffers_.erase(buffer);
  buffer_pool_.push(buffer);
}