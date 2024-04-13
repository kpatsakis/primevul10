void StreamBase::AddMethods(Environment* env, Local<FunctionTemplate> t) {
  HandleScope scope(env->isolate());

  enum PropertyAttribute attributes =
      static_cast<PropertyAttribute>(ReadOnly | DontDelete | DontEnum);
  Local<Signature> sig = Signature::New(env->isolate(), t);

  AddMethod(env, sig, attributes, t, GetFD, env->fd_string());
  AddMethod(
      env, sig, attributes, t, GetExternal, env->external_stream_string());
  AddMethod(env, sig, attributes, t, GetBytesRead, env->bytes_read_string());
  AddMethod(
      env, sig, attributes, t, GetBytesWritten, env->bytes_written_string());
  env->SetProtoMethod(t, "readStart", JSMethod<&StreamBase::ReadStartJS>);
  env->SetProtoMethod(t, "readStop", JSMethod<&StreamBase::ReadStopJS>);
  env->SetProtoMethod(t, "shutdown", JSMethod<&StreamBase::Shutdown>);
  env->SetProtoMethod(t,
                      "useUserBuffer",
                      JSMethod<&StreamBase::UseUserBuffer>);
  env->SetProtoMethod(t, "writev", JSMethod<&StreamBase::Writev>);
  env->SetProtoMethod(t, "writeBuffer", JSMethod<&StreamBase::WriteBuffer>);
  env->SetProtoMethod(
      t, "writeAsciiString", JSMethod<&StreamBase::WriteString<ASCII>>);
  env->SetProtoMethod(
      t, "writeUtf8String", JSMethod<&StreamBase::WriteString<UTF8>>);
  env->SetProtoMethod(
      t, "writeUcs2String", JSMethod<&StreamBase::WriteString<UCS2>>);
  env->SetProtoMethod(
      t, "writeLatin1String", JSMethod<&StreamBase::WriteString<LATIN1>>);
  t->PrototypeTemplate()->Set(FIXED_ONE_BYTE_STRING(env->isolate(),
                                                    "isStreamBase"),
                              True(env->isolate()));
  t->PrototypeTemplate()->SetAccessor(
      FIXED_ONE_BYTE_STRING(env->isolate(), "onread"),
      BaseObject::InternalFieldGet<
          StreamBase::kOnReadFunctionField>,
      BaseObject::InternalFieldSet<
          StreamBase::kOnReadFunctionField,
          &Value::IsFunction>);
}