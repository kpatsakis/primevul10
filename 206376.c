Http2Session::~Http2Session() {
  CHECK_EQ(flags_ & SESSION_STATE_HAS_SCOPE, 0);
  Debug(this, "freeing nghttp2 session");
  for (const auto& stream : streams_)
    stream.second->session_ = nullptr;
  nghttp2_session_del(session_);
  CHECK_EQ(current_nghttp2_memory_, 0);
  free(stream_buf_allocation_.base);
}