void CustomBufferJSListener::OnStreamRead(ssize_t nread, const uv_buf_t& buf) {
  CHECK_NOT_NULL(stream_);

  StreamBase* stream = static_cast<StreamBase*>(stream_);
  Environment* env = stream->stream_env();
  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(env->context());

  // To deal with the case where POLLHUP is received and UV_EOF is returned, as
  // libuv returns an empty buffer (on unices only).
  if (nread == UV_EOF && buf.base == nullptr) {
    stream->CallJSOnreadMethod(nread, Local<ArrayBuffer>());
    return;
  }

  CHECK_EQ(buf.base, buffer_.base);

  MaybeLocal<Value> ret = stream->CallJSOnreadMethod(nread,
                             Local<ArrayBuffer>(),
                             0,
                             StreamBase::SKIP_NREAD_CHECKS);
  Local<Value> next_buf_v;
  if (ret.ToLocal(&next_buf_v) && !next_buf_v->IsUndefined()) {
    buffer_.base = Buffer::Data(next_buf_v);
    buffer_.len = Buffer::Length(next_buf_v);
  }
}