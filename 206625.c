WriteWrap* WriteWrap::FromObject(
    const BaseObjectPtrImpl<T, kIsWeak>& base_obj) {
  if (!base_obj) return nullptr;
  return FromObject(base_obj->object());
}