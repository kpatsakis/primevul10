CallResult<PseudoHandle<>> JSObject::getComputedPropertyValue_RJS(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    Handle<JSObject> propObj,
    ComputedPropertyDescriptor desc,
    Handle<> nameValHandle) {
  if (!propObj) {
    return createPseudoHandle(HermesValue::encodeEmptyValue());
  }

  if (LLVM_LIKELY(!desc.flags.proxyObject)) {
    return JSObject::getComputedPropertyValue_RJS(
        selfHandle, runtime, propObj, desc);
  }

  CallResult<Handle<>> keyRes = toPropertyKey(runtime, nameValHandle);
  if (LLVM_UNLIKELY(keyRes == ExecutionStatus::EXCEPTION)) {
    return ExecutionStatus::EXCEPTION;
  }
  CallResult<bool> hasRes = JSProxy::hasComputed(propObj, runtime, *keyRes);
  if (LLVM_UNLIKELY(hasRes == ExecutionStatus::EXCEPTION)) {
    return ExecutionStatus::EXCEPTION;
  }
  if (!*hasRes) {
    return createPseudoHandle(HermesValue::encodeEmptyValue());
  }
  return JSProxy::getComputed(propObj, runtime, *keyRes, selfHandle);
}