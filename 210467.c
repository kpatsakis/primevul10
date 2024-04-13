  RTCVideoDecoderTest() {
    decoder_ = new RTCVideoDecoder(&message_loop_, kUrl);
    renderer_ = new MockVideoRenderer();
    read_cb_ = base::Bind(&RTCVideoDecoderTest::FrameReady,
                          base::Unretained(this));

    DCHECK(decoder_);

    EXPECT_CALL(statistics_cb_, OnStatistics(_))
        .Times(AnyNumber());
  }
