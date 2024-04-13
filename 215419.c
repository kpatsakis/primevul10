UntrustedCacheMalloc::UntrustedCacheMalloc() : lock_(/*is_recursive=*/true) {
  if (is_destroyed_) {
    return;
  }
  // Initialize a free list object in the trusted heap. The free list object
  // stores an array of buffers stored in the untrusted heap.
  free_list_ = absl::make_unique<FreeList>();
  free_list_->buffers.reset(reinterpret_cast<void **>(
      primitives::TrustedPrimitives::UntrustedLocalAlloc(sizeof(void *) *
                                                         kFreeListCapacity)));
  free_list_->count = 0;
}