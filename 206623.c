void StreamListener::OnStreamAfterWrite(WriteWrap* w, int status) {
  CHECK_NOT_NULL(previous_listener_);
  previous_listener_->OnStreamAfterWrite(w, status);
}