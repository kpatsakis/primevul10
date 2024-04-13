void VideoRendererBase::Seek(base::TimeDelta time, const PipelineStatusCB& cb) {
  base::AutoLock auto_lock(lock_);
  DCHECK_EQ(state_, kFlushed) << "Must flush prior to seeking.";
  DCHECK(!cb.is_null());
  DCHECK(seek_cb_.is_null());

  state_ = kSeeking;
  seek_cb_ = cb;
  seek_timestamp_ = time;
  AttemptRead_Locked();
}
