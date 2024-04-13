CallResult<bool> getOwnComputedPrimitiveDescriptorImpl(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    Handle<> nameValHandle,
    JSObject::IgnoreProxy ignoreProxy,
    SymbolID &id,
    ComputedPropertyDescriptor &desc) {
  assert(
      !nameValHandle->isObject() &&
      "nameValHandle passed to "
      "getOwnComputedPrimitiveDescriptor "
      "cannot be an object");

  // Try the fast paths first if we have "fast" index properties and the
  // property name is an obvious index.
  if (auto arrayIndex = toArrayIndexFastPath(*nameValHandle)) {
    if (JSObject::Helper::flags(*selfHandle).fastIndexProperties) {
      auto res = JSObject::Helper::getOwnIndexedPropertyFlags(
          selfHandle.get(), runtime, *arrayIndex);
      if (res) {
        // This a valid array index, residing in our indexed storage.
        desc.flags = *res;
        desc.flags.indexed = 1;
        desc.slot = *arrayIndex;
        return true;
      }

      // This a valid array index, but we don't have it in our indexed storage,
      // and we don't have index-like named properties.
      return false;
    }

    if (!selfHandle->getClass(runtime)->getHasIndexLikeProperties() &&
        !selfHandle->isHostObject() && !selfHandle->isLazy() &&
        !selfHandle->isProxyObject()) {
      // Early return to handle the case where an object definitely has no
      // index-like properties. This avoids allocating a new StringPrimitive and
      // uniquing it below.
      return false;
    }
  }

  // Convert the string to a SymbolID
  LAZY_TO_IDENTIFIER(runtime, nameValHandle, id);

  // Look for a named property with this name.
  if (JSObject::getOwnNamedDescriptor(
          selfHandle,
          runtime,
          id,
          JSObject::Helper::castToNamedPropertyDescriptorRef(desc))) {
    return true;
  }

  if (LLVM_LIKELY(
          !JSObject::Helper::flags(*selfHandle).indexedStorage &&
          !selfHandle->isLazy() && !selfHandle->isProxyObject())) {
    return false;
  }
  MutableHandle<StringPrimitive> strPrim{runtime};

  // If we have indexed storage, perform potentially expensive conversions
  // to array index and check it.
  if (JSObject::Helper::flags(*selfHandle).indexedStorage) {
    // If the name is a valid integer array index, store it here.
    OptValue<uint32_t> arrayIndex;

    // Try to convert the property name to an array index.
    TO_ARRAY_INDEX(runtime, nameValHandle, strPrim, arrayIndex);

    if (arrayIndex) {
      auto res = JSObject::Helper::getOwnIndexedPropertyFlags(
          selfHandle.get(), runtime, *arrayIndex);
      if (res) {
        desc.flags = *res;
        desc.flags.indexed = 1;
        desc.slot = *arrayIndex;
        return true;
      }
    }
    return false;
  }

  if (selfHandle->isLazy()) {
    JSObject::initializeLazyObject(runtime, selfHandle);
    return JSObject::getOwnComputedPrimitiveDescriptor(
        selfHandle, runtime, nameValHandle, ignoreProxy, desc);
  }

  assert(selfHandle->isProxyObject() && "descriptor flags are impossible");
  if (ignoreProxy == JSObject::IgnoreProxy::Yes) {
    return false;
  }
  return JSProxy::getOwnProperty(
      selfHandle, runtime, nameValHandle, desc, nullptr);
}