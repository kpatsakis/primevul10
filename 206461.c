void Http2Session::OnStreamAfterWrite(WriteWrap* w, int status) {
  Debug(this, "write finished with status %d", status);

  CHECK_NE(flags_ & SESSION_STATE_WRITE_IN_PROGRESS, 0);
  flags_ &= ~SESSION_STATE_WRITE_IN_PROGRESS;

  // Inform all pending writes about their completion.
  ClearOutgoing(status);

  if ((flags_ & SESSION_STATE_READING_STOPPED) &&
      !(flags_ & SESSION_STATE_WRITE_IN_PROGRESS) &&
      nghttp2_session_want_read(session_)) {
    flags_ &= ~SESSION_STATE_READING_STOPPED;
    stream_->ReadStart();
  }

  // If there is more incoming data queued up, consume it.
  if (stream_buf_offset_ > 0) {
    ConsumeHTTP2Data();
  }

  if (!(flags_ & SESSION_STATE_WRITE_SCHEDULED)) {
    // Schedule a new write if nghttp2 wants to send data.
    MaybeScheduleWrite();
  }
}