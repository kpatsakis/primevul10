void Http2Session::Request(const FunctionCallbackInfo<Value>& args) {
  Http2Session* session;
  ASSIGN_OR_RETURN_UNWRAP(&session, args.Holder());
  Environment* env = session->env();
  Local<Context> context = env->context();
  Isolate* isolate = env->isolate();

  Local<Array> headers = args[0].As<Array>();
  int options = args[1]->IntegerValue(context).ToChecked();
  Http2Priority priority(env, args[2], args[3], args[4]);

  Headers list(isolate, context, headers);

  Debug(session, "request submitted");

  int32_t ret = 0;
  Http2Stream* stream =
      session->Http2Session::SubmitRequest(*priority, *list, list.length(),
                                           &ret, options);

  if (ret <= 0 || stream == nullptr) {
    Debug(session, "could not submit request: %s", nghttp2_strerror(ret));
    return args.GetReturnValue().Set(ret);
  }

  Debug(session, "request submitted, new stream id %d", stream->id());
  args.GetReturnValue().Set(stream->object());
}