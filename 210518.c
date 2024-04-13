void AudioManagerBase::Init() {
  base::AutoLock lock(audio_thread_lock_);
  DCHECK(!audio_thread_.get());
  audio_thread_.reset(new base::Thread("AudioThread"));
  CHECK(audio_thread_->Start());
}
