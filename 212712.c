CallResult<bool> JSObject::internalSetter(
    Handle<JSObject> selfHandle,
    Runtime *runtime,
    SymbolID name,
    NamedPropertyDescriptor /*desc*/,
    Handle<> value,
    PropOpFlags opFlags) {
  if (vmisa<JSArray>(selfHandle.get())) {
    if (name == Predefined::getSymbolID(Predefined::length)) {
      return JSArray::setLength(
          Handle<JSArray>::vmcast(selfHandle), runtime, value, opFlags);
    }
  }

  llvm_unreachable("unhandled property in Object::internalSetter()");
}