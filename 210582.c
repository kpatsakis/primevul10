bool SoftwareFrameManager::HasCurrentFrame() const {
  return current_frame_.get() ? true : false;
}
