  FFmpegVideoDecoderTest()
      : decryptor_(new AesDecryptor(&decryptor_client_)),
        decoder_(new FFmpegVideoDecoder(base::Bind(&Identity<MessageLoop*>,
                                                   &message_loop_))),
        demuxer_(new StrictMock<MockDemuxerStream>()),
        read_cb_(base::Bind(&FFmpegVideoDecoderTest::FrameReady,
                            base::Unretained(this))) {
    CHECK(FFmpegGlue::GetInstance());

    decoder_->set_decryptor(decryptor_.get());

    frame_buffer_.reset(new uint8[kCodedSize.GetArea()]);
    end_of_stream_buffer_ = DecoderBuffer::CreateEOSBuffer();
    i_frame_buffer_ = ReadTestDataFile("vp8-I-frame-320x240");
    corrupt_i_frame_buffer_ = ReadTestDataFile("vp8-corrupt-I-frame");
    encrypted_i_frame_buffer_ = ReadTestDataFile(
        "vp8-encrypted-I-frame-320x240");

    config_.Initialize(kCodecVP8, VIDEO_CODEC_PROFILE_UNKNOWN,
                       kVideoFormat, kCodedSize, kVisibleRect,
                       kFrameRate.num, kFrameRate.den,
                       kAspectRatio.num, kAspectRatio.den,
                       NULL, 0, true);
  }
