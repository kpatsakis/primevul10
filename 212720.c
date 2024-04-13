CallResult<bool> JSObject::defineOwnComputed(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    Handle<> nameValHandle,
    DefinePropertyFlags dpFlags,
    Handle<> valueOrAccessor,
    PropOpFlags opFlags) {
  auto converted = toPropertyKeyIfObject(runtime, nameValHandle);
  if (LLVM_UNLIKELY(converted == ExecutionStatus::EXCEPTION))
    return ExecutionStatus::EXCEPTION;
  return defineOwnComputedPrimitive(
      selfHandle, runtime, *converted, dpFlags, valueOrAccessor, opFlags);
}