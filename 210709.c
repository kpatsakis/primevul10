void VideoRendererBase::GetCurrentFrame(scoped_refptr<VideoFrame>* frame_out) {
  base::AutoLock auto_lock(lock_);
  DCHECK(!pending_paint_ && !pending_paint_with_last_available_);

  if ((!current_frame_ || current_frame_->IsEndOfStream()) &&
      (!last_available_frame_ || last_available_frame_->IsEndOfStream())) {
    *frame_out = NULL;
    return;
  }

  DCHECK_NE(state_, kUninitialized);
  DCHECK_NE(state_, kStopped);
  DCHECK_NE(state_, kError);

  if (current_frame_) {
    *frame_out = current_frame_;
    last_available_frame_ = current_frame_;
    pending_paint_ = true;
  } else {
    DCHECK(last_available_frame_);
    *frame_out = last_available_frame_;
    pending_paint_with_last_available_ = true;
  }
}
