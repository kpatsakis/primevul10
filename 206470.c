void Http2Stream::Destroy() {
  // Do nothing if this stream instance is already destroyed
  if (IsDestroyed())
    return;
  if (session_->HasPendingRstStream(id_))
    FlushRstStream();
  flags_ |= NGHTTP2_STREAM_FLAG_DESTROYED;

  Debug(this, "destroying stream");

  // Wait until the start of the next loop to delete because there
  // may still be some pending operations queued for this stream.
  env()->SetImmediate([](Environment* env, void* data) {
    Http2Stream* stream = static_cast<Http2Stream*>(data);
    // Free any remaining outgoing data chunks here. This should be done
    // here because it's possible for destroy to have been called while
    // we still have queued outbound writes.
    while (!stream->queue_.empty()) {
      nghttp2_stream_write& head = stream->queue_.front();
      if (head.req_wrap != nullptr)
        head.req_wrap->Done(UV_ECANCELED);
      stream->queue_.pop();
    }

    // We can destroy the stream now if there are no writes for it
    // already on the socket. Otherwise, we'll wait for the garbage collector
    // to take care of cleaning up.
    if (stream->session() == nullptr ||
        !stream->session()->HasWritesOnSocketForStream(stream))
      delete stream;
  }, this, this->object());

  statistics_.end_time = uv_hrtime();
  session_->statistics_.stream_average_duration =
      ((statistics_.end_time - statistics_.start_time) /
          session_->statistics_.stream_count) / 1e6;
  EmitStatistics();
}