void VideoRendererBase::PutCurrentFrame(scoped_refptr<VideoFrame> frame) {
  base::AutoLock auto_lock(lock_);

  if (pending_paint_) {
    DCHECK_EQ(current_frame_, frame);
    DCHECK(!pending_paint_with_last_available_);
    pending_paint_ = false;
  } else if (pending_paint_with_last_available_) {
    DCHECK_EQ(last_available_frame_, frame);
    DCHECK(!pending_paint_);
    pending_paint_with_last_available_ = false;
  } else {
    DCHECK(!frame);
  }

  frame_available_.Signal();
  if (state_ == kFlushingDecoder)
    return;

  if (state_ == kFlushing) {
    AttemptFlush_Locked();
    return;
  }

  if (state_ == kError || state_ == kStopped) {
    DoStopOrError_Locked();
  }
}
