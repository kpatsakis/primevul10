CallResult<bool> JSObject::isExtensible(
    PseudoHandle<JSObject> self,
    Runtime *runtime) {
  if (LLVM_UNLIKELY(self->isProxyObject())) {
    return JSProxy::isExtensible(runtime->makeHandle(std::move(self)), runtime);
  }
  return self->isExtensible();
}