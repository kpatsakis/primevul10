void StreamListener::OnStreamAfterShutdown(ShutdownWrap* w, int status) {
  CHECK_NOT_NULL(previous_listener_);
  previous_listener_->OnStreamAfterShutdown(w, status);
}