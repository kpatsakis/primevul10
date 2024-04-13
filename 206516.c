void Http2Session::Origin(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Local<Context> context = env->context();
  Http2Session* session;
  ASSIGN_OR_RETURN_UNWRAP(&session, args.Holder());

  Local<String> origin_string = args[0].As<String>();
  int count = args[1]->IntegerValue(context).ToChecked();


  Origins origins(env->isolate(),
                  env->context(),
                  origin_string,
                  count);

  session->Origin(*origins, origins.length());
}