void EmitToJSStreamListener::OnStreamRead(ssize_t nread, const uv_buf_t& buf_) {
  CHECK_NOT_NULL(stream_);
  StreamBase* stream = static_cast<StreamBase*>(stream_);
  Environment* env = stream->stream_env();
  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(env->context());
  AllocatedBuffer buf(env, buf_);

  if (nread <= 0)  {
    if (nread < 0)
      stream->CallJSOnreadMethod(nread, Local<ArrayBuffer>());
    return;
  }

  CHECK_LE(static_cast<size_t>(nread), buf.size());
  buf.Resize(nread);

  stream->CallJSOnreadMethod(nread, buf.ToArrayBuffer());
}