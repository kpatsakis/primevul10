void SoftwareFrameManager::SwapToNewFrameComplete(bool visible) {
  DCHECK(HasCurrentFrame());
  RendererFrameManager::GetInstance()->AddFrame(this, visible);
}
