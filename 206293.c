int Http2Stream::ReadStart() {
  Http2Scope h2scope(this);
  CHECK(!this->IsDestroyed());
  flags_ |= NGHTTP2_STREAM_FLAG_READ_START;
  flags_ &= ~NGHTTP2_STREAM_FLAG_READ_PAUSED;

  Debug(this, "reading starting");

  // Tell nghttp2 about our consumption of the data that was handed
  // off to JS land.
  nghttp2_session_consume_stream(**session_,
                                 id_,
                                 inbound_consumed_data_while_paused_);
  inbound_consumed_data_while_paused_ = 0;

  return 0;
}