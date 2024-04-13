void JSStream::Finish(const FunctionCallbackInfo<Value>& args) {
  CHECK(args[0]->IsObject());
  Wrap* w = static_cast<Wrap*>(StreamReq::FromObject(args[0].As<Object>()));

  CHECK(args[1]->IsInt32());
  w->Done(args[1].As<Int32>()->Value());
}