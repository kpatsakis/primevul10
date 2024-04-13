  static void StopTracking(Http2Session* session, void* ptr) {
    size_t* original_ptr = reinterpret_cast<size_t*>(
        static_cast<char*>(ptr) - sizeof(size_t));
    session->current_nghttp2_memory_ -= *original_ptr;
    *original_ptr = 0;
  }