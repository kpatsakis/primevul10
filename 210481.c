void VideoRendererBase::AttemptFlush_Locked() {
  lock_.AssertAcquired();
  DCHECK_EQ(kFlushing, state_);

  ready_frames_.clear();

  if (!pending_paint_ && !pending_read_) {
    state_ = kFlushed;
    current_frame_ = NULL;
    base::ResetAndReturn(&flush_cb_).Run();
  }
}
