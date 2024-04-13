void JSObject::preventExtensions(JSObject *self) {
  assert(
      !self->flags_.proxyObject &&
      "[[Extensible]] slot cannot be set directly on Proxy objects");
  self->flags_.noExtend = true;
}