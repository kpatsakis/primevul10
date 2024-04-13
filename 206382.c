int Http2Session::OnInvalidHeader(nghttp2_session* session,
                                  const nghttp2_frame* frame,
                                  nghttp2_rcbuf* name,
                                  nghttp2_rcbuf* value,
                                  uint8_t flags,
                                  void* user_data) {
  // Ignore invalid header fields by default.
  return 0;
}