CallResult<bool> JSObject::hasComputed(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    Handle<> nameValHandle) {
  // Try the fast-path first: no "index-like" properties and the "name" already
  // is a valid integer index.
  if (selfHandle->flags_.fastIndexProperties) {
    if (auto arrayIndex = toArrayIndexFastPath(*nameValHandle)) {
      // Do we have this value present in our array storage? If so, return true.
      if (haveOwnIndexed(selfHandle.get(), runtime, *arrayIndex)) {
        return true;
      }
    }
  }

  // If nameValHandle is an object, we should convert it to string now,
  // because toString may have side-effect, and we want to do this only
  // once.
  auto converted = toPropertyKeyIfObject(runtime, nameValHandle);
  if (LLVM_UNLIKELY(converted == ExecutionStatus::EXCEPTION)) {
    return ExecutionStatus::EXCEPTION;
  }
  auto nameValPrimitiveHandle = *converted;

  ComputedPropertyDescriptor desc;
  MutableHandle<JSObject> propObj{runtime};
  if (getComputedPrimitiveDescriptor(
          selfHandle, runtime, nameValPrimitiveHandle, propObj, desc) ==
      ExecutionStatus::EXCEPTION) {
    return ExecutionStatus::EXCEPTION;
  }
  if (!propObj) {
    return false;
  }
  if (LLVM_UNLIKELY(desc.flags.proxyObject)) {
    CallResult<Handle<>> key = toPropertyKey(runtime, nameValPrimitiveHandle);
    if (key == ExecutionStatus::EXCEPTION)
      return ExecutionStatus::EXCEPTION;
    return JSProxy::hasComputed(propObj, runtime, *key);
  }
  // For compatibility with polyfills we want to pretend that all HostObject
  // properties are "own" properties in 'in'. Since there is no way to check for
  // a HostObject property, we must always assume success. In practice the
  // property name would have been obtained from enumerating the properties in
  // JS code that looks something like this:
  //    for(key in hostObj) {
  //      if (key in hostObj)
  //        ...
  //    }
  return true;
}