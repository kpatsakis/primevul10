uint32 SoftwareFrameManager::GetCurrentFrameOutputSurfaceId() const {
  DCHECK(HasCurrentFrame());
  return current_frame_->output_surface_id_;
}
