ExecutionStatus JSObject::addOwnPropertyImpl(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    SymbolID name,
    PropertyFlags propertyFlags,
    Handle<> valueOrAccessor) {
  assert(
      !selfHandle->flags_.proxyObject &&
      "Internal properties cannot be added to Proxy objects");
  // Add a new property to the class.
  // TODO: if we check for OOM here in the future, we must undo the slot
  // allocation.
  auto addResult = HiddenClass::addProperty(
      runtime->makeHandle(selfHandle->clazz_), runtime, name, propertyFlags);
  if (LLVM_UNLIKELY(addResult == ExecutionStatus::EXCEPTION)) {
    return ExecutionStatus::EXCEPTION;
  }
  selfHandle->clazz_.set(runtime, *addResult->first, &runtime->getHeap());

  allocateNewSlotStorage(
      selfHandle, runtime, addResult->second, valueOrAccessor);

  // If this is an index-like property, we need to clear the fast path flags.
  if (LLVM_UNLIKELY(
          selfHandle->clazz_.getNonNull(runtime)->getHasIndexLikeProperties()))
    selfHandle->flags_.fastIndexProperties = false;

  return ExecutionStatus::RETURNED;
}