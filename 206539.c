Http2Session::Http2Settings::Http2Settings(Environment* env,
                                           Http2Session* session,
                                           Local<Object> obj,
                                           uint64_t start_time)
    : AsyncWrap(env, obj, PROVIDER_HTTP2SETTINGS),
      session_(session),
      startTime_(start_time) {
  Init();
}