uv_buf_t EmitToJSStreamListener::OnStreamAlloc(size_t suggested_size) {
  CHECK_NOT_NULL(stream_);
  Environment* env = static_cast<StreamBase*>(stream_)->stream_env();
  return AllocatedBuffer::AllocateManaged(env, suggested_size).release();
}