CallResult<bool> JSObject::getOwnComputedPrimitiveDescriptor(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    Handle<> nameValHandle,
    JSObject::IgnoreProxy ignoreProxy,
    ComputedPropertyDescriptor &desc) {
  SymbolID id{};

  return getOwnComputedPrimitiveDescriptorImpl(
      selfHandle, runtime, nameValHandle, ignoreProxy, id, desc);
}