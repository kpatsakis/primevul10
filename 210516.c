scoped_refptr<base::MessageLoopProxy> AudioManagerBase::GetMessageLoop() {
  base::AutoLock lock(audio_thread_lock_);
  return audio_thread_.get() ? audio_thread_->message_loop_proxy() : NULL;
}
