float SoftwareFrameManager::GetCurrentFrameDeviceScaleFactor() const {
  DCHECK(HasCurrentFrame());
  return current_frame_->frame_device_scale_factor_;
}
