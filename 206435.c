void Http2Stream::SubmitRstStream(const uint32_t code) {
  CHECK(!this->IsDestroyed());
  code_ = code;
  // If possible, force a purge of any currently pending data here to make sure
  // it is sent before closing the stream. If it returns non-zero then we need
  // to wait until the current write finishes and try again to avoid nghttp2
  // behaviour where it prioritizes RstStream over everything else.
  if (session_->SendPendingData() != 0) {
    session_->AddPendingRstStream(id_);
    return;
  }

  FlushRstStream();
}