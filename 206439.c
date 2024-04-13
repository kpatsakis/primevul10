ssize_t Http2Session::ConsumeHTTP2Data() {
  CHECK_NOT_NULL(stream_buf_.base);
  CHECK_LE(stream_buf_offset_, stream_buf_.len);
  size_t read_len = stream_buf_.len - stream_buf_offset_;

  // multiple side effects.
  Debug(this, "receiving %d bytes [wants data? %d]",
        read_len,
        nghttp2_session_want_read(session_));
  flags_ &= ~SESSION_STATE_NGHTTP2_RECV_PAUSED;
  ssize_t ret =
    nghttp2_session_mem_recv(session_,
                             reinterpret_cast<uint8_t*>(stream_buf_.base) +
                                 stream_buf_offset_,
                             read_len);
  CHECK_NE(ret, NGHTTP2_ERR_NOMEM);

  if (flags_ & SESSION_STATE_NGHTTP2_RECV_PAUSED) {
    CHECK_NE(flags_ & SESSION_STATE_READING_STOPPED, 0);

    CHECK_GT(ret, 0);
    CHECK_LE(static_cast<size_t>(ret), read_len);

    // Mark the remainder of the data as available for later consumption.
    // Even if all bytes were received, a paused stream may delay the
    // nghttp2_on_frame_recv_callback which may have an END_STREAM flag.
    stream_buf_offset_ += ret;
    return ret;
  }

  // We are done processing the current input chunk.
  DecrementCurrentSessionMemory(stream_buf_.len);
  stream_buf_offset_ = 0;
  stream_buf_ab_.Reset();
  free(stream_buf_allocation_.base);
  stream_buf_allocation_ = uv_buf_init(nullptr, 0);
  stream_buf_ = uv_buf_init(nullptr, 0);

  if (ret < 0)
    return ret;

  // Send any data that was queued up while processing the received data.
  if (!IsDestroyed()) {
    SendPendingData();
  }
  return ret;
}