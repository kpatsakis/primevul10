void WebContents::MessageTo(bool internal,
                            bool send_to_all,
                            int32_t web_contents_id,
                            const std::string& channel,
                            blink::CloneableMessage arguments) {
  TRACE_EVENT1("electron", "WebContents::MessageTo", "channel", channel);
  auto* web_contents = gin_helper::TrackableObject<WebContents>::FromWeakMapID(
      isolate(), web_contents_id);

  if (web_contents) {
    web_contents->SendIPCMessageWithSender(internal, send_to_all, channel,
                                           std::move(arguments), ID());
  }
}