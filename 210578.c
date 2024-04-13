void SoftwareFrameManager::GetCurrentFrameMailbox(
    cc::TextureMailbox* mailbox,
    scoped_ptr<cc::SingleReleaseCallback>* callback) {
  DCHECK(HasCurrentFrame());
  *mailbox = cc::TextureMailbox(
      current_frame_->shared_memory_.get(), current_frame_->frame_size_pixels_);
  *callback = cc::SingleReleaseCallback::Create(
      base::Bind(ReleaseMailbox, current_frame_));
}
