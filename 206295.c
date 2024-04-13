void Http2Stream::Close(int32_t code) {
  CHECK(!this->IsDestroyed());
  flags_ |= NGHTTP2_STREAM_FLAG_CLOSED;
  code_ = code;
  Debug(this, "closed with code %d", code);
}