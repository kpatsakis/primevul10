void SoftwareFrameManager::EvictCurrentFrame() {
  DCHECK(HasCurrentFrame());
  DiscardCurrentFrame();
  if (client_)
    client_->ReleaseReferencesToSoftwareFrame();
}
