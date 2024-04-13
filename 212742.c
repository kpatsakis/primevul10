ExecutionStatus setProtoClasses(
    Runtime *runtime,
    Handle<JSObject> obj,
    MutableHandle<BigStorage> &arr) {
  // Layout of a JSArray stored in the for-in cache:
  // [class(proto(obj)), class(proto(proto(obj))), ..., null, prop0, prop1, ...]

  if (!obj->shouldCacheForIn(runtime)) {
    arr->clear(runtime);
    return ExecutionStatus::RETURNED;
  }
  MutableHandle<JSObject> head(runtime, obj->getParent(runtime));
  MutableHandle<> clazz(runtime);
  GCScopeMarkerRAII marker{runtime};
  while (head.get()) {
    if (!head->shouldCacheForIn(runtime)) {
      arr->clear(runtime);
      return ExecutionStatus::RETURNED;
    }
    if (JSObject::Helper::flags(*head).lazyObject) {
      // Ensure all properties have been initialized before caching the hidden
      // class. Not doing this will result in changes to the hidden class
      // when getOwnPropertyKeys is called later.
      JSObject::initializeLazyObject(runtime, head);
    }
    clazz = HermesValue::encodeObjectValue(head->getClass(runtime));
    if (LLVM_UNLIKELY(
            BigStorage::push_back(arr, runtime, clazz) ==
            ExecutionStatus::EXCEPTION)) {
      return ExecutionStatus::EXCEPTION;
    }
    head = head->getParent(runtime);
    marker.flush();
  }
  clazz = HermesValue::encodeNullValue();
  return BigStorage::push_back(arr, runtime, clazz);
}