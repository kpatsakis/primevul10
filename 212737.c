std::string JSObject::getNameIfExists(PointerBase *base) {
  // Try "displayName" first, if it is defined.
  if (auto nameVal = tryGetNamedNoAlloc(
          this, base, Predefined::getSymbolID(Predefined::displayName))) {
    if (auto *name = dyn_vmcast<StringPrimitive>(*nameVal)) {
      return converter(name);
    }
  }
  // Next, use "name" if it is defined.
  if (auto nameVal = tryGetNamedNoAlloc(
          this, base, Predefined::getSymbolID(Predefined::name))) {
    if (auto *name = dyn_vmcast<StringPrimitive>(*nameVal)) {
      return converter(name);
    }
  }
  // There is no other way to access the "name" property on an object.
  return "";
}