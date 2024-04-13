std::string JSObject::_snapshotNameImpl(GCCell *cell, GC *gc) {
  auto *const self = vmcast<JSObject>(cell);
  return self->getHeuristicTypeName(gc);
}