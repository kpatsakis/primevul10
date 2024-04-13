int Http2Stream::ReadStop() {
  CHECK(!this->IsDestroyed());
  if (!IsReading())
    return 0;
  flags_ |= NGHTTP2_STREAM_FLAG_READ_PAUSED;
  Debug(this, "reading stopped");
  return 0;
}