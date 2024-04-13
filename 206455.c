ShutdownWrap* LibuvStreamWrap::CreateShutdownWrap(Local<Object> object) {
  return new LibuvShutdownWrap(this, object);
}