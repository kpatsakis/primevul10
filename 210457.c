  CaptureVideoDecoderTest() {
    message_loop_.reset(new MessageLoop(MessageLoop::TYPE_IO));
    message_loop_proxy_ =
        base::MessageLoopProxy::current().get();
    vc_manager_ = new MockVideoCaptureImplManager();
    media::VideoCaptureCapability capability;
    capability.width = kWidth;
    capability.height = kHeight;
    capability.frame_rate = kFPS;
    capability.expected_capture_delay = 0;
    capability.color = media::VideoCaptureCapability::kI420;
    capability.interlaced = false;

    decoder_ = new CaptureVideoDecoder(message_loop_proxy_,
                                       kVideoStreamId, vc_manager_, capability);
    EXPECT_CALL(statistics_cb_object_, OnStatistics(_))
        .Times(AnyNumber());

    read_cb_ = base::Bind(&CaptureVideoDecoderTest::FrameReady,
                          base::Unretained(this));

    vc_impl_.reset(new MockVideoCaptureImpl(
        kVideoStreamId, message_loop_proxy_, new VideoCaptureMessageFilter()));
  }
