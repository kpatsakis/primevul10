void Http2Stream::FlushData(const FunctionCallbackInfo<Value>& args) {
  Http2Stream* stream;
  ASSIGN_OR_RETURN_UNWRAP(&stream, args.Holder());
  stream->ReadStart();
  Debug(stream, "data flushed to js");
}