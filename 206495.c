  static void* H2Realloc(void* ptr, size_t size, void* user_data) {
    Http2Session* session = static_cast<Http2Session*>(user_data);
    size_t previous_size = 0;
    char* original_ptr = nullptr;

    // We prepend each allocated buffer with a size_t containing the full
    // size of the allocation.
    if (size > 0) size += sizeof(size_t);

    if (ptr != nullptr) {
      // We are free()ing or re-allocating.
      original_ptr = static_cast<char*>(ptr) - sizeof(size_t);
      previous_size = *reinterpret_cast<size_t*>(original_ptr);
      // This means we called StopTracking() on this pointer before.
      if (previous_size == 0) {
        // Fall back to the standard Realloc() function.
        char* ret = UncheckedRealloc(original_ptr, size);
        if (ret != nullptr)
          ret += sizeof(size_t);
        return ret;
      }
    }
    CHECK_GE(session->current_nghttp2_memory_, previous_size);

    // TODO(addaleax): Add the following, and handle NGHTTP2_ERR_NOMEM properly
    // everywhere:
    //
    // if (size > previous_size &&
    //     !session->IsAvailableSessionMemory(size - previous_size)) {
    //  return nullptr;
    //}

    char* mem = UncheckedRealloc(original_ptr, size);

    if (mem != nullptr) {
      // Adjust the memory info counter.
      session->current_nghttp2_memory_ += size - previous_size;
      *reinterpret_cast<size_t*>(mem) = size;
      mem += sizeof(size_t);
    } else if (size == 0) {
      session->current_nghttp2_memory_ -= previous_size;
    }

    return mem;
  }