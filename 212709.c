CallResult<bool> JSObject::deleteNamed(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    SymbolID name,
    PropOpFlags opFlags) {
  assert(
      !opFlags.getMustExist() && "mustExist cannot be specified when deleting");

  // Find the property by name.
  NamedPropertyDescriptor desc;
  auto pos = findProperty(selfHandle, runtime, name, desc);

  // If the property doesn't exist in this object, return success.
  if (!pos) {
    if (LLVM_LIKELY(
            !selfHandle->flags_.lazyObject &&
            !selfHandle->flags_.proxyObject)) {
      return true;
    } else if (selfHandle->flags_.lazyObject) {
      // object is lazy, initialize and read again.
      initializeLazyObject(runtime, selfHandle);
      pos = findProperty(selfHandle, runtime, name, desc);
      if (!pos) // still not there, return true.
        return true;
    } else {
      assert(selfHandle->flags_.proxyObject && "object flags are impossible");
      return proxyOpFlags(
          runtime,
          opFlags,
          "Proxy delete returned false",
          JSProxy::deleteNamed(selfHandle, runtime, name));
    }
  }
  // If the property isn't configurable, fail.
  if (LLVM_UNLIKELY(!desc.flags.configurable)) {
    if (opFlags.getThrowOnError()) {
      return runtime->raiseTypeError(
          TwineChar16("Property '") +
          runtime->getIdentifierTable().getStringViewForDev(runtime, name) +
          "' is not configurable");
    }
    return false;
  }

  // Clear the deleted property value to prevent memory leaks.
  setNamedSlotValue(
      *selfHandle, runtime, desc, HermesValue::encodeEmptyValue());

  // Perform the actual deletion.
  auto newClazz = HiddenClass::deleteProperty(
      runtime->makeHandle(selfHandle->clazz_), runtime, *pos);
  selfHandle->clazz_.set(runtime, *newClazz, &runtime->getHeap());

  return true;
}