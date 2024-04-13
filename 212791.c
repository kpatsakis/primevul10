std::string JSObject::getHeuristicTypeName(GC *gc) {
  PointerBase *const base = gc->getPointerBase();
  if (auto constructorVal = tryGetNamedNoAlloc(
          this, base, Predefined::getSymbolID(Predefined::constructor))) {
    if (auto *constructor = dyn_vmcast<JSObject>(*constructorVal)) {
      auto name = constructor->getNameIfExists(base);
      // If the constructor's name doesn't exist, or it is just the object
      // constructor, attempt to find a different name.
      if (!name.empty() && name != "Object")
        return name;
    }
  }

  std::string name = getVT()->base.snapshotMetaData.defaultNameForNode(this);
  // A constructor's name was not found, check if the object is in dictionary
  // mode.
  if (getClass(base)->isDictionary()) {
    return name + "(Dictionary)";
  }

  // If it's not an Object, the CellKind is most likely good enough on its own
  if (getKind() != CellKind::ObjectKind) {
    return name;
  }

  // If the object isn't a dictionary, and it has only a few property names,
  // make the name based on those property names.
  std::vector<std::string> propertyNames;
  HiddenClass::forEachPropertyNoAlloc(
      getClass(base),
      base,
      [gc, &propertyNames](SymbolID id, NamedPropertyDescriptor) {
        if (InternalProperty::isInternal(id)) {
          // Internal properties aren't user-visible, skip them.
          return;
        }
        propertyNames.emplace_back(gc->convertSymbolToUTF8(id));
      });
  // NOTE: One option is to sort the property names before truncation, to
  // reduce the number of groups; however, by not sorting them it makes it
  // easier to spot sets of objects with the same properties but in different
  // orders, and thus find HiddenClass optimizations to make.

  // For objects with a lot of properties but aren't in dictionary mode yet,
  // keep the number displayed small.
  constexpr int kMaxPropertiesForTypeName = 5;
  bool truncated = false;
  if (propertyNames.size() > kMaxPropertiesForTypeName) {
    propertyNames.erase(
        propertyNames.begin() + kMaxPropertiesForTypeName, propertyNames.end());
    truncated = true;
  }
  // The final name should look like Object(a, b, c).
  if (propertyNames.empty()) {
    // Don't add parentheses for objects with no properties.
    return name;
  }
  name += "(";
  bool first = true;
  for (const auto &prop : propertyNames) {
    if (!first) {
      name += ", ";
    }
    first = false;
    name += prop;
  }
  if (truncated) {
    // No need to check for comma edge case because this only happens for
    // greater than one property.
    static_assert(
        kMaxPropertiesForTypeName >= 1,
        "Property truncation should not happen for 0 properties");
    name += ", ...";
  }
  name += ")";
  return name;
}