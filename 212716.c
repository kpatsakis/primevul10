JSObject::JSObject(Deserializer &d, const VTable *vtp)
    : GCCell(&d.getRuntime()->getHeap(), vtp) {
  d.readData(&flags_, sizeof(ObjectFlags));
  d.readRelocation(&parent_, RelocationKind::GCPointer);
  d.readRelocation(&clazz_, RelocationKind::GCPointer);
  if (d.readInt<uint8_t>()) {
    propStorage_.set(
        d.getRuntime(),
        ArrayStorage::deserializeArrayStorage(d),
        &d.getRuntime()->getHeap());
  }

  auto overlapSlots = d.readInt<uint8_t>();
  for (size_t i = overlapSlots; i < JSObject::DIRECT_PROPERTY_SLOTS; i++) {
    d.readHermesValue(&directProps()[i]);
  }
}