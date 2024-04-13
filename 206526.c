void LibuvStreamWrap::AfterUvWrite(uv_write_t* req, int status) {
  LibuvWriteWrap* req_wrap = static_cast<LibuvWriteWrap*>(
      LibuvWriteWrap::from_req(req));
  CHECK_NOT_NULL(req_wrap);
  HandleScope scope(req_wrap->env()->isolate());
  Context::Scope context_scope(req_wrap->env()->context());
  req_wrap->Done(status);
}