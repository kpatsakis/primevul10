void VideoRendererBase::OnDecoderFlushDone() {
  base::AutoLock auto_lock(lock_);
  DCHECK_EQ(kFlushingDecoder, state_);
  DCHECK(!pending_read_);

  state_ = kFlushing;
  AttemptFlush_Locked();
}
