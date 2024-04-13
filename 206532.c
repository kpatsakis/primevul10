Http2Stream* Http2Stream::SubmitPushPromise(nghttp2_nv* nva,
                                            size_t len,
                                            int32_t* ret,
                                            int options) {
  CHECK(!this->IsDestroyed());
  Http2Scope h2scope(this);
  Debug(this, "sending push promise");
  *ret = nghttp2_submit_push_promise(**session_, NGHTTP2_FLAG_NONE,
                                     id_, nva, len, nullptr);
  CHECK_NE(*ret, NGHTTP2_ERR_NOMEM);
  Http2Stream* stream = nullptr;
  if (*ret > 0)
    stream = Http2Stream::New(session_, *ret, NGHTTP2_HCAT_HEADERS, options);

  return stream;
}