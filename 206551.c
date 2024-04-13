void Http2Session::Settings(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Http2Session* session;
  ASSIGN_OR_RETURN_UNWRAP(&session, args.Holder());

  Local<Object> obj;
  if (!env->http2settings_constructor_template()
           ->NewInstance(env->context())
           .ToLocal(&obj)) {
    return;
  }
  if (obj->Set(env->context(), env->ondone_string(), args[0]).IsNothing())
    return;

  Http2Session::Http2Settings* settings =
      new Http2Settings(session->env(), session, obj, 0);
  if (!session->AddSettings(settings)) {
    settings->Done(false);
    return args.GetReturnValue().Set(false);
  }

  settings->Send();
  args.GetReturnValue().Set(true);
}