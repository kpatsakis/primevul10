int LibuvStreamWrap::DoShutdown(ShutdownWrap* req_wrap_) {
  LibuvShutdownWrap* req_wrap = static_cast<LibuvShutdownWrap*>(req_wrap_);
  return req_wrap->Dispatch(uv_shutdown, stream(), AfterUvShutdown);
}