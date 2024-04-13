int Http2Stream::DoWrite(WriteWrap* req_wrap,
                         uv_buf_t* bufs,
                         size_t nbufs,
                         uv_stream_t* send_handle) {
  CHECK_NULL(send_handle);
  Http2Scope h2scope(this);
  if (!is_writable() || is_destroyed()) {
    req_wrap->Done(UV_EOF);
    return 0;
  }
  Debug(this, "queuing %d buffers to send", nbufs);
  for (size_t i = 0; i < nbufs; ++i) {
    // Store the req_wrap on the last write info in the queue, so that it is
    // only marked as finished once all buffers associated with it are finished.
    queue_.emplace(NgHttp2StreamWrite {
      BaseObjectPtr<AsyncWrap>(
          i == nbufs - 1 ? req_wrap->GetAsyncWrap() : nullptr),
      bufs[i]
    });
    IncrementAvailableOutboundLength(bufs[i].len);
  }
  CHECK_NE(nghttp2_session_resume_data(
      session_->session(),
      id_), NGHTTP2_ERR_NOMEM);
  return 0;
}