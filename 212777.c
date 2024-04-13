ExecutionStatus JSObject::seal(Handle<JSObject> selfHandle, Runtime *runtime) {
  CallResult<bool> statusRes = JSObject::preventExtensions(
      selfHandle, runtime, PropOpFlags().plusThrowOnError());
  if (LLVM_UNLIKELY(statusRes == ExecutionStatus::EXCEPTION)) {
    return ExecutionStatus::EXCEPTION;
  }
  assert(
      *statusRes && "seal preventExtensions with ThrowOnError returned false");

  // Already sealed?
  if (selfHandle->flags_.sealed)
    return ExecutionStatus::RETURNED;

  auto newClazz = HiddenClass::makeAllNonConfigurable(
      runtime->makeHandle(selfHandle->clazz_), runtime);
  selfHandle->clazz_.set(runtime, *newClazz, &runtime->getHeap());

  selfHandle->flags_.sealed = true;

  return ExecutionStatus::RETURNED;
}