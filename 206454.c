int JSStream::ReadStart() {
  HandleScope scope(env()->isolate());
  Context::Scope context_scope(env()->context());
  TryCatch try_catch(env()->isolate());
  Local<Value> value;
  int value_int = UV_EPROTO;
  if (!MakeCallback(env()->onreadstart_string(), 0, nullptr).ToLocal(&value) ||
      !value->Int32Value(env()->context()).To(&value_int)) {
    if (!try_catch.HasTerminated())
      FatalException(env()->isolate(), try_catch);
  }
  return value_int;
}