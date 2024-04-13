VideoRendererBase::~VideoRendererBase() {
  base::AutoLock auto_lock(lock_);
  DCHECK(state_ == kUninitialized || state_ == kStopped) << state_;
 }
