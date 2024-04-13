void PropertyAccessorSerialize(Serializer &s, const GCCell *cell) {
  auto *self = vmcast<const PropertyAccessor>(cell);
  s.writeRelocation(self->getter.get(s.getRuntime()));
  s.writeRelocation(self->setter.get(s.getRuntime()));
  s.endObject(cell);
}