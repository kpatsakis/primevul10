void Http2Session::New(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  CHECK(args.IsConstructCall());
  int val = args[0]->IntegerValue(env->context()).ToChecked();
  nghttp2_session_type type = static_cast<nghttp2_session_type>(val);
  Http2Session* session = new Http2Session(env, args.This(), type);
  session->get_async_id();  // avoid compiler warning
  Debug(session, "session created");
}