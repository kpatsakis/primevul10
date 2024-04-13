ExecutionStatus JSObject::freeze(
    Handle<JSObject> selfHandle,
    Runtime *runtime) {
  CallResult<bool> statusRes = JSObject::preventExtensions(
      selfHandle, runtime, PropOpFlags().plusThrowOnError());
  if (LLVM_UNLIKELY(statusRes == ExecutionStatus::EXCEPTION)) {
    return ExecutionStatus::EXCEPTION;
  }
  assert(
      *statusRes &&
      "freeze preventExtensions with ThrowOnError returned false");

  // Already frozen?
  if (selfHandle->flags_.frozen)
    return ExecutionStatus::RETURNED;

  auto newClazz = HiddenClass::makeAllReadOnly(
      runtime->makeHandle(selfHandle->clazz_), runtime);
  selfHandle->clazz_.set(runtime, *newClazz, &runtime->getHeap());

  selfHandle->flags_.frozen = true;
  selfHandle->flags_.sealed = true;

  return ExecutionStatus::RETURNED;
}