int Http2Stream::DoShutdown(ShutdownWrap* req_wrap) {
  if (IsDestroyed())
    return UV_EPIPE;

  {
    Http2Scope h2scope(this);
    flags_ |= NGHTTP2_STREAM_FLAG_SHUT;
    CHECK_NE(nghttp2_session_resume_data(**session_, id_),
             NGHTTP2_ERR_NOMEM);
    Debug(this, "writable side shutdown");
  }
  return 1;
}