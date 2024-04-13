CallResult<bool> JSObject::deleteComputed(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    Handle<> nameValHandle,
    PropOpFlags opFlags) {
  assert(
      !opFlags.getMustExist() && "mustExist cannot be specified when deleting");

  // If nameValHandle is an object, we should convert it to string now,
  // because toString may have side-effect, and we want to do this only
  // once.
  auto converted = toPropertyKeyIfObject(runtime, nameValHandle);
  if (LLVM_UNLIKELY(converted == ExecutionStatus::EXCEPTION)) {
    return ExecutionStatus::EXCEPTION;
  }

  auto nameValPrimitiveHandle = *converted;

  // If the name is a valid integer array index, store it here.
  OptValue<uint32_t> arrayIndex;

  // If we have indexed storage, we must attempt to convert the name to array
  // index, even if the conversion is expensive.
  if (selfHandle->flags_.indexedStorage) {
    MutableHandle<StringPrimitive> strPrim{runtime};
    TO_ARRAY_INDEX(runtime, nameValPrimitiveHandle, strPrim, arrayIndex);
  }

  // Try the fast-path first: the "name" is a valid array index and we don't
  // have "index-like" named properties.
  if (arrayIndex && selfHandle->flags_.fastIndexProperties) {
    // Delete the indexed property.
    if (deleteOwnIndexed(selfHandle, runtime, *arrayIndex))
      return true;

    // Cannot delete property (for example this may be a typed array).
    if (opFlags.getThrowOnError()) {
      // TODO: better error message.
      return runtime->raiseTypeError("Cannot delete property");
    }
    return false;
  }

  // slow path, check if object is lazy before continuing.
  if (LLVM_UNLIKELY(selfHandle->flags_.lazyObject)) {
    // initialize and try again.
    initializeLazyObject(runtime, selfHandle);
    return deleteComputed(selfHandle, runtime, nameValHandle, opFlags);
  }

  // Convert the string to an SymbolID;
  SymbolID id;
  LAZY_TO_IDENTIFIER(runtime, nameValPrimitiveHandle, id);

  // Find the property by name.
  NamedPropertyDescriptor desc;
  auto pos = findProperty(selfHandle, runtime, id, desc);

  // If the property exists, make sure it is configurable.
  if (pos) {
    // If the property isn't configurable, fail.
    if (LLVM_UNLIKELY(!desc.flags.configurable)) {
      if (opFlags.getThrowOnError()) {
        // TODO: a better message.
        return runtime->raiseTypeError("Property is not configurable");
      }
      return false;
    }
  }

  // At this point we know that the named property either doesn't exist, or
  // is configurable and so can be deleted, or the object is a Proxy.

  // If it is an "index-like" property, we must also delete the "shadow" indexed
  // property in order to keep Array.length correct.
  if (arrayIndex) {
    if (!deleteOwnIndexed(selfHandle, runtime, *arrayIndex)) {
      // Cannot delete property (for example this may be a typed array).
      if (opFlags.getThrowOnError()) {
        // TODO: better error message.
        return runtime->raiseTypeError("Cannot delete property");
      }
      return false;
    }
  }

  if (pos) {
    // delete the named property (if it exists).
    // Clear the deleted property value to prevent memory leaks.
    setNamedSlotValue(
        *selfHandle, runtime, desc, HermesValue::encodeEmptyValue());

    // Remove the property descriptor.
    auto newClazz = HiddenClass::deleteProperty(
        runtime->makeHandle(selfHandle->clazz_), runtime, *pos);
    selfHandle->clazz_.set(runtime, *newClazz, &runtime->getHeap());
  } else if (LLVM_UNLIKELY(selfHandle->flags_.proxyObject)) {
    CallResult<Handle<>> key = toPropertyKey(runtime, nameValPrimitiveHandle);
    if (key == ExecutionStatus::EXCEPTION)
      return ExecutionStatus::EXCEPTION;
    return proxyOpFlags(
        runtime,
        opFlags,
        "Proxy delete returned false",
        JSProxy::deleteComputed(selfHandle, runtime, *key));
  }

  return true;
}