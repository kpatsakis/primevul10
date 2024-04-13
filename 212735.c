void PropertyAccessorDeserialize(Deserializer &d, CellKind kind) {
  assert(kind == CellKind::PropertyAccessorKind && "Expected PropertyAccessor");
  void *mem = d.getRuntime()->alloc(cellSize<PropertyAccessor>());
  auto *cell = new (mem) PropertyAccessor(d);
  d.endObject(cell);
}