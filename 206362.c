void LibuvStreamWrap::OnUvRead(ssize_t nread, const uv_buf_t* buf) {
  HandleScope scope(env()->isolate());
  Context::Scope context_scope(env()->context());
  uv_handle_type type = UV_UNKNOWN_HANDLE;

  if (is_named_pipe_ipc() &&
      uv_pipe_pending_count(reinterpret_cast<uv_pipe_t*>(stream())) > 0) {
    type = uv_pipe_pending_type(reinterpret_cast<uv_pipe_t*>(stream()));
  }

  // We should not be getting this callback if someone has already called
  // uv_close() on the handle.
  CHECK_EQ(persistent().IsEmpty(), false);

  if (nread > 0) {
    if (is_tcp()) {
      NODE_COUNT_NET_BYTES_RECV(nread);
    } else if (is_named_pipe()) {
      NODE_COUNT_PIPE_BYTES_RECV(nread);
    }

    Local<Object> pending_obj;

    if (type == UV_TCP) {
      pending_obj = AcceptHandle<TCPWrap>(env(), this);
    } else if (type == UV_NAMED_PIPE) {
      pending_obj = AcceptHandle<PipeWrap>(env(), this);
    } else if (type == UV_UDP) {
      pending_obj = AcceptHandle<UDPWrap>(env(), this);
    } else {
      CHECK_EQ(type, UV_UNKNOWN_HANDLE);
    }

    if (!pending_obj.IsEmpty()) {
      object()->Set(env()->context(),
                    env()->pending_handle_string(),
                    pending_obj).FromJust();
    }
  }

  EmitRead(nread, *buf);
}