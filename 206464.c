uv_buf_t Http2StreamListener::OnStreamAlloc(size_t size) {
  // See the comments in Http2Session::OnDataChunkReceived
  // (which is the only possible call site for this method).
  return uv_buf_init(nullptr, size);
}