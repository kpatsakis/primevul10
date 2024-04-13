CallResult<bool> JSObject::updateOwnProperty(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    SymbolID name,
    HiddenClass::PropertyPos propertyPos,
    NamedPropertyDescriptor desc,
    const DefinePropertyFlags dpFlags,
    Handle<> valueOrAccessor,
    PropOpFlags opFlags) {
  auto updateStatus = checkPropertyUpdate(
      runtime,
      desc.flags,
      dpFlags,
      getNamedSlotValue(selfHandle.get(), runtime, desc),
      valueOrAccessor,
      opFlags);
  if (updateStatus == ExecutionStatus::EXCEPTION)
    return ExecutionStatus::EXCEPTION;
  if (updateStatus->first == PropertyUpdateStatus::failed)
    return false;

  // If the property flags changed, update them.
  if (updateStatus->second != desc.flags) {
    desc.flags = updateStatus->second;
    auto newClazz = HiddenClass::updateProperty(
        runtime->makeHandle(selfHandle->clazz_),
        runtime,
        propertyPos,
        desc.flags);
    selfHandle->clazz_.set(runtime, *newClazz, &runtime->getHeap());
  }

  if (updateStatus->first == PropertyUpdateStatus::done)
    return true;
  assert(
      updateStatus->first == PropertyUpdateStatus::needSet &&
      "unexpected PropertyUpdateStatus");

  if (dpFlags.setValue) {
    if (LLVM_LIKELY(!desc.flags.internalSetter))
      setNamedSlotValue(selfHandle.get(), runtime, desc, valueOrAccessor.get());
    else
      return internalSetter(
          selfHandle, runtime, name, desc, valueOrAccessor, opFlags);
  } else if (dpFlags.isAccessor()) {
    setNamedSlotValue(selfHandle.get(), runtime, desc, valueOrAccessor.get());
  } else {
    // If checkPropertyUpdate() returned needSet, but there is no value or
    // accessor, clear the value.
    setNamedSlotValue(
        selfHandle.get(), runtime, desc, HermesValue::encodeUndefinedValue());
  }

  return true;
}