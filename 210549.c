void ServiceWorkerScriptContext::DidHandleInstallEvent(int request_id) {
  Send(request_id, ServiceWorkerHostMsg_InstallEventFinished());
}
