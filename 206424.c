static Local<Object> AcceptHandle(Environment* env, LibuvStreamWrap* parent) {
  static_assert(std::is_base_of<LibuvStreamWrap, WrapType>::value ||
                std::is_base_of<UDPWrap, WrapType>::value,
                "Can only accept stream handles");

  EscapableHandleScope scope(env->isolate());
  Local<Object> wrap_obj;

  wrap_obj = WrapType::Instantiate(env, parent, WrapType::SOCKET);
  if (wrap_obj.IsEmpty())
    return Local<Object>();

  HandleWrap* wrap = Unwrap<HandleWrap>(wrap_obj);
  CHECK_NOT_NULL(wrap);
  uv_stream_t* stream = reinterpret_cast<uv_stream_t*>(wrap->GetHandle());
  CHECK_NOT_NULL(stream);

  if (uv_accept(parent->stream(), stream))
    ABORT();

  return scope.Escape(wrap_obj);
}