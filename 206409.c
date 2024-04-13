void Http2Stream::Priority(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Local<Context> context = env->context();
  Http2Stream* stream;
  ASSIGN_OR_RETURN_UNWRAP(&stream, args.Holder());

  Http2Priority priority(env, args[0], args[1], args[2]);
  bool silent = args[3]->BooleanValue(context).ToChecked();

  CHECK_EQ(stream->SubmitPriority(*priority, silent), 0);
  Debug(stream, "priority submitted");
}