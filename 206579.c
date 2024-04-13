ShutdownWrap* StreamBase::CreateShutdownWrap(
    Local<Object> object) {
  auto* wrap = new SimpleShutdownWrap<AsyncWrap>(this, object);
  wrap->MakeWeak();
  return wrap;
}