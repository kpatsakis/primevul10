CallResult<PseudoHandle<>> JSObject::getComputedPropertyValue_RJS(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    Handle<JSObject> propObj,
    ComputedPropertyDescriptor desc) {
  assert(
      !selfHandle->flags_.proxyObject && !propObj->flags_.proxyObject &&
      "getComputedPropertyValue_RJS cannot be used with proxy objects");

  if (LLVM_LIKELY(!desc.flags.accessor))
    return createPseudoHandle(
        getComputedSlotValue(propObj.get(), runtime, desc));

  auto *accessor = vmcast<PropertyAccessor>(
      getComputedSlotValue(propObj.get(), runtime, desc));
  if (!accessor->getter)
    return createPseudoHandle(HermesValue::encodeUndefinedValue());

  // Execute the accessor on this object.
  return accessor->getter.get(runtime)->executeCall0(
      runtime->makeHandle(accessor->getter), runtime, selfHandle);
}