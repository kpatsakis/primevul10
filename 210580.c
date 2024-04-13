gfx::Size SoftwareFrameManager::GetCurrentFrameSizeInDIP() const {
  DCHECK(HasCurrentFrame());
  return ConvertSizeToDIP(current_frame_->frame_device_scale_factor_,
                          current_frame_->frame_size_pixels_);
}
