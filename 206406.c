inline WriteWrap* StreamBase::CreateWriteWrap(
    v8::Local<v8::Object> object) {
  return new SimpleWriteWrap<AsyncWrap>(this, object);
}