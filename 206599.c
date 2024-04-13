void StreamReq::ResetObject(v8::Local<v8::Object> obj) {
  DCHECK_GT(obj->InternalFieldCount(), StreamReq::kStreamReqField);

  obj->SetAlignedPointerInInternalField(StreamReq::kSlot, nullptr);
  obj->SetAlignedPointerInInternalField(StreamReq::kStreamReqField, nullptr);
}