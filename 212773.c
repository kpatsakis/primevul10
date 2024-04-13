CallResult<bool> JSObject::putNamedOrIndexed(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    SymbolID name,
    Handle<> valueHandle,
    PropOpFlags opFlags) {
  if (LLVM_UNLIKELY(selfHandle->flags_.indexedStorage)) {
    // Note that getStringView can be satisfied without materializing the
    // Identifier.
    const auto strView =
        runtime->getIdentifierTable().getStringView(runtime, name);
    if (auto nameAsIndex = toArrayIndex(strView)) {
      return putComputed_RJS(
          selfHandle,
          runtime,
          runtime->makeHandle(HermesValue::encodeNumberValue(*nameAsIndex)),
          valueHandle,
          opFlags);
    }
    // Here we have indexed properties but the symbol was not index-like.
    // Fall through to putNamed().
  }
  return putNamed_RJS(selfHandle, runtime, name, valueHandle, opFlags);
}