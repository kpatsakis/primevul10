int VideoRendererBase::NumFrames_Locked() const {
  lock_.AssertAcquired();
  int outstanding_frames =
      (current_frame_ ? 1 : 0) + (last_available_frame_ ? 1 : 0) +
      (current_frame_ && (current_frame_ == last_available_frame_) ? -1 : 0);
  return ready_frames_.size() + outstanding_frames;
}
