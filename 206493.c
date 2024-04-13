void Http2Session::Close(uint32_t code, bool socket_closed) {
  Debug(this, "closing session");

  if (flags_ & SESSION_STATE_CLOSING)
    return;
  flags_ |= SESSION_STATE_CLOSING;

  // Stop reading on the i/o stream
  if (stream_ != nullptr) {
    flags_ |= SESSION_STATE_READING_STOPPED;
    stream_->ReadStop();
  }

  // If the socket is not closed, then attempt to send a closing GOAWAY
  // frame. There is no guarantee that this GOAWAY will be received by
  // the peer but the HTTP/2 spec recommends sending it anyway. We'll
  // make a best effort.
  if (!socket_closed) {
    Debug(this, "terminating session with code %d", code);
    CHECK_EQ(nghttp2_session_terminate_session(session_, code), 0);
    SendPendingData();
  } else if (stream_ != nullptr) {
    stream_->RemoveStreamListener(this);
  }

  flags_ |= SESSION_STATE_CLOSED;

  // If there are outstanding pings, those will need to be canceled, do
  // so on the next iteration of the event loop to avoid calling out into
  // javascript since this may be called during garbage collection.
  while (!outstanding_pings_.empty()) {
    Http2Session::Http2Ping* ping = PopPing();
    env()->SetImmediate([](Environment* env, void* data) {
      static_cast<Http2Session::Http2Ping*>(data)->Done(false);
    }, static_cast<void*>(ping));
  }

  statistics_.end_time = uv_hrtime();
  EmitStatistics();
}