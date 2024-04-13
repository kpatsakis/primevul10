void Http2Session::Http2Settings::Init() {
  AliasedBuffer<uint32_t, Uint32Array>& buffer =
      env()->http2_state()->settings_buffer;
  uint32_t flags = buffer[IDX_SETTINGS_COUNT];

  size_t n = 0;

#define GRABSETTING(N, trace)                                                 \
  if (flags & (1 << IDX_SETTINGS_##N)) {                                      \
    uint32_t val = buffer[IDX_SETTINGS_##N];                                  \
    if (session_ != nullptr)                                                  \
      Debug(session_, "setting " trace ": %d\n", val);                        \
    entries_[n++] =                                                           \
        nghttp2_settings_entry {NGHTTP2_SETTINGS_##N, val};                   \
  }

  GRABSETTING(HEADER_TABLE_SIZE, "header table size");
  GRABSETTING(MAX_CONCURRENT_STREAMS, "max concurrent streams");
  GRABSETTING(MAX_FRAME_SIZE, "max frame size");
  GRABSETTING(INITIAL_WINDOW_SIZE, "initial window size");
  GRABSETTING(MAX_HEADER_LIST_SIZE, "max header list size");
  GRABSETTING(ENABLE_PUSH, "enable push");
  GRABSETTING(ENABLE_CONNECT_PROTOCOL, "enable connect protocol");

#undef GRABSETTING

  count_ = n;
}