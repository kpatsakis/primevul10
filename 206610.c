int StreamBase::WriteBuffer(const FunctionCallbackInfo<Value>& args) {
  CHECK(args[0]->IsObject());

  Environment* env = Environment::GetCurrent(args);

  if (!args[1]->IsUint8Array()) {
    node::THROW_ERR_INVALID_ARG_TYPE(env, "Second argument must be a buffer");
    return 0;
  }

  Local<Object> req_wrap_obj = args[0].As<Object>();
  uv_buf_t buf;
  buf.base = Buffer::Data(args[1]);
  buf.len = Buffer::Length(args[1]);

  uv_stream_t* send_handle = nullptr;

  if (args[2]->IsObject() && IsIPCPipe()) {
    Local<Object> send_handle_obj = args[2].As<Object>();

    HandleWrap* wrap;
    ASSIGN_OR_RETURN_UNWRAP(&wrap, send_handle_obj, UV_EINVAL);
    send_handle = reinterpret_cast<uv_stream_t*>(wrap->GetHandle());
    // Reference LibuvStreamWrap instance to prevent it from being garbage
    // collected before `AfterWrite` is called.
    req_wrap_obj->Set(env->context(),
                      env->handle_string(),
                      send_handle_obj).Check();
  }

  StreamWriteResult res = Write(&buf, 1, send_handle, req_wrap_obj);
  SetWriteResult(res);

  return res.err;
}