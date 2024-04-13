bool SearchEngineTabHelper::OnMessageReceived(
    const IPC::Message& message,
    content::RenderFrameHost* render_frame_host) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(SearchEngineTabHelper, message)
    IPC_MESSAGE_HANDLER(ChromeViewHostMsg_PageHasOSDD, OnPageHasOSDD)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()

  return handled;
}
