void SoftwareFrameManager::SetVisibility(bool visible) {
  if (HasCurrentFrame()) {
    if (visible) {
      RendererFrameManager::GetInstance()->LockFrame(this);
    } else {
      RendererFrameManager::GetInstance()->UnlockFrame(this);
    }
  }
}
