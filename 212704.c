ExecutionStatus JSObject::defineNewOwnProperty(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    SymbolID name,
    PropertyFlags propertyFlags,
    Handle<> valueOrAccessor) {
  assert(
      !selfHandle->flags_.proxyObject &&
      "definedNewOwnProperty cannot be used with proxy objects");
  assert(
      !(propertyFlags.accessor && !valueOrAccessor.get().isPointer()) &&
      "accessor must be non-empty");
  assert(
      !(propertyFlags.accessor && propertyFlags.writable) &&
      "writable must not be set with accessors");
  assert(
      !HiddenClass::debugIsPropertyDefined(
          selfHandle->clazz_.get(runtime), runtime, name) &&
      "new property is already defined");

  return addOwnPropertyImpl(
      selfHandle, runtime, name, propertyFlags, valueOrAccessor);
}