inline uv_buf_t StreamResource::EmitAlloc(size_t suggested_size) {
#ifdef DEBUG
  v8::SealHandleScope handle_scope(v8::Isolate::GetCurrent());
#endif
  return listener_->OnStreamAlloc(suggested_size);
}