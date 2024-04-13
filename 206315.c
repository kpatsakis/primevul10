int Http2Stream::SubmitInfo(nghttp2_nv* nva, size_t len) {
  CHECK(!this->IsDestroyed());
  Http2Scope h2scope(this);
  Debug(this, "sending %d informational headers", len);
  int ret = nghttp2_submit_headers(**session_,
                                   NGHTTP2_FLAG_NONE,
                                   id_, nullptr,
                                   nva, len, nullptr);
  CHECK_NE(ret, NGHTTP2_ERR_NOMEM);
  return ret;
}