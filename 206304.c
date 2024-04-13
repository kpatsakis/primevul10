Http2Stream* Http2Session::SubmitRequest(
    nghttp2_priority_spec* prispec,
    nghttp2_nv* nva,
    size_t len,
    int32_t* ret,
    int options) {
  Debug(this, "submitting request");
  Http2Scope h2scope(this);
  Http2Stream* stream = nullptr;
  Http2Stream::Provider::Stream prov(options);
  *ret = nghttp2_submit_request(session_, prispec, nva, len, *prov, nullptr);
  CHECK_NE(*ret, NGHTTP2_ERR_NOMEM);
  if (LIKELY(*ret > 0))
    stream = Http2Stream::New(this, *ret, NGHTTP2_HCAT_HEADERS, options);
  return stream;
}