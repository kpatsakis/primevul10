void Http2Session::Goaway(uint32_t code,
                          int32_t lastStreamID,
                          uint8_t* data,
                          size_t len) {
  if (IsDestroyed())
    return;

  Http2Scope h2scope(this);
  // the last proc stream id is the most recently created Http2Stream.
  if (lastStreamID <= 0)
    lastStreamID = nghttp2_session_get_last_proc_stream_id(session_);
  Debug(this, "submitting goaway");
  nghttp2_submit_goaway(session_, NGHTTP2_FLAG_NONE,
                        lastStreamID, code, data, len);
}