Http2Session::Http2Ping::Http2Ping(Http2Session* session, Local<Object> obj)
    : AsyncWrap(session->env(), obj, AsyncWrap::PROVIDER_HTTP2PING),
      session_(session),
      startTime_(uv_hrtime()) {}