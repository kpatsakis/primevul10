PseudoHandle<JSObject> JSObject::create(
    Runtime *runtime,
    Handle<JSObject> parentHandle) {
  JSObjectAlloc<JSObject> mem{runtime};
  return mem.initToPseudoHandle(new (mem) JSObject(
      runtime,
      &vt.base,
      *parentHandle,
      runtime->getHiddenClassForPrototypeRaw(
          *parentHandle,
          numOverlapSlots<JSObject>() + ANONYMOUS_PROPERTY_SLOTS),
      GCPointerBase::NoBarriers()));
}