uv_buf_t StreamResource::EmitAlloc(size_t suggested_size) {
  DebugSealHandleScope seal_handle_scope;
  return listener_->OnStreamAlloc(suggested_size);
}