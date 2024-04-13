void Http2Stream::FlushRstStream() {
  if (IsDestroyed())
    return;
  Http2Scope h2scope(this);
  CHECK_EQ(nghttp2_submit_rst_stream(**session_, NGHTTP2_FLAG_NONE,
                                     id_, code_), 0);
}