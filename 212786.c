CallResult<bool> JSObject::getOwnComputedDescriptor(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    Handle<> nameValHandle,
    ComputedPropertyDescriptor &desc) {
  auto converted = toPropertyKeyIfObject(runtime, nameValHandle);
  if (LLVM_UNLIKELY(converted == ExecutionStatus::EXCEPTION)) {
    return ExecutionStatus::EXCEPTION;
  }
  return JSObject::getOwnComputedPrimitiveDescriptor(
      selfHandle, runtime, *converted, IgnoreProxy::No, desc);
}