inline void StreamReq::ResetObject(v8::Local<v8::Object> obj) {
#ifdef DEBUG
  CHECK_GT(obj->InternalFieldCount(), StreamReq::kStreamReqField);
#endif
  obj->SetAlignedPointerInInternalField(0, nullptr);  // BaseObject field.
  obj->SetAlignedPointerInInternalField(StreamReq::kStreamReqField, nullptr);
}