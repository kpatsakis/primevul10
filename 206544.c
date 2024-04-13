void Http2Session::Http2Settings::Send() {
  Http2Scope h2scope(session_);
  CHECK_EQ(nghttp2_submit_settings(**session_, NGHTTP2_FLAG_NONE,
                                   &entries_[0], count_), 0);
}