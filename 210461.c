  void Stop() {
    EXPECT_CALL(*vc_impl_, StopCapture(capture_client()))
        .Times(1)
        .WillOnce(CaptureStopped(capture_client(), vc_impl_.get()));
    EXPECT_CALL(*vc_manager_, RemoveDevice(_, _))
        .WillOnce(Return());
    decoder_->Stop(media::NewExpectedClosure());
    message_loop_->RunAllPending();
  }
