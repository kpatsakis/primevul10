inline StreamBase::StreamBase(Environment* env) : env_(env) {
  PushStreamListener(&default_listener_);
}