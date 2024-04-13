CallResult<bool> JSObject::getOwnComputedDescriptor(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    Handle<> nameValHandle,
    ComputedPropertyDescriptor &desc,
    MutableHandle<> &valueOrAccessor) {
  auto converted = toPropertyKeyIfObject(runtime, nameValHandle);
  if (LLVM_UNLIKELY(converted == ExecutionStatus::EXCEPTION)) {
    return ExecutionStatus::EXCEPTION;
  }
  // The proxy is ignored here so we can avoid calling
  // JSProxy::getOwnProperty twice on proxies, since
  // getOwnComputedPrimitiveDescriptor doesn't pass back the
  // valueOrAccessor.
  CallResult<bool> res = JSObject::getOwnComputedPrimitiveDescriptor(
      selfHandle, runtime, *converted, IgnoreProxy::Yes, desc);
  if (LLVM_UNLIKELY(res == ExecutionStatus::EXCEPTION)) {
    return ExecutionStatus::EXCEPTION;
  }
  if (*res) {
    valueOrAccessor = getComputedSlotValue(selfHandle.get(), runtime, desc);
    return true;
  }
  if (LLVM_UNLIKELY(selfHandle->isProxyObject())) {
    return JSProxy::getOwnProperty(
        selfHandle, runtime, nameValHandle, desc, &valueOrAccessor);
  }
  return false;
}