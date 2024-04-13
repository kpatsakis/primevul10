inline void StreamReq::AttachToObject(v8::Local<v8::Object> req_wrap_obj) {
  CHECK_EQ(req_wrap_obj->GetAlignedPointerFromInternalField(kStreamReqField),
           nullptr);
  req_wrap_obj->SetAlignedPointerInInternalField(kStreamReqField, this);
}