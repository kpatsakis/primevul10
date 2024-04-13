  void EnterEndOfStreamState() {
    scoped_refptr<VideoFrame> video_frame;
    VideoDecoder::DecoderStatus status;
    Read(&status, &video_frame);
    EXPECT_EQ(status, VideoDecoder::kOk);
    ASSERT_TRUE(video_frame);
    EXPECT_TRUE(video_frame->IsEndOfStream());
  }
