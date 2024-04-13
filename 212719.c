CallResult<bool> JSObject::addOwnProperty(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    SymbolID name,
    DefinePropertyFlags dpFlags,
    Handle<> valueOrAccessor,
    PropOpFlags opFlags) {
  /// Can we add more properties?
  if (!selfHandle->isExtensible() && !opFlags.getInternalForce()) {
    if (opFlags.getThrowOnError()) {
      return runtime->raiseTypeError(
          TwineChar16("Cannot add new property '") +
          runtime->getIdentifierTable().getStringViewForDev(runtime, name) +
          "'");
    }
    return false;
  }

  PropertyFlags flags{};

  // Accessors don't set writeable.
  if (dpFlags.isAccessor()) {
    dpFlags.setWritable = 0;
    flags.accessor = 1;
  }

  // Override the default flags if specified.
  if (dpFlags.setEnumerable)
    flags.enumerable = dpFlags.enumerable;
  if (dpFlags.setWritable)
    flags.writable = dpFlags.writable;
  if (dpFlags.setConfigurable)
    flags.configurable = dpFlags.configurable;
  flags.internalSetter = dpFlags.enableInternalSetter;

  if (LLVM_UNLIKELY(
          addOwnPropertyImpl(
              selfHandle, runtime, name, flags, valueOrAccessor) ==
          ExecutionStatus::EXCEPTION)) {
    return ExecutionStatus::EXCEPTION;
  }

  return true;
}