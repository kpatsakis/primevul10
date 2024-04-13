CallResult<bool> JSObject::hasNamed(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    SymbolID name) {
  NamedPropertyDescriptor desc;
  JSObject *propObj = getNamedDescriptor(selfHandle, runtime, name, desc);
  if (propObj == nullptr) {
    return false;
  }
  if (LLVM_UNLIKELY(desc.flags.proxyObject)) {
    return JSProxy::hasNamed(runtime->makeHandle(propObj), runtime, name);
  }
  return true;
}