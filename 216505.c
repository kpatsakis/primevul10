bool WebContents::SendIPCMessageWithSender(bool internal,
                                           bool send_to_all,
                                           const std::string& channel,
                                           blink::CloneableMessage args,
                                           int32_t sender_id) {
  std::vector<content::RenderFrameHost*> target_hosts;
  if (!send_to_all) {
    auto* frame_host = web_contents()->GetMainFrame();
    if (frame_host) {
      target_hosts.push_back(frame_host);
    }
  } else {
    target_hosts = web_contents()->GetAllFrames();
  }

  for (auto* frame_host : target_hosts) {
    mojo::AssociatedRemote<mojom::ElectronRenderer> electron_renderer;
    frame_host->GetRemoteAssociatedInterfaces()->GetInterface(
        &electron_renderer);
    electron_renderer->Message(internal, false, channel, args.ShallowClone(),
                               sender_id);
  }
  return true;
}