int StreamResource::DoTryWrite(uv_buf_t** bufs, size_t* count) {
  // No TryWrite by default
  return 0;
}