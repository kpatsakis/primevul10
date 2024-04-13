bool JSStream::IsClosing() {
  HandleScope scope(env()->isolate());
  Context::Scope context_scope(env()->context());
  TryCatch try_catch(env()->isolate());
  Local<Value> value;
  if (!MakeCallback(env()->isclosing_string(), 0, nullptr).ToLocal(&value)) {
    if (!try_catch.HasTerminated())
      FatalException(env()->isolate(), try_catch);
    return true;
  }
  return value->IsTrue();
}