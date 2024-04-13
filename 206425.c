inline void StreamResource::EmitRead(ssize_t nread, const uv_buf_t& buf) {
#ifdef DEBUG
  v8::SealHandleScope handle_scope(v8::Isolate::GetCurrent());
#endif
  if (nread > 0)
    bytes_read_ += static_cast<uint64_t>(nread);
  listener_->OnStreamRead(nread, buf);
}