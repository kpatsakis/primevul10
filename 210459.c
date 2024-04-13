  MockVideoCaptureImpl(const media::VideoCaptureSessionId id,
                       scoped_refptr<base::MessageLoopProxy> ml_proxy,
                       VideoCaptureMessageFilter* filter)
      : VideoCaptureImpl(id, ml_proxy, filter) {
  }
