void JSObject::_snapshotAddEdgesImpl(GCCell *cell, GC *gc, HeapSnapshot &snap) {
  auto *const self = vmcast<JSObject>(cell);

  // Add the prototype as a property edge, so it's easy for JS developers to
  // walk the prototype chain on their own.
  if (self->parent_) {
    snap.addNamedEdge(
        HeapSnapshot::EdgeType::Property,
        // __proto__ chosen for similarity to V8.
        "__proto__",
        gc->getObjectID(self->parent_));
  }

  HiddenClass::forEachPropertyNoAlloc(
      self->clazz_.get(gc->getPointerBase()),
      gc->getPointerBase(),
      [self, gc, &snap](SymbolID id, NamedPropertyDescriptor desc) {
        if (InternalProperty::isInternal(id)) {
          // Internal properties aren't user-visible, skip them.
          return;
        }
        // Else, it's a user-visible property.
        GCHermesValue &prop =
            namedSlotRef(self, gc->getPointerBase(), desc.slot);
        const llvh::Optional<HeapSnapshot::NodeID> idForProp =
            gc->getSnapshotID(prop);
        if (!idForProp) {
          return;
        }
        std::string propName = gc->convertSymbolToUTF8(id);
        // If the property name is a valid array index, display it as an
        // "element" instead of a "property". This will put square brackets
        // around the number and sort it numerically rather than
        // alphabetically.
        if (auto index = ::hermes::toArrayIndex(propName)) {
          snap.addIndexedEdge(
              HeapSnapshot::EdgeType::Element,
              index.getValue(),
              idForProp.getValue());
        } else {
          snap.addNamedEdge(
              HeapSnapshot::EdgeType::Property, propName, idForProp.getValue());
        }
      });
}