void JSObject::serializeObjectImpl(
    Serializer &s,
    const GCCell *cell,
    unsigned overlapSlots) {
  auto *self = vmcast<const JSObject>(cell);
  s.writeData(&self->flags_, sizeof(ObjectFlags));
  s.writeRelocation(self->parent_.get(s.getRuntime()));
  s.writeRelocation(self->clazz_.get(s.getRuntime()));
  // propStorage_ : GCPointer<PropStorage> is also ArrayStorage. Serialize
  // *propStorage_ with this JSObject.
  bool hasArray = (bool)self->propStorage_;
  s.writeInt<uint8_t>(hasArray);
  if (hasArray) {
    ArrayStorage::serializeArrayStorage(
        s, self->propStorage_.get(s.getRuntime()));
  }

  // Record the number of overlap slots, so that the deserialization code
  // doesn't need to keep track of it.
  s.writeInt<uint8_t>(overlapSlots);
  for (size_t i = overlapSlots; i < JSObject::DIRECT_PROPERTY_SLOTS; i++) {
    s.writeHermesValue(self->directProps()[i]);
  }
}