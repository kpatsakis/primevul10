Http2Stream* Http2Stream::New(Http2Session* session,
                              int32_t id,
                              nghttp2_headers_category category,
                              int options) {
  Local<Object> obj;
  if (!session->env()
           ->http2stream_constructor_template()
           ->NewInstance(session->env()->context())
           .ToLocal(&obj)) {
    return nullptr;
  }
  return new Http2Stream(session, obj, id, category, options);
}