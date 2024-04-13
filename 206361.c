inline ShutdownWrap* StreamBase::CreateShutdownWrap(
    v8::Local<v8::Object> object) {
  return new SimpleShutdownWrap<AsyncWrap>(this, object);
}