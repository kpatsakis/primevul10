void VideoRendererBase::Stop(const base::Closure& callback) {
  if (state_ == kStopped) {
    callback.Run();
    return;
  }

  base::PlatformThreadHandle thread_to_join = base::kNullThreadHandle;
  {
    base::AutoLock auto_lock(lock_);
    state_ = kStopped;

    statistics_cb_.Reset();
    time_cb_.Reset();
    if (!pending_paint_ && !pending_paint_with_last_available_)
      DoStopOrError_Locked();

    if (thread_ != base::kNullThreadHandle) {
      frame_available_.Signal();
      thread_to_join = thread_;
      thread_ = base::kNullThreadHandle;
    }
  }
  if (thread_to_join != base::kNullThreadHandle)
    base::PlatformThread::Join(thread_to_join);

  decoder_->Stop(callback);
}
