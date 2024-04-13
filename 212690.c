ExecutionStatus JSObject::getComputedDescriptor(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    Handle<> nameValHandle,
    MutableHandle<JSObject> &propObj,
    ComputedPropertyDescriptor &desc) {
  auto converted = toPropertyKeyIfObject(runtime, nameValHandle);
  if (LLVM_UNLIKELY(converted == ExecutionStatus::EXCEPTION)) {
    return ExecutionStatus::EXCEPTION;
  }
  return getComputedPrimitiveDescriptor(
      selfHandle, runtime, *converted, propObj, desc);
}