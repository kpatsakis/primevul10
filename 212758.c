CallResult<bool> JSObject::hasNamedOrIndexed(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    SymbolID name) {
  if (LLVM_UNLIKELY(selfHandle->flags_.indexedStorage)) {
    const auto strView =
        runtime->getIdentifierTable().getStringView(runtime, name);
    if (auto nameAsIndex = toArrayIndex(strView)) {
      if (haveOwnIndexed(selfHandle.get(), runtime, *nameAsIndex)) {
        return true;
      }
      if (selfHandle->flags_.fastIndexProperties) {
        return false;
      }
    }
    // Here we have indexed properties but the symbol was not stored in the
    // indexedStorage.
    // Fall through to getNamed().
  }
  return hasNamed(selfHandle, runtime, name);
}