void StreamBase::AttachToObject(v8::Local<v8::Object> obj) {
  obj->SetAlignedPointerInInternalField(
      StreamBase::kStreamBaseField, this);
}