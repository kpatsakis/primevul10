PseudoHandle<JSObject> JSObject::create(Runtime *runtime) {
  JSObjectAlloc<JSObject> mem{runtime};
  JSObject *objProto = runtime->objectPrototypeRawPtr;
  return mem.initToPseudoHandle(new (mem) JSObject(
      runtime,
      &vt.base,
      objProto,
      runtime->getHiddenClassForPrototypeRaw(
          objProto, numOverlapSlots<JSObject>() + ANONYMOUS_PROPERTY_SLOTS),
      GCPointerBase::NoBarriers()));
}