void UntrustedCacheMalloc::PushToFreeList(void *buffer) {
  free_list_->buffers.get()[free_list_->count] = buffer;
  free_list_->count++;

  if (free_list_->count == kFreeListCapacity) {
    primitives::DeAllocateUntrustedBuffers(free_list_->buffers.get(),
                                           kFreeListCapacity);
    free_list_->count = 0;
  }
}