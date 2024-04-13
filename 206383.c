bool Http2Stream::AddHeader(nghttp2_rcbuf* name,
                            nghttp2_rcbuf* value,
                            uint8_t flags) {
  CHECK(!this->IsDestroyed());
  if (this->statistics_.first_header == 0)
    this->statistics_.first_header = uv_hrtime();
  size_t name_len = nghttp2_rcbuf_get_buf(name).len;
  if (name_len == 0 && !IsReverted(SECURITY_REVERT_CVE_2019_9516)) {
    return true;  // Ignore headers with empty names.
  }
  size_t value_len = nghttp2_rcbuf_get_buf(value).len;
  size_t length = name_len + value_len + 32;
  // A header can only be added if we have not exceeded the maximum number
  // of headers and the session has memory available for it.
  if (!session_->IsAvailableSessionMemory(length) ||
      current_headers_.size() == max_header_pairs_ ||
      current_headers_length_ + length > max_header_length_) {
    return false;
  }
  nghttp2_header header;
  header.name = name;
  header.value = value;
  header.flags = flags;
  current_headers_.push_back(header);
  nghttp2_rcbuf_incref(name);
  nghttp2_rcbuf_incref(value);
  current_headers_length_ += length;
  session_->IncrementCurrentSessionMemory(length);
  return true;
}