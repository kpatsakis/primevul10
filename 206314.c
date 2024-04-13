void Http2Session::OnStreamRead(ssize_t nread, const uv_buf_t& buf_) {
  HandleScope handle_scope(env()->isolate());
  Context::Scope context_scope(env()->context());
  Http2Scope h2scope(this);
  CHECK_NOT_NULL(stream_);
  Debug(this, "receiving %d bytes", nread);

  // Only pass data on if nread > 0
  if (nread <= 0) {
    free(buf_.base);
    if (nread < 0) {
      PassReadErrorToPreviousListener(nread);
    }
    return;
  }

  uv_buf_t buf = buf_;

  statistics_.data_received += nread;

  if (UNLIKELY(stream_buf_offset_ > 0)) {
    // This is a very unlikely case, and should only happen if the ReadStart()
    // call in OnStreamAfterWrite() immediately provides data. If that does
    // happen, we concatenate the data we received with the already-stored
    // pending input data, slicing off the already processed part.
    char* new_buf = Malloc(stream_buf_.len - stream_buf_offset_ + nread);
    memcpy(new_buf,
           stream_buf_.base + stream_buf_offset_,
           stream_buf_.len - stream_buf_offset_);
    memcpy(new_buf + stream_buf_.len - stream_buf_offset_,
           buf.base,
           nread);
    free(buf.base);
    nread = stream_buf_.len - stream_buf_offset_ + nread;
    buf = uv_buf_init(new_buf, nread);
    stream_buf_offset_ = 0;
    stream_buf_ab_.Reset();

    // We have now fully processed the stream_buf_ input chunk (by moving the
    // remaining part into buf, which will be accounted for below).
    DecrementCurrentSessionMemory(stream_buf_.len);
  }

  // Shrink to the actual amount of used data.
  buf.base = Realloc(buf.base, nread);
  IncrementCurrentSessionMemory(nread);

  // Remember the current buffer, so that OnDataChunkReceived knows the
  // offset of a DATA frame's data into the socket read buffer.
  stream_buf_ = uv_buf_init(buf.base, nread);

  // Verify that currently: There is memory allocated into which
  // the data has been read, and that memory buffer is at least as large
  // as the amount of data we have read, but we have not yet made an
  // ArrayBuffer out of it.
  CHECK_LE(static_cast<size_t>(nread), stream_buf_.len);

  Isolate* isolate = env()->isolate();

  // Store this so we can create an ArrayBuffer for read data from it.
  // DATA frames will be emitted as slices of that ArrayBuffer to avoid having
  // to copy memory.
  stream_buf_allocation_ = buf;

  ssize_t ret = ConsumeHTTP2Data();

  if (UNLIKELY(ret < 0)) {
    Debug(this, "fatal error receiving data: %d", ret);
    Local<Value> arg = Integer::New(isolate, ret);
    MakeCallback(env()->error_string(), 1, &arg);
    return;
  }

  MaybeStopReading();
}