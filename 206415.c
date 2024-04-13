void Http2Session::Origin(nghttp2_origin_entry* ov, size_t count) {
  Http2Scope h2scope(this);
  CHECK_EQ(nghttp2_submit_origin(session_, NGHTTP2_FLAG_NONE, ov, count), 0);
}