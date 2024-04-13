void SoftwareFrameManager::DiscardCurrentFrame() {
  if (!HasCurrentFrame())
    return;
  current_frame_ = NULL;
  RendererFrameManager::GetInstance()->RemoveFrame(this);
}
