void VideoRendererBase::AttemptRead_Locked() {
  lock_.AssertAcquired();
  DCHECK_NE(kEnded, state_);

  if (pending_read_ ||
      NumFrames_Locked() == limits::kMaxVideoFrames ||
      (!ready_frames_.empty() && ready_frames_.back()->IsEndOfStream()) ||
      state_ == kFlushingDecoder ||
      state_ == kFlushing) {
    return;
  }

  pending_read_ = true;
  decoder_->Read(base::Bind(&VideoRendererBase::FrameReady, this));
}
