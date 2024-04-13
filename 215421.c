UntrustedCacheMalloc::~UntrustedCacheMalloc() {
  while (!buffer_pool_.empty()) {
    PushToFreeList(buffer_pool_.top());
    buffer_pool_.pop();
  }

  // Free remaining elements in the free_list_.
  // The free_list_ object and the struct FreeList member buffers are destroyed
  // when the unique pointers referencing them go out of scope.
  if (free_list_->count > 0) {
    primitives::DeAllocateUntrustedBuffers(free_list_->buffers.get(),
                                           free_list_->count);
  }
  is_destroyed_ = true;
}