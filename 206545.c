void Http2Session::Destroy(const FunctionCallbackInfo<Value>& args) {
  Http2Session* session;
  ASSIGN_OR_RETURN_UNWRAP(&session, args.Holder());
  Debug(session, "destroying session");
  Environment* env = Environment::GetCurrent(args);
  Local<Context> context = env->context();

  uint32_t code = args[0]->Uint32Value(context).ToChecked();
  bool socketDestroyed = args[1]->BooleanValue(context).ToChecked();

  session->Close(code, socketDestroyed);
}