bool VideoRendererBase::HasEnded() {
  base::AutoLock auto_lock(lock_);
  return state_ == kEnded;
}
