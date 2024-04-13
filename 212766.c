ExecutionStatus JSObject::getComputedPrimitiveDescriptor(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    Handle<> nameValHandle,
    MutableHandle<JSObject> &propObj,
    ComputedPropertyDescriptor &desc) {
  assert(
      !nameValHandle->isObject() &&
      "nameValHandle passed to "
      "getComputedPrimitiveDescriptor cannot "
      "be an object");

  propObj = selfHandle.get();

  SymbolID id{};

  GCScopeMarkerRAII marker{runtime};
  do {
    // A proxy is ignored here so we can check the bit later and
    // return it back to the caller for additional processing.

    Handle<JSObject> loopHandle = propObj;

    CallResult<bool> res = getOwnComputedPrimitiveDescriptorImpl(
        loopHandle, runtime, nameValHandle, IgnoreProxy::Yes, id, desc);
    if (LLVM_UNLIKELY(res == ExecutionStatus::EXCEPTION)) {
      return ExecutionStatus::EXCEPTION;
    }
    if (*res) {
      return ExecutionStatus::RETURNED;
    }

    if (LLVM_UNLIKELY(propObj->flags_.hostObject)) {
      desc.flags.hostObject = true;
      desc.flags.writable = true;
      return ExecutionStatus::RETURNED;
    }
    if (LLVM_UNLIKELY(propObj->flags_.proxyObject)) {
      desc.flags.proxyObject = true;
      return ExecutionStatus::RETURNED;
    }
    // This isn't a proxy, so use the faster getParent() instead of
    // getPrototypeOf.
    propObj = propObj->getParent(runtime);
    // Flush at the end of the loop to allow first iteration to be as fast as
    // possible.
    marker.flush();
  } while (propObj);
  return ExecutionStatus::RETURNED;
}