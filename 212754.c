void ObjectDeserialize(Deserializer &d, CellKind kind) {
  assert(kind == CellKind::ObjectKind && "Expected JSObject");
  void *mem = d.getRuntime()->alloc</*fixedSize*/ true>(cellSize<JSObject>());
  auto *obj = new (mem) JSObject(d, &JSObject::vt.base);

  d.endObject(obj);
}