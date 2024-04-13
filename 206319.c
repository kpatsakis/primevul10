void StreamBase::AddMethods(Environment* env, Local<FunctionTemplate> t) {
  HandleScope scope(env->isolate());

  enum PropertyAttribute attributes =
      static_cast<PropertyAttribute>(
          v8::ReadOnly | v8::DontDelete | v8::DontEnum);

  Local<Signature> signature = Signature::New(env->isolate(), t);

  // TODO(TimothyGu): None of these should have ConstructorBehavior::kAllow.
  Local<FunctionTemplate> get_fd_templ =
      env->NewFunctionTemplate(GetFD<Base>,
                               signature,
                               v8::ConstructorBehavior::kAllow,
                               v8::SideEffectType::kHasNoSideEffect);

  Local<FunctionTemplate> get_external_templ =
      env->NewFunctionTemplate(GetExternal<Base>,
                               signature,
                               v8::ConstructorBehavior::kAllow,
                               v8::SideEffectType::kHasNoSideEffect);

  Local<FunctionTemplate> get_bytes_read_templ =
      env->NewFunctionTemplate(GetBytesRead<Base>,
                               signature,
                               v8::ConstructorBehavior::kAllow,
                               v8::SideEffectType::kHasNoSideEffect);

  Local<FunctionTemplate> get_bytes_written_templ =
      env->NewFunctionTemplate(GetBytesWritten<Base>,
                               signature,
                               v8::ConstructorBehavior::kAllow,
                               v8::SideEffectType::kHasNoSideEffect);

  t->PrototypeTemplate()->SetAccessorProperty(env->fd_string(),
                                              get_fd_templ,
                                              Local<FunctionTemplate>(),
                                              attributes);

  t->PrototypeTemplate()->SetAccessorProperty(env->external_stream_string(),
                                              get_external_templ,
                                              Local<FunctionTemplate>(),
                                              attributes);

  t->PrototypeTemplate()->SetAccessorProperty(env->bytes_read_string(),
                                              get_bytes_read_templ,
                                              Local<FunctionTemplate>(),
                                              attributes);

  t->PrototypeTemplate()->SetAccessorProperty(env->bytes_written_string(),
                                              get_bytes_written_templ,
                                              Local<FunctionTemplate>(),
                                              attributes);

  env->SetProtoMethod(t, "readStart", JSMethod<Base, &StreamBase::ReadStartJS>);
  env->SetProtoMethod(t, "readStop", JSMethod<Base, &StreamBase::ReadStopJS>);
  env->SetProtoMethod(t, "shutdown", JSMethod<Base, &StreamBase::Shutdown>);
  env->SetProtoMethod(t, "writev", JSMethod<Base, &StreamBase::Writev>);
  env->SetProtoMethod(t,
                      "writeBuffer",
                      JSMethod<Base, &StreamBase::WriteBuffer>);
  env->SetProtoMethod(t,
                      "writeAsciiString",
                      JSMethod<Base, &StreamBase::WriteString<ASCII> >);
  env->SetProtoMethod(t,
                      "writeUtf8String",
                      JSMethod<Base, &StreamBase::WriteString<UTF8> >);
  env->SetProtoMethod(t,
                      "writeUcs2String",
                      JSMethod<Base, &StreamBase::WriteString<UCS2> >);
  env->SetProtoMethod(t,
                      "writeLatin1String",
                      JSMethod<Base, &StreamBase::WriteString<LATIN1> >);
}