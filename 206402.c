int JSStream::DoShutdown(ShutdownWrap* req_wrap) {
  HandleScope scope(env()->isolate());
  Context::Scope context_scope(env()->context());

  Local<Value> argv[] = {
    req_wrap->object()
  };

  TryCatch try_catch(env()->isolate());
  Local<Value> value;
  int value_int = UV_EPROTO;
  if (!MakeCallback(env()->onshutdown_string(),
                    arraysize(argv),
                    argv).ToLocal(&value) ||
      !value->Int32Value(env()->context()).To(&value_int)) {
    if (!try_catch.HasTerminated())
      FatalException(env()->isolate(), try_catch);
  }
  return value_int;
}