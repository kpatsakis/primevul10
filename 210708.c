void VideoRendererBase::DoStopOrError_Locked() {
  DCHECK(!pending_paint_);
  DCHECK(!pending_paint_with_last_available_);
  lock_.AssertAcquired();
  current_frame_ = NULL;
  last_available_frame_ = NULL;
  ready_frames_.clear();
}
