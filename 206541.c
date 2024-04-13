void Http2Session::Ping(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Http2Session* session;
  ASSIGN_OR_RETURN_UNWRAP(&session, args.Holder());

  // A PING frame may have exactly 8 bytes of payload data. If not provided,
  // then the current hrtime will be used as the payload.
  uint8_t* payload = nullptr;
  if (Buffer::HasInstance(args[0])) {
    payload = reinterpret_cast<uint8_t*>(Buffer::Data(args[0]));
    CHECK_EQ(Buffer::Length(args[0]), 8);
  }

  Local<Object> obj;
  if (!env->http2ping_constructor_template()
           ->NewInstance(env->context())
           .ToLocal(&obj)) {
    return;
  }
  if (obj->Set(env->context(), env->ondone_string(), args[1]).IsNothing())
    return;
  Http2Session::Http2Ping* ping = new Http2Ping(session, obj);

  // To prevent abuse, we strictly limit the number of unacknowledged PING
  // frames that may be sent at any given time. This is configurable in the
  // Options when creating a Http2Session.
  if (!session->AddPing(ping)) {
    ping->Done(false);
    return args.GetReturnValue().Set(false);
  }

  // The Ping itself is an Async resource. When the acknowledgement is received,
  // the callback will be invoked and a notification sent out to JS land. The
  // notification will include the duration of the ping, allowing the round
  // trip to be measured.
  ping->Send(payload);
  args.GetReturnValue().Set(true);
}