int Http2Stream::SubmitPriority(nghttp2_priority_spec* prispec,
                                bool silent) {
  CHECK(!this->IsDestroyed());
  Http2Scope h2scope(this);
  Debug(this, "sending priority spec");
  int ret = silent ?
      nghttp2_session_change_stream_priority(**session_,
                                             id_, prispec) :
      nghttp2_submit_priority(**session_,
                              NGHTTP2_FLAG_NONE,
                              id_, prispec);
  CHECK_NE(ret, NGHTTP2_ERR_NOMEM);
  return ret;
}