void StreamListener::PassReadErrorToPreviousListener(ssize_t nread) {
  CHECK_NOT_NULL(previous_listener_);
  previous_listener_->OnStreamRead(nread, uv_buf_init(nullptr, 0));
}