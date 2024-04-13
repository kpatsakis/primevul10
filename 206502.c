void LibuvStreamWrap::OnUvAlloc(size_t suggested_size, uv_buf_t* buf) {
  HandleScope scope(env()->isolate());
  Context::Scope context_scope(env()->context());

  *buf = EmitAlloc(suggested_size);
}