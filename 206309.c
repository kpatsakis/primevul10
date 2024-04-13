Http2Stream::~Http2Stream() {
  for (nghttp2_header& header : current_headers_) {
    nghttp2_rcbuf_decref(header.name);
    nghttp2_rcbuf_decref(header.value);
  }

  if (session_ == nullptr)
    return;
  Debug(this, "tearing down stream");
  session_->DecrementCurrentSessionMemory(current_headers_length_);
  session_->RemoveStream(this);
  session_ = nullptr;
}