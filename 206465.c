void Http2Session::MaybeStopReading() {
  if (flags_ & SESSION_STATE_READING_STOPPED) return;
  int want_read = nghttp2_session_want_read(session_);
  Debug(this, "wants read? %d", want_read);
  if (want_read == 0 || (flags_ & SESSION_STATE_WRITE_IN_PROGRESS)) {
    flags_ |= SESSION_STATE_READING_STOPPED;
    stream_->ReadStop();
  }
}