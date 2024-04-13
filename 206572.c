void StreamResource::PushStreamListener(StreamListener* listener) {
  CHECK_NOT_NULL(listener);
  CHECK_NULL(listener->stream_);

  listener->previous_listener_ = listener_;
  listener->stream_ = this;

  listener_ = listener;
}