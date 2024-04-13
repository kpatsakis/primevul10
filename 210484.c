void VideoRendererBase::Flush(const base::Closure& callback) {
  base::AutoLock auto_lock(lock_);
  DCHECK_EQ(state_, kPaused);
  flush_cb_ = callback;
  state_ = kFlushingDecoder;

  base::AutoUnlock auto_unlock(lock_);
  decoder_->Reset(base::Bind(&VideoRendererBase::OnDecoderFlushDone, this));
}
