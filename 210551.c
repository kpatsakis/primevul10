void ServiceWorkerScriptContext::OnMessageReceived(
    int request_id,
    const IPC::Message& message) {
  DCHECK_EQ(kInvalidRequestId, current_request_id_);
  current_request_id_ = request_id;
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(ServiceWorkerScriptContext, message)
    IPC_MESSAGE_HANDLER(ServiceWorkerMsg_InstallEvent, OnInstallEvent)
    IPC_MESSAGE_HANDLER(ServiceWorkerMsg_FetchEvent, OnFetchEvent)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  DCHECK(handled);
  current_request_id_ = kInvalidRequestId;
}
