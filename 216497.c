void WebContents::ReceivePostMessage(const std::string& channel,
                                     blink::TransferableMessage message) {
  v8::HandleScope handle_scope(isolate());
  auto wrapped_ports =
      MessagePort::EntanglePorts(isolate(), std::move(message.ports));
  v8::Local<v8::Value> message_value =
      electron::DeserializeV8Value(isolate(), message);
  EmitWithSender("-ipc-ports", receivers_.current_context(), InvokeCallback(),
                 false, channel, message_value, std::move(wrapped_ports));
}