CallResult<bool> JSObject::preventExtensions(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    PropOpFlags opFlags) {
  if (LLVM_UNLIKELY(selfHandle->isProxyObject())) {
    return JSProxy::preventExtensions(selfHandle, runtime, opFlags);
  }
  JSObject::preventExtensions(*selfHandle);
  return true;
}