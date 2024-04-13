void Http2Session::Consume(Local<External> external) {
  StreamBase* stream = static_cast<StreamBase*>(external->Value());
  stream->PushStreamListener(this);
  Debug(this, "i/o stream consumed");
}