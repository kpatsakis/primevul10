  void flushSb() {
    if (tl_buffer.raw) {
      json_free(tl_buffer.raw);
      tl_buffer.raw = nullptr;
    }
    sb_cap = 0;
    sb_buf.setBuf(nullptr, 0);
    sb_key.setBuf(nullptr, 0);
  }