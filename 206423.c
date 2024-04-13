void LibuvStreamWrap::AfterUvShutdown(uv_shutdown_t* req, int status) {
  LibuvShutdownWrap* req_wrap = static_cast<LibuvShutdownWrap*>(
      LibuvShutdownWrap::from_req(req));
  CHECK_NOT_NULL(req_wrap);
  HandleScope scope(req_wrap->env()->isolate());
  Context::Scope context_scope(req_wrap->env()->context());
  req_wrap->Done(status);
}