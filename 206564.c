uv_buf_t CustomBufferJSListener::OnStreamAlloc(size_t suggested_size) {
  return buffer_;
}