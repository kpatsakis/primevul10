WriteWrap* LibuvStreamWrap::CreateWriteWrap(Local<Object> object) {
  return new LibuvWriteWrap(this, object);
}