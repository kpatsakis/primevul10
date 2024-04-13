CallResult<HermesValue> PropertyAccessor::create(
    Runtime *runtime,
    Handle<Callable> getter,
    Handle<Callable> setter) {
  void *mem = runtime->alloc(cellSize<PropertyAccessor>());
  return HermesValue::encodeObjectValue(
      new (mem) PropertyAccessor(runtime, *getter, *setter));
}