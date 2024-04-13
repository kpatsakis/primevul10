WriteWrap* StreamBase::CreateWriteWrap(
    Local<Object> object) {
  auto* wrap = new SimpleWriteWrap<AsyncWrap>(this, object);
  wrap->MakeWeak();
  return wrap;
}