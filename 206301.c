void Http2Session::Http2Settings::Update(Environment* env,
                                         Http2Session* session,
                                         get_setting fn) {
  AliasedBuffer<uint32_t, Uint32Array>& buffer =
      env->http2_state()->settings_buffer;
  buffer[IDX_SETTINGS_HEADER_TABLE_SIZE] =
      fn(**session, NGHTTP2_SETTINGS_HEADER_TABLE_SIZE);
  buffer[IDX_SETTINGS_MAX_CONCURRENT_STREAMS] =
      fn(**session, NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS);
  buffer[IDX_SETTINGS_INITIAL_WINDOW_SIZE] =
      fn(**session, NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE);
  buffer[IDX_SETTINGS_MAX_FRAME_SIZE] =
      fn(**session, NGHTTP2_SETTINGS_MAX_FRAME_SIZE);
  buffer[IDX_SETTINGS_MAX_HEADER_LIST_SIZE] =
      fn(**session, NGHTTP2_SETTINGS_MAX_HEADER_LIST_SIZE);
  buffer[IDX_SETTINGS_ENABLE_PUSH] =
      fn(**session, NGHTTP2_SETTINGS_ENABLE_PUSH);
  buffer[IDX_SETTINGS_ENABLE_CONNECT_PROTOCOL] =
      fn(**session, NGHTTP2_SETTINGS_ENABLE_CONNECT_PROTOCOL);
}