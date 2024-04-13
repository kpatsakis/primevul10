void LibuvStreamWrap::Initialize(Local<Object> target,
                                 Local<Value> unused,
                                 Local<Context> context) {
  Environment* env = Environment::GetCurrent(context);

  auto is_construct_call_callback =
      [](const FunctionCallbackInfo<Value>& args) {
    CHECK(args.IsConstructCall());
    StreamReq::ResetObject(args.This());
  };
  Local<FunctionTemplate> sw =
      FunctionTemplate::New(env->isolate(), is_construct_call_callback);
  sw->InstanceTemplate()->SetInternalFieldCount(StreamReq::kStreamReqField + 1);
  Local<String> wrapString =
      FIXED_ONE_BYTE_STRING(env->isolate(), "ShutdownWrap");
  sw->SetClassName(wrapString);
  sw->Inherit(AsyncWrap::GetConstructorTemplate(env));
  target->Set(wrapString, sw->GetFunction(env->context()).ToLocalChecked());
  env->set_shutdown_wrap_template(sw->InstanceTemplate());

  Local<FunctionTemplate> ww =
      FunctionTemplate::New(env->isolate(), is_construct_call_callback);
  ww->InstanceTemplate()->SetInternalFieldCount(StreamReq::kStreamReqField + 1);
  Local<String> writeWrapString =
      FIXED_ONE_BYTE_STRING(env->isolate(), "WriteWrap");
  ww->SetClassName(writeWrapString);
  ww->Inherit(AsyncWrap::GetConstructorTemplate(env));
  target->Set(writeWrapString,
              ww->GetFunction(env->context()).ToLocalChecked());
  env->set_write_wrap_template(ww->InstanceTemplate());
}