PropertyAccessor::PropertyAccessor(Deserializer &d)
    : GCCell(&d.getRuntime()->getHeap(), &vt) {
  d.readRelocation(&getter, RelocationKind::GCPointer);
  d.readRelocation(&setter, RelocationKind::GCPointer);
}