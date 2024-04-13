void VideoRendererBase::Initialize(const scoped_refptr<VideoDecoder>& decoder,
                                   const PipelineStatusCB& status_cb,
                                   const StatisticsCB& statistics_cb,
                                   const TimeCB& time_cb) {
  base::AutoLock auto_lock(lock_);
  DCHECK(decoder);
  DCHECK(!status_cb.is_null());
  DCHECK(!statistics_cb.is_null());
  DCHECK(!time_cb.is_null());
  DCHECK_EQ(kUninitialized, state_);
  decoder_ = decoder;

  statistics_cb_ = statistics_cb;
  time_cb_ = time_cb;

  host()->SetNaturalVideoSize(decoder_->natural_size());

  state_ = kFlushed;

  set_opaque_cb_.Run(!decoder->HasAlpha());
  set_opaque_cb_.Reset();

  if (!base::PlatformThread::Create(0, this, &thread_)) {
    NOTREACHED() << "Video thread creation failed";
    state_ = kError;
    status_cb.Run(PIPELINE_ERROR_INITIALIZATION_FAILED);
    return;
  }

#if defined(OS_WIN)
  ::SetThreadPriority(thread_, THREAD_PRIORITY_ABOVE_NORMAL);
#endif  // defined(OS_WIN)
  status_cb.Run(PIPELINE_OK);
}
