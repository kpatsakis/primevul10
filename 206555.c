Origins::Origins(Isolate* isolate,
                 Local<Context> context,
                 Local<String> origin_string,
                 size_t origin_count) : count_(origin_count) {
  int origin_string_len = origin_string->Length();
  if (count_ == 0) {
    CHECK_EQ(origin_string_len, 0);
    return;
  }

  // Allocate a single buffer with count_ nghttp2_nv structs, followed
  // by the raw header data as passed from JS. This looks like:
  // | possible padding | nghttp2_nv | nghttp2_nv | ... | header contents |
  buf_.AllocateSufficientStorage((alignof(nghttp2_origin_entry) - 1) +
                                 count_ * sizeof(nghttp2_origin_entry) +
                                 origin_string_len);

  // Make sure the start address is aligned appropriately for an nghttp2_nv*.
  char* start = reinterpret_cast<char*>(
      ROUND_UP(reinterpret_cast<uintptr_t>(*buf_),
               alignof(nghttp2_origin_entry)));
  char* origin_contents = start + (count_ * sizeof(nghttp2_origin_entry));
  nghttp2_origin_entry* const nva =
      reinterpret_cast<nghttp2_origin_entry*>(start);

  CHECK_LE(origin_contents + origin_string_len, *buf_ + buf_.length());
  CHECK_EQ(origin_string->WriteOneByte(
               isolate,
               reinterpret_cast<uint8_t*>(origin_contents),
               0,
               origin_string_len,
               String::NO_NULL_TERMINATION),
           origin_string_len);

  size_t n = 0;
  char* p;
  for (p = origin_contents; p < origin_contents + origin_string_len; n++) {
    if (n >= count_) {
      static uint8_t zero = '\0';
      nva[0].origin = &zero;
      nva[0].origin_len = 1;
      count_ = 1;
      return;
    }

    nva[n].origin = reinterpret_cast<uint8_t*>(p);
    nva[n].origin_len = strlen(p);
    p += nva[n].origin_len + 1;
  }
}