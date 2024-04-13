void Http2Stream::Info(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Local<Context> context = env->context();
  Isolate* isolate = env->isolate();
  Http2Stream* stream;
  ASSIGN_OR_RETURN_UNWRAP(&stream, args.Holder());

  Local<Array> headers = args[0].As<Array>();

  Headers list(isolate, context, headers);
  args.GetReturnValue().Set(stream->SubmitInfo(*list, list.length()));
  Debug(stream, "%d informational headers sent", list.length());
}