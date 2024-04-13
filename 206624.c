void ReportWritesToJSStreamListener::OnStreamAfterWrite(
    WriteWrap* req_wrap, int status) {
  OnStreamAfterReqFinished(req_wrap, status);
}