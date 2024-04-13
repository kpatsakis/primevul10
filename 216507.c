void WebContents::DisableDeviceEmulation() {
  if (type_ == Type::REMOTE)
    return;

  auto* frame_host = web_contents()->GetMainFrame();
  if (frame_host) {
    auto* widget_host =
        frame_host ? frame_host->GetView()->GetRenderWidgetHost() : nullptr;
    if (!widget_host)
      return;
    widget_host->Send(
        new WidgetMsg_DisableDeviceEmulation(widget_host->GetRoutingID()));
  }
}