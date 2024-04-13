CallResult<PseudoHandle<JSObject>> JSObject::getPrototypeOf(
    PseudoHandle<JSObject> selfHandle,
    Runtime *runtime) {
  if (LLVM_LIKELY(!selfHandle->isProxyObject())) {
    return createPseudoHandle(selfHandle->getParent(runtime));
  }

  return JSProxy::getPrototypeOf(
      runtime->makeHandle(std::move(selfHandle)), runtime);
}