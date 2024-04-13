Local<FunctionTemplate> LibuvStreamWrap::GetConstructorTemplate(
    Environment* env) {
  Local<FunctionTemplate> tmpl = env->libuv_stream_wrap_ctor_template();
  if (tmpl.IsEmpty()) {
    tmpl = env->NewFunctionTemplate(nullptr);
    tmpl->SetClassName(
        FIXED_ONE_BYTE_STRING(env->isolate(), "LibuvStreamWrap"));
    tmpl->Inherit(HandleWrap::GetConstructorTemplate(env));
    Local<FunctionTemplate> get_write_queue_size =
        FunctionTemplate::New(env->isolate(),
                              GetWriteQueueSize,
                              env->as_external(),
                              Signature::New(env->isolate(), tmpl));
    tmpl->PrototypeTemplate()->SetAccessorProperty(
        env->write_queue_size_string(),
        get_write_queue_size,
        Local<FunctionTemplate>(),
        static_cast<PropertyAttribute>(ReadOnly | DontDelete));
    env->SetProtoMethod(tmpl, "setBlocking", SetBlocking);
    StreamBase::AddMethods<LibuvStreamWrap>(env, tmpl);
    env->set_libuv_stream_wrap_ctor_template(tmpl);
  }
  return tmpl;
}