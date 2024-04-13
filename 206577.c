void ReportWritesToJSStreamListener::OnStreamAfterShutdown(
    ShutdownWrap* req_wrap, int status) {
  OnStreamAfterReqFinished(req_wrap, status);
}