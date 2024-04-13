PseudoHandle<JSObject> JSObject::create(
    Runtime *runtime,
    unsigned propertyCount) {
  JSObjectAlloc<JSObject> mem{runtime};
  JSObject *objProto = runtime->objectPrototypeRawPtr;
  auto self = mem.initToPseudoHandle(new (mem) JSObject(
      runtime,
      &vt.base,
      objProto,
      runtime->getHiddenClassForPrototypeRaw(
          objProto, numOverlapSlots<JSObject>() + ANONYMOUS_PROPERTY_SLOTS),
      GCPointerBase::NoBarriers()));

  return runtime->ignoreAllocationFailure(
      JSObject::allocatePropStorage(std::move(self), runtime, propertyCount));
}