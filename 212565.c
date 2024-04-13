  void initSb(int length) {
    if (UNLIKELY(length >= sb_cap)) {
      // No decoded string in the output can use more bytes than input size.
      const auto new_cap = length + 1;
      size_t bufSize = length <= RuntimeOption::EvalSimpleJsonMaxLength ?
        SimpleParser::BufferBytesForLength(length) :
        new_cap * 2;
      if (tl_buffer.raw) {
        json_free(tl_buffer.raw);
        tl_buffer.raw = nullptr;
      }
      sb_cap = 0;
      if (!tl_heap->preAllocOOM(bufSize)) {
        tl_buffer.raw = (char*)json_malloc(bufSize);
        if (!tl_buffer.raw) tl_heap->forceOOM();
      }
      check_non_safepoint_surprise();
      always_assert(tl_buffer.raw);
      sb_buf.setBuf(tl_buffer.raw, new_cap);
      sb_key.setBuf(tl_buffer.raw + new_cap, new_cap);
      // Set new capacity if and ony if allocations succeed.
      sb_cap = new_cap;
    } else {
      sb_buf.clear();
      sb_key.clear();
    }
  }