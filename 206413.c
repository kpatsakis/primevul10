int Http2Stream::SubmitResponse(nghttp2_nv* nva, size_t len, int options) {
  CHECK(!this->IsDestroyed());
  Http2Scope h2scope(this);
  Debug(this, "submitting response");
  if (options & STREAM_OPTION_GET_TRAILERS)
    flags_ |= NGHTTP2_STREAM_FLAG_TRAILERS;

  if (!IsWritable())
    options |= STREAM_OPTION_EMPTY_PAYLOAD;

  Http2Stream::Provider::Stream prov(this, options);
  int ret = nghttp2_submit_response(**session_, id_, nva, len, *prov);
  CHECK_NE(ret, NGHTTP2_ERR_NOMEM);
  return ret;
}