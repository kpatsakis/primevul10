void TLSWrap::EncOut() {
  Debug(this, "Trying to write encrypted output");

  // Ignore cycling data if ClientHello wasn't yet parsed
  if (!hello_parser_.IsEnded()) {
    Debug(this, "Returning from EncOut(), hello_parser_ active");
    return;
  }

  // Write in progress
  if (write_size_ != 0) {
    Debug(this, "Returning from EncOut(), write currently in progress");
    return;
  }

  // Wait for `newSession` callback to be invoked
  if (is_awaiting_new_session()) {
    Debug(this, "Returning from EncOut(), awaiting new session");
    return;
  }

  // Split-off queue
  if (established_ && current_write_ != nullptr) {
    Debug(this, "EncOut() setting write_callback_scheduled_");
    write_callback_scheduled_ = true;
  }

  if (ssl_ == nullptr) {
    Debug(this, "Returning from EncOut(), ssl_ == nullptr");
    return;
  }

  // No encrypted output ready to write to the underlying stream.
  if (BIO_pending(enc_out_) == 0) {
    Debug(this, "No pending encrypted output");
    if (pending_cleartext_input_.size() == 0)
      InvokeQueued(0);
    return;
  }

  char* data[kSimultaneousBufferCount];
  size_t size[arraysize(data)];
  size_t count = arraysize(data);
  write_size_ = crypto::NodeBIO::FromBIO(enc_out_)->PeekMultiple(data,
                                                                 size,
                                                                 &count);
  CHECK(write_size_ != 0 && count != 0);

  uv_buf_t buf[arraysize(data)];
  uv_buf_t* bufs = buf;
  for (size_t i = 0; i < count; i++)
    buf[i] = uv_buf_init(data[i], size[i]);

  Debug(this, "Writing %zu buffers to the underlying stream", count);
  StreamWriteResult res = underlying_stream()->Write(bufs, count);
  if (res.err != 0) {
    InvokeQueued(res.err);
    return;
  }

  NODE_COUNT_NET_BYTES_SENT(write_size_);

  if (!res.async) {
    Debug(this, "Write finished synchronously");
    HandleScope handle_scope(env()->isolate());

    // Simulate asynchronous finishing, TLS cannot handle this at the moment.
    env()->SetImmediate([](Environment* env, void* data) {
      static_cast<TLSWrap*>(data)->OnStreamAfterWrite(nullptr, 0);
    }, this, object());
  }
}