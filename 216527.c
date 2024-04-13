void WebContents::PostMessage(const std::string& channel,
                              v8::Local<v8::Value> message_value,
                              base::Optional<v8::Local<v8::Value>> transfer) {
  blink::TransferableMessage transferable_message;
  if (!electron::SerializeV8Value(isolate(), message_value,
                                  &transferable_message)) {
    // SerializeV8Value sets an exception.
    return;
  }

  std::vector<gin::Handle<MessagePort>> wrapped_ports;
  if (transfer) {
    if (!gin::ConvertFromV8(isolate(), *transfer, &wrapped_ports)) {
      isolate()->ThrowException(v8::Exception::Error(
          gin::StringToV8(isolate(), "Invalid value for transfer")));
      return;
    }
  }

  bool threw_exception = false;
  transferable_message.ports =
      MessagePort::DisentanglePorts(isolate(), wrapped_ports, &threw_exception);
  if (threw_exception)
    return;

  content::RenderFrameHost* frame_host = web_contents()->GetMainFrame();
  mojo::AssociatedRemote<mojom::ElectronRenderer> electron_renderer;
  frame_host->GetRemoteAssociatedInterfaces()->GetInterface(&electron_renderer);
  electron_renderer->ReceivePostMessage(channel,
                                        std::move(transferable_message));
}