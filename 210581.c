gfx::Size SoftwareFrameManager::GetCurrentFrameSizeInPixels() const {
  DCHECK(HasCurrentFrame());
  return current_frame_->frame_size_pixels_;
}
