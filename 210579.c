void* SoftwareFrameManager::GetCurrentFramePixels() const {
  DCHECK(HasCurrentFrame());
  DCHECK(base::SharedMemory::IsHandleValid(
      current_frame_->shared_memory_->handle()));
  return current_frame_->shared_memory_->memory();
}
