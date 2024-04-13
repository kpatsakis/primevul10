CallResult<PseudoHandle<>> JSObject::getNamedWithReceiver_RJS(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    SymbolID name,
    Handle<> receiver,
    PropOpFlags opFlags,
    PropertyCacheEntry *cacheEntry) {
  NamedPropertyDescriptor desc;
  // Locate the descriptor. propObj contains the object which may be anywhere
  // along the prototype chain.
  JSObject *propObj = getNamedDescriptor(selfHandle, runtime, name, desc);
  if (!propObj) {
    if (LLVM_UNLIKELY(opFlags.getMustExist())) {
      return runtime->raiseReferenceError(
          TwineChar16("Property '") +
          runtime->getIdentifierTable().getStringViewForDev(runtime, name) +
          "' doesn't exist");
    }
    return createPseudoHandle(HermesValue::encodeUndefinedValue());
  }

  if (LLVM_LIKELY(
          !desc.flags.accessor && !desc.flags.hostObject &&
          !desc.flags.proxyObject)) {
    // Populate the cache if requested.
    if (cacheEntry && !propObj->getClass(runtime)->isDictionaryNoCache()) {
      cacheEntry->clazz = propObj->getClassGCPtr().getStorageType();
      cacheEntry->slot = desc.slot;
    }
    return createPseudoHandle(getNamedSlotValue(propObj, runtime, desc));
  }

  if (desc.flags.accessor) {
    auto *accessor =
        vmcast<PropertyAccessor>(getNamedSlotValue(propObj, runtime, desc));
    if (!accessor->getter)
      return createPseudoHandle(HermesValue::encodeUndefinedValue());

    // Execute the accessor on this object.
    return Callable::executeCall0(
        runtime->makeHandle(accessor->getter), runtime, receiver);
  } else if (desc.flags.hostObject) {
    auto res = vmcast<HostObject>(propObj)->get(name);
    if (LLVM_UNLIKELY(res == ExecutionStatus::EXCEPTION)) {
      return ExecutionStatus::EXCEPTION;
    }
    return createPseudoHandle(*res);
  } else {
    assert(desc.flags.proxyObject && "descriptor flags are impossible");
    return JSProxy::getNamed(
        runtime->makeHandle(propObj), runtime, name, receiver);
  }
}