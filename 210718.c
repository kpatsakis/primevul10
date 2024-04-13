bool EmbeddedWorkerContextClient::OnMessageReceived(
    const IPC::Message& msg) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(EmbeddedWorkerContextClient, msg)
    IPC_MESSAGE_HANDLER(EmbeddedWorkerContextMsg_SendMessageToWorker,
                        OnSendMessageToWorker)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  return handled;
}
