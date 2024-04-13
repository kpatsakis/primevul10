bool WebContents::SendIPCMessage(bool internal,
                                 bool send_to_all,
                                 const std::string& channel,
                                 v8::Local<v8::Value> args) {
  blink::CloneableMessage message;
  if (!gin::ConvertFromV8(isolate(), args, &message)) {
    isolate()->ThrowException(v8::Exception::Error(
        gin::StringToV8(isolate(), "Failed to serialize arguments")));
    return false;
  }
  return SendIPCMessageWithSender(internal, send_to_all, channel,
                                  std::move(message));
}