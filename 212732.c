CallResult<bool> JSObject::setParent(
    JSObject *self,
    Runtime *runtime,
    JSObject *parent,
    PropOpFlags opFlags) {
  if (LLVM_UNLIKELY(self->isProxyObject())) {
    return proxyOpFlags(
        runtime,
        opFlags,
        "Object is not extensible.",
        JSProxy::setPrototypeOf(
            runtime->makeHandle(self), runtime, runtime->makeHandle(parent)));
  }
  // ES9 9.1.2
  // 4.
  if (self->parent_.get(runtime) == parent)
    return true;
  // 5.
  if (!self->isExtensible()) {
    if (opFlags.getThrowOnError()) {
      return runtime->raiseTypeError("Object is not extensible.");
    } else {
      return false;
    }
  }
  // 6-8. Check for a prototype cycle.
  for (JSObject *cur = parent; cur; cur = cur->parent_.get(runtime)) {
    if (cur == self) {
      if (opFlags.getThrowOnError()) {
        return runtime->raiseTypeError("Prototype cycle detected");
      } else {
        return false;
      }
    } else if (LLVM_UNLIKELY(cur->isProxyObject())) {
      // TODO this branch should also be used for module namespace and
      // immutable prototype exotic objects.
      break;
    }
  }
  // 9.
  self->parent_.set(runtime, parent, &runtime->getHeap());
  // 10.
  return true;
}