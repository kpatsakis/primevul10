StreamBase* StreamBase::FromObject(v8::Local<v8::Object> obj) {
  if (obj->GetAlignedPointerFromInternalField(StreamBase::kSlot) == nullptr)
    return nullptr;

  return static_cast<StreamBase*>(
      obj->GetAlignedPointerFromInternalField(
          StreamBase::kStreamBaseField));
}