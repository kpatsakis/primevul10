CallResult<PseudoHandle<>> JSObject::getNamedPropertyValue_RJS(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    Handle<JSObject> propObj,
    NamedPropertyDescriptor desc) {
  assert(
      !selfHandle->flags_.proxyObject && !propObj->flags_.proxyObject &&
      "getNamedPropertyValue_RJS cannot be used with proxy objects");

  if (LLVM_LIKELY(!desc.flags.accessor))
    return createPseudoHandle(getNamedSlotValue(propObj.get(), runtime, desc));

  auto *accessor =
      vmcast<PropertyAccessor>(getNamedSlotValue(propObj.get(), runtime, desc));
  if (!accessor->getter)
    return createPseudoHandle(HermesValue::encodeUndefinedValue());

  // Execute the accessor on this object.
  return accessor->getter.get(runtime)->executeCall0(
      runtime->makeHandle(accessor->getter), runtime, selfHandle);
}