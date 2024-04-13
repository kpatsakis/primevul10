void Http2Stream::PushPromise(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Local<Context> context = env->context();
  Isolate* isolate = env->isolate();
  Http2Stream* parent;
  ASSIGN_OR_RETURN_UNWRAP(&parent, args.Holder());

  Local<Array> headers = args[0].As<Array>();
  int options = args[1]->IntegerValue(context).ToChecked();

  Headers list(isolate, context, headers);

  Debug(parent, "creating push promise");

  int32_t ret = 0;
  Http2Stream* stream = parent->SubmitPushPromise(*list, list.length(),
                                                  &ret, options);
  if (ret <= 0 || stream == nullptr) {
    Debug(parent, "failed to create push stream: %d", ret);
    return args.GetReturnValue().Set(ret);
  }
  Debug(parent, "push stream %d created", stream->id());
  args.GetReturnValue().Set(stream->object());
}