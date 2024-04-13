  void Initialize() {
    EXPECT_CALL(*this, FrameReady(media::VideoDecoder::kOk, _));
    decoder_->Read(read_cb_);

    EXPECT_CALL(*vc_manager_, AddDevice(_, _))
        .WillOnce(Return(vc_impl_.get()));
    int buffer_count = 1;
    EXPECT_CALL(*vc_impl_, StartCapture(capture_client(), _))
        .Times(1)
        .WillOnce(CreateDataBufferFromCapture(capture_client(),
                                              vc_impl_.get(),
                                              buffer_count));
    EXPECT_CALL(*vc_impl_, FeedBuffer(_))
        .Times(buffer_count)
        .WillRepeatedly(DeleteDataBuffer());

    decoder_->Initialize(NULL,
                         media::NewExpectedStatusCB(media::PIPELINE_OK),
                         NewStatisticsCB());
    message_loop_->RunAllPending();
  }
