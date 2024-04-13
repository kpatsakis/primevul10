Local<Object> StreamBase::GetObject() {
  return GetAsyncWrap()->object();
}