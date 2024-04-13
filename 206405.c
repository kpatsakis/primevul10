LibuvStreamWrap::LibuvStreamWrap(Environment* env,
                                 Local<Object> object,
                                 uv_stream_t* stream,
                                 AsyncWrap::ProviderType provider)
    : HandleWrap(env,
                 object,
                 reinterpret_cast<uv_handle_t*>(stream),
                 provider),
      StreamBase(env),
      stream_(stream) {
}